#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;
//int thresh = 140;
//RNG rng(12345);

// Function declarations
double preprocessAndComputeOrientation(Mat& src, const int thresh = 140);
void drawAxis(Mat&, Point, Point, Scalar, const float);
double getOrientation(const vector<Point> &, Mat&);

int main(int, char**)
{
    printf("main starting\n");
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
    {
	    printf("camera 0 not found!\n");
	    return -1;
    }
    printf("Camera opened!");
    
#if 0
    double fps = cap.get(CV_CAP_PROP_FPS);
    printf("FPS = %f\n", fps);    
#endif

    Mat gray;
    Mat src;

    for(;;)
    {
        cap >> src; // get a new frame from camera
        preprocessAndComputeOrientation(src);
#if 0
        cvtColor(src, gray, COLOR_BGR2GRAY);

        //printf("Image captured and converted to grayscale!");

        /// Convert image to binary
        Mat bw;
        //threshold(gray, bw, 50, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
        /// Detect edges using canny
        Canny( gray, bw, thresh, thresh*2, 3 );
        imshow("bw", bw);
        //! [contours]
        // Find all the contours in the thresholded image
        vector<Vec4i> hierarchy;
        vector<vector<Point> > contours;
        findContours(bw, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        for (size_t i = 0; i < contours.size(); ++i)
        //for (size_t i = 0; i < 5; ++i)
        {
            // Calculate the area of each contour
            double area = contourArea(contours[i]);
            // Ignore contours that are too small or too large
            if (area < 1e3 || area > 1e6) continue;
            // <Nokia 920 back>

            printf("area = %f for contour %u\n", area, i);

            // Draw each contour only for visualisation purposes
            //printf("drawContours");
            drawContours(src, contours, static_cast<int>(i), Scalar(0, 0, 255), 2, 8, hierarchy, 0);
            // Find the orientation of each shape
            //printf("getOrientation");
            getOrientation(contours[i], src);
        }
        //! [contours]
#endif
        /// Show in a window
        //imshow( "Contours", contours);
        imshow( "Orientation", src);

        //imshow("edges", canny_output);
        if(waitKey(30) >= 0) break;


    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

// in callback only change threshold. perform all operations in loop

double preprocessAndComputeOrientation(Mat& src, const int thresh)
{
    // Return value
    double angle;

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    //printf("Image captured and converted to grayscale!");

    /// Convert image to binary
    Mat bw;
    //threshold(gray, bw, 50, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    /// Detect edges using canny
    Canny( gray, bw, thresh, thresh*2, 3 );
    imshow("bw", bw);
    //! [contours]
    // Find all the contours in the thresholded image
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(bw, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    Rect boundingBox;
    Mat roi;

    for (size_t i = 0; i < contours.size(); ++i)
    //for (size_t i = 0; i < 5; ++i)
    {
        // Calculate the area of each contour
        double area = contourArea(contours[i]);
        // Ignore contours that are too small or too large
        if (area < 1e3 || area > 1e6) continue;
        
        // Region of interest is around object - actual contour
        boundingBox = boundingRect(contours[i]);
        // Take the subset of bw image
        roi = bw(boundingBox);

        // Pad the ROI to multiple of 8 columns for OpenCL
        int numOfColumnsNeeded = 0;
	    if(roi.cols % 8 != 0)
	    {
	        numOfColumnsNeeded = 8 - (roi.cols % 8);
	        printf("Unable to perform openCL computations - image width must be a multiplication of 8 and needs additional %d columns!\n", numOfColumnsNeeded);
	        Mat pad = Mat::zeros(roi.rows, numOfColumnsNeeded, CV_8UC1);
	        hconcat(roi, pad, roi);
	    }

        // Draw a box
        rectangle(src, boundingBox, Scalar(0, 255, 255), 2);

    	// Show ROI
    	imshow("ROI", roi);

    	// Compute Hu moments of a single contour
    	Moments mu = moments( roi, false );
		double hu[7];
      	HuMoments(mu, hu);
      	printf("Hu invariants for contour %zu:\n", i);
	    for( int i = 0; i < 7; i++ )
	    	printf("[%d]=%.4e ", i+1, hu[i]);
	    printf("\n");

		/// Show 7th Hu moment as an arrow from the mass center
      	mc = Point2f( mu.m10/mu[i].m00 , mu[i].m01/mu[i].m00 );  
      	Point hu_orient = Point(static_cast<int>(mc[i].x) , static_cast<int>(mc[i].y+scale*(hu[6]))); // 7th Hu moment as a vertical arrow
      	drawAxis(drawing, mc[i], hu_orient, Scalar(255, 255, 0), 5);


        // Draw each contour only for visualisation purposes
        //printf("drawContours");
        drawContours(src, contours, static_cast<int>(i), Scalar(0, 0, 255), -1, 8, hierarchy, 0);
        // Find the orientation of each shape
        //printf("getOrientation");
        angle = getOrientation(contours[i], src);
        angle = angle * 180.0 / CV_PI;
        printf("area = %f for contour %u rotated towards %f degrees \n", area, i, angle);
    }
    //! [contours]


    return angle;
}


/**
 * @function drawAxis
 */
void drawAxis(Mat& img, Point p, Point q, Scalar colour, const float scale = 10)
{
//! [visualization1]
    double angle;
    double hypotenuse;
    angle = atan2( (double) p.y - q.y, (double) p.x - q.x ); // angle in radians
    hypotenuse = sqrt( (double) (p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));
//    double degrees = angle * 180 / CV_PI; // convert radians to degrees (0-180 range)
//    cout << "Degrees: " << abs(degrees - 180) << endl; // angle in 0-360 degrees range

    // Here we lengthen the arrow by a factor of scale
    q.x = (int) (p.x - scale * hypotenuse * cos(angle));
    q.y = (int) (p.y - scale * hypotenuse * sin(angle));
    line(img, p, q, colour, 1, CV_AA);

    // create the arrow hooks
    p.x = (int) (q.x + 9 * cos(angle + CV_PI / 4));
    p.y = (int) (q.y + 9 * sin(angle + CV_PI / 4));
    line(img, p, q, colour, 1, CV_AA);

    p.x = (int) (q.x + 9 * cos(angle - CV_PI / 4));
    p.y = (int) (q.y + 9 * sin(angle - CV_PI / 4));
    line(img, p, q, colour, 1, CV_AA);
//! [visualization1]
}

/**
 * @function getOrientation
 */
double getOrientation(const vector<Point> &pts, Mat &img)
{
//! [pca]
    //Construct a buffer used by the pca analysis
    int sz = static_cast<int>(pts.size());
    Mat data_pts = Mat(sz, 2, CV_64FC1);
    for (int i = 0; i < data_pts.rows; ++i)
    {
        data_pts.at<double>(i, 0) = pts[i].x;
        data_pts.at<double>(i, 1) = pts[i].y;
    }

    //Perform PCA analysis
    PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);

    //Store the center of the object
    Point cntr = Point(static_cast<int>(pca_analysis.mean.at<double>(0, 0)),
                      static_cast<int>(pca_analysis.mean.at<double>(0, 1)));

    //Store the eigenvalues and eigenvectors
    vector<Point2d> eigen_vecs(2);
    vector<double> eigen_val(2);
    for (int i = 0; i < 2; ++i)
    {
        eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
                                pca_analysis.eigenvectors.at<double>(i, 1));

        eigen_val[i] = pca_analysis.eigenvalues.at<double>(0, i);
    }

//! [pca]
//! [visualization]
    // Draw the principal components
    circle(img, cntr, 3, Scalar(255, 0, 255), 2);
    Point p1 = cntr + 0.02 * Point(static_cast<int>(eigen_vecs[0].x * eigen_val[0]), static_cast<int>(eigen_vecs[0].y * eigen_val[0]));
    Point p2 = cntr - 0.02 * Point(static_cast<int>(eigen_vecs[1].x * eigen_val[1]), static_cast<int>(eigen_vecs[1].y * eigen_val[1]));
    drawAxis(img, cntr, p1, Scalar(0, 255, 0), 1);
    drawAxis(img, cntr, p2, Scalar(255, 255, 0), 5);

    double angle = atan2(eigen_vecs[0].y, eigen_vecs[0].x); // orientation in radians
//! [visualization]

    return angle;
}

