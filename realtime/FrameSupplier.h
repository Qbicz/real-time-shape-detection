#ifndef REALTIME_FRAMESUPPLIER_H_
#define REALTIME_FRAMESUPPLIER_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;

//TODO: move it to .cpp and leave only "extern ..." here
std::queue<Mat> framesBuffer;
std::mutex mx;
std::condition_variable cvar;

class FrameSupplier{
public:
    FrameSupplier(VideoCapture& cap, int fps) : capture(cap), fps(fps), the_thread() {}
    ~FrameSupplier()
    {
        if(the_thread.joinable())
            the_thread.join();
    }
    void start()
    {
        the_thread = std::thread(&FrameSupplier::thread_main, this);
    }
    bool capturing()
    {
        return !capture_ended;
    }
private:
    VideoCapture& capture;
    int fps;
    bool capture_ended = false;
    std::thread the_thread;
    void thread_main()
    {
        Mat captured_frame;
        while(capture.read(captured_frame))
        {
            //any pre-processing on frame
            //...
            //push the frame to the queue
            {
                std::lock_guard<std::mutex> lk(mx);
                framesBuffer.emplace(captured_frame);
            }

            cvar.notify_one();

            if(waitKey(1000 / fps) >= 0) break;
        }

        capture_ended = true;
        cvar.notify_one(); //notify about the end
    }
};

#endif /* REALTIME_FRAMESUPPLIER_H_ */
