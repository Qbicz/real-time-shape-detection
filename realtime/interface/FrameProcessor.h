#ifndef REALTIME_FRAMEPROCESSOR_H_
#define REALTIME_FRAMEPROCESSOR_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <opencv2/core/core.hpp>

namespace synchro {
    extern std::mutex mx;
    extern std::condition_variable cvar;
}

struct ProcessingParams
{
    const int canny_threshold;
    const int upper_canny_threshold;
    const int sobel_kernel_size;
};

class OrientationDetectionFsm;

class FrameProcessor {
public:
    void push_frame(const cv::Mat& captured_frame);
    void signal_capture_end();
    FrameProcessor(OrientationDetectionFsm& fsm, const cv::Mat& background, const ProcessingParams& params);
    virtual ~FrameProcessor();
private:
    void main_thread();
    std::thread the_thread;
    OrientationDetectionFsm& fsm;
    cv::Mat bg;
    const int canny_threshold;
    const int upper_canny_threshold;
    const int sobel_kernel_size;
    bool capture_ended = false;
    std::queue<cv::Mat> framesBuffer;
};

#endif /* REALTIME_FRAMEPROCESSOR_H_ */
