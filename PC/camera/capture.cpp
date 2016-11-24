#include "opencv2/opencv.hpp"

using namespace cv;
int thresh = 160;
RNG rng(12345);

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    Mat gray;
    namedWindow("edges",1);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, gray, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);

        //getMoments();
        Mat canny_output;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        /// Detect edges using canny
        Canny( gray, canny_output, thresh, thresh*2, 3 );
        /// Find contours
        findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

        /// Get the moments
        vector<Moments> mu(contours.size() );
        for( int i = 0; i < contours.size(); i++ )
           { mu[i] = moments( contours[i], false ); }

        /// Compute Hu moments - use to tell the difference between mirrored objects/rotated 180 degrees
        for( int i = 0; i < contours.size(); i++)
        {
            Moments mom = mu[i];
            double hu[7];
            HuMoments(mom, hu);
            printf("Hu invariants for contour %d:\n", i);
            for( int i = 0; i < 7; i++ )
               printf("[%d]=%.4e ", i+1, hu[i]);
            printf("\n");
        }

        // TODO: draw 7th Hu moment as an arrow on the frame

        ///  Get the mass centers:
        vector<Point2f> mc( contours.size() );
        for( int i = 0; i < contours.size(); i++ )
           { mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); }

        /// Draw contours
        Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
        for( int i = 0; i< contours.size(); i++ )
           {
             Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
             drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
             circle( drawing, mc[i], 4, color, -1, 8, 0 );
           }

        /// Show in a window
        namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
        imshow( "Contours", drawing );

        imshow("edges", canny_output);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

// in callback only change threshold. perform all operations in loop

void getMoments()
{
    // all moments code
}
