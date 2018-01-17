#ifndef REALTIME_FRAMESUPPLIER_H_
#define REALTIME_FRAMESUPPLIER_H_

#include <thread>

class FrameSupplier{
public:
    /* Explicitly using the default constructor to
     * underline the fact that it does get called */
    FrameSupplier() : the_thread() {}
    ~FrameSupplier(){
        stop_thread = true;
        if(the_thread.joinable())
            the_thread.join();
    }
    void start(){
        // This will start the thread. Notice move semantics!
        the_thread = std::thread(&FrameSupplier::ThreadMain, this);
    }
private:
    bool stop_thread = false;
    std::thread the_thread;
    void ThreadMain()
    {
        while(!stop_thread)
        {
            std::this_thread::sleep_for( std::chrono::seconds(1) );
            std::cout << "Inside frame supplier\n";
        }
    }
};




#endif /* REALTIME_FRAMESUPPLIER_H_ */
