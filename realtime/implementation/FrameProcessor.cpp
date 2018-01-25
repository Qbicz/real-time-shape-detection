#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "FrameProcessor.h"
#include "Events.h"
#include "FSM.h"

#include "center_of_mass.h"

constexpr int HU_MOMENTS_NUM = 7;

std::mutex synchro::mx;
std::condition_variable synchro::cvar;

using namespace synchro;
using namespace cv;

FrameProcessor::FrameProcessor(OrientationDetectionFsm& fsm, const Mat& src_background, const ProcessingParams& params)
    : fsm(fsm),
      canny_threshold(params.canny_threshold),
      upper_canny_threshold(params.upper_canny_threshold),
      sobel_kernel_size(params.sobel_kernel_size)
{
    cvtColor(src_background, bg, COLOR_BGR2GRAY);

    the_thread = std::thread(&FrameProcessor::main_thread, this);
}

void FrameProcessor::signal_capture_end()
{
    capture_ended = true;
}

void FrameProcessor::push_frame(const Mat& captured_frame)
{
    {
        std::lock_guard<std::mutex> lk(mx);
        framesBuffer.emplace(captured_frame);
    }

    cvar.notify_one();
}

void FrameProcessor::main_thread()
{
    std::cout << "main_thread\n";

    while(true)
    {
        std::unique_lock<std::mutex> lk(mx);
        cvar.wait(lk, [this]{ return !framesBuffer.empty() || capture_ended; });

        if(framesBuffer.empty())
        {
            //end processing as soon as the capture is ended and there are no more frames left
            break;
        }

        Mat  src = framesBuffer.front();
        framesBuffer.pop();

        lk.unlock();

        bool acorn_detected = false;

        // Args
        int label;
        const bool label_images = false;
        const bool show_images = true;

        //
        // Feature
        //
        Mat gray_image;
        cvtColor(src, gray_image, COLOR_BGR2GRAY);

        // Subtract background to remove not interesting objects
        Mat diff(gray_image);
        absdiff(gray_image, this->bg, diff);

        float center_of_mass_position = center_of_mass_position_compute(diff, canny_threshold, upper_canny_threshold, label, sobel_kernel_size, label_images, show_images, acorn_detected);
        if (acorn_detected)
        {
            std::cout << "Center of mass position: " << center_of_mass_position << "\n";
        }

        //
        // Recognition
        //


        Orientation orientation = Orientation::LEFT_ORIENTED;

        if(acorn_detected)
        {
            fsm.dispatch(AcornDetected(orientation));
        }
        else
        {
            fsm.dispatch(AcornMissing());
        }
    }
}

FrameProcessor::~FrameProcessor()
{
    if(the_thread.joinable())
         the_thread.join();
}

