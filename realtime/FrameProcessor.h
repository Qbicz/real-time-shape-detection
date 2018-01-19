#ifndef REALTIME_FRAMEPROCESSOR_H_
#define REALTIME_FRAMEPROCESSOR_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;

extern std::queue<Mat> framesBuffer;
extern std::mutex mx;
extern std::condition_variable cvar;

struct ProcessingParams
{
    const int canny_threshold;
    const int upper_canny_threshold;
    const int sobel_kernel_size;
};

class OrientationDetectionFsm;

class FrameProcessor {
public:
    void signal_capture_end();
    FrameProcessor(OrientationDetectionFsm& fsm, const Mat& background, const ProcessingParams& params);
    virtual ~FrameProcessor();
private:
    void main_thread();
    std::thread the_thread;
    OrientationDetectionFsm& fsm;
    const Mat& bg;
    const int canny_threshold;
    const int upper_canny_threshold;
    const int sobel_kernel_size;
    bool capture_ended = false;
};

#endif /* REALTIME_FRAMEPROCESSOR_H_ */
