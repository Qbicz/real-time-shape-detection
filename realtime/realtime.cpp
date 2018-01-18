#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include "FrameSupplier.h"
#include "Events.h"
#include "FSM.h"

FSM_INITIAL_STATE(OrientationDetectionFsm, Idle)

using namespace std;
using namespace cv;

constexpr int HU_MOMENTS_NUM = 7;

int main(int argc, char* argv[])
{
    const char* keys = {
        "{h | help        | false    | print this message }"
        "{d | device      | 0        | device number }"
        "{w | width       | 1200     | image width }"
        "{h | height      | 720      | image height }"
        "{v | video       |          | use video file as input }"
        "{l | lower       | 150      | lower canny threshold  }"
        "{u | upper       | 300      | upper canny threshold  }"
        "{k | kernel      | 3        | sobel kernel size  }"
        "{f | fps         | 25       | frames per second  }"
        "{e | empty       | 0        | frames allowed to be empty  }"
    };

    CommandLineParser parser(argc, argv, keys);

    if(parser.get<bool>("help"))
    {
        std::cout << "Realtime acorn evaluation system - orientation detection module.\n";
        std::cout << "Usage:\n";
        parser.printParams();
        return EXIT_FAILURE;
    }

    const auto canny_threshold = parser.get<int>("lower");
    const auto upper_canny_threshold = parser.get<int>("upper");
    const auto sobel_kernel_size = parser.get<int>("kernel");
    const auto device_number = parser.get<int>("device");
    const auto frame_width = parser.get<int>("width");
    const auto frame_height = parser.get<int>("height");
    const auto fps = parser.get<int>("fps");
    const auto allowed_empty_frames_param = parser.get<int>("empty");
    const auto video_input_file = parser.get<std::string>("video");

    const bool streaming = video_input_file == "";

    VideoCapture cap = streaming ? VideoCapture(device_number) : VideoCapture(video_input_file);

    if(streaming)
    {
        cap.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);

        if(!cap.isOpened())  // check if we succeeded
        {
            std::cout << "Device " << device_number << " not found\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "Image width: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;
    std::cout << "Image height: " << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << std::endl;

    Mat src, gray, edges, bg, diff;
    bool acorn_shape_detected = false;

    cap.read(src);
    cvtColor(src, bg, COLOR_BGR2GRAY);

    OrientationDetectionFsm fsm;
    fsm.initialize();
    fsm.set_allowed_empty_frames(allowed_empty_frames_param);

    //the object will hold the thread inside.
    //we use RAII so no need to manually join() the thread
    FrameSupplier frame_supplier(cap, fps);
    frame_supplier.start();

    while (true)
    {
        std::unique_lock<std::mutex> lk(mx);

        cvar.wait(lk, [&]{ return (!framesBuffer.empty() || !frame_supplier.capturing()); });

        if(framesBuffer.empty())
            break;

        src = framesBuffer.front();
        framesBuffer.pop();

        lk.unlock();

        cvtColor(src, gray, COLOR_BGR2GRAY);
        absdiff(gray, bg, diff);

        // Detect edges using Canny
        Canny(diff, edges, canny_threshold, upper_canny_threshold, sobel_kernel_size);

        imshow("Edges", edges);

        vector<Vec4i> hierarchy;
        vector<vector<Point> > contours;
        findContours(edges, contours, hierarchy, CV_RETR_EXTERNAL/*CV_RETR_LIST*/, CV_CHAIN_APPROX_NONE);
        acorn_shape_detected = false;

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

        Orientation orientation = LEFT_ORIENTED; //TODO: update it with SVM decision

        if(acorn_shape_detected)
        {
            fsm.dispatch(AcornDetected(orientation));
        }
        else
        {
            fsm.dispatch(AcornMissing());
        }
    }

    return EXIT_SUCCESS;
}
