#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

//for demo purposes
#include <chrono>
// if producer is faster (shorter time in pace) than consumer, the buffer will grow.
// Otherwise the buffer will remain with only one element.
const int producer_pace_ms = 700;
const int consumer_pace_ms = 1000;


std::mutex mx;
std::condition_variable cv;
std::queue<int> q;

const int FRAME_SIZE = 100;

struct OpenCVMat {
    int frame[FRAME_SIZE][FRAME_SIZE];
    int index = 0;
};

std::queue<OpenCVMat> framesBuffer;

void grabNextFrame(OpenCVMat& capturedFrame)
{
    for(int i = 0; i < FRAME_SIZE; i++)
    {
        for(int j = 0; j < FRAME_SIZE; j++)
        {
            //simulate video capture. matrix is square for simplicity
            capturedFrame.frame[i][j] = i*j*j - i - j;
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(producer_pace_ms));
}

void processFrame(OpenCVMat& frame)
{
    std::cout << "Processing frame: " << frame.index << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(consumer_pace_ms));
}

void producer() {
    OpenCVMat capturedFrame;
    static int framesCounter = 0;

    while(true)
    {
        capturedFrame.index = framesCounter++;
        grabNextFrame(capturedFrame);

        std::lock_guard<std::mutex> lk(mx);
        framesBuffer.emplace(capturedFrame);
        std::cout << "inserting " << capturedFrame.index << " current size: " << framesBuffer.size() << "\n";

        cv.notify_all();
    }
}

void consumer() {
    OpenCVMat capturedFrame;
    while (true)
    {
        std::unique_lock<std::mutex> lk(mx);
        cv.wait(lk, []{ return !framesBuffer.empty(); });
        capturedFrame = framesBuffer.front();
        framesBuffer.pop();
        lk.unlock();

        processFrame(capturedFrame);
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
}




