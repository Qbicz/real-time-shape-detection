#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include "FrameProcessor.h"
#include "Events.h"
#include "FSM.h"

FSM_INITIAL_STATE(OrientationDetectionFsm, Idle)

using namespace std;
using namespace cv;

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

    Mat src, gray, edges, background, diff;
    bool acorn_shape_detected = false;

    cap.read(src);
    cvtColor(src, background, COLOR_BGR2GRAY);

    OrientationDetectionFsm fsm;
    fsm.initialize();
    fsm.set_allowed_empty_frames(allowed_empty_frames_param);

    ProcessingParams params {canny_threshold, upper_canny_threshold, sobel_kernel_size};
    FrameProcessor processor(fsm, background, params);
    //the FrameProcessor object will hold the thread inside.
    //we use RAII so no need to manually join() the thread

    while(cap.read(src))
    {
        {
            std::lock_guard<std::mutex> lk(mx);
            framesBuffer.emplace(src);
        }

        cvar.notify_one();

        if(waitKey(1000 / fps) == 'q') break;
    }

    processor.signal_capture_end();
    cvar.notify_one();

    return EXIT_SUCCESS;
}
