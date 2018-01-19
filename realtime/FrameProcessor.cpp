#include "FrameProcessor.h"

#include "Events.h"
#include "FSM.h"

constexpr int HU_MOMENTS_NUM = 7;

std::queue<Mat> framesBuffer;
std::mutex mx;
std::condition_variable cvar;

FrameProcessor::FrameProcessor(OrientationDetectionFsm& fsm, const Mat& background, const ProcessingParams& params)
    : fsm(fsm),
      bg(background),
      canny_threshold(params.canny_threshold),
      upper_canny_threshold(params.upper_canny_threshold),
      sobel_kernel_size(params.sobel_kernel_size)
{
    the_thread = std::thread(&FrameProcessor::main_thread, this);
}

void FrameProcessor::signal_capture_end()
{
    capture_ended = true;
}

void FrameProcessor::main_thread()
{
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

        Mat gray, diff, edges;

        cvtColor(src, gray, COLOR_BGR2GRAY);
        absdiff(gray, bg, diff);

        // Detect edges using Canny
        Canny(diff, edges, canny_threshold, upper_canny_threshold, sobel_kernel_size);

        imshow("Edges", edges);

        vector<Vec4i> hierarchy;
        vector<vector<Point> > contours;
        findContours(edges, contours, hierarchy, CV_RETR_EXTERNAL/*CV_RETR_LIST*/, CV_CHAIN_APPROX_NONE);
        bool acorn_shape_detected = false;

        for (size_t i = 0; i < contours.size(); ++i)
        {
            // Calculate the area of each contour
            double area = contourArea(contours[i]);
            // Fix contours that are too small or too large
            const int imageArea = edges.rows * edges.cols;

            if (area / imageArea < 0.001)
            {
                continue;
            }
            else
            {
                acorn_shape_detected = true;
            }

            // Compute Hu moments the filled shape
            Moments mu = moments(contours[i], false);
            double hu[HU_MOMENTS_NUM];
            HuMoments(mu, hu);
        }

        Orientation orientation = Orientation::LEFT_ORIENTED;

        if(acorn_shape_detected)
        {
            fsm.dispatch(AcornDetected(orientation));
        }
        else
        {
            fsm.dispatch(AcornMissing());
        }
    }
}

FrameProcessor::~FrameProcessor() {
    if(the_thread.joinable())
         the_thread.join();
}

