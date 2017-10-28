#include <iostream>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include "print_vector.h"

#define SHOW_IMAGES     1
#define HU_MOMENTS_NUM  7

using namespace cv;

// Type for Hu moments
typedef std::vector<float> hu_moments_t;

hu_moments_t hu_moments_compute(Mat& src, const int thresh);

int canny_threshold;
const int threshold_ratio = 2;
const int sobel_kernel_size = 3;


int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cout << "Usage: ./moments <input_image> <init_canny_threshold>\n";
        exit(1);
    }

    if(argc == 3)
    {
        canny_threshold = atoi(argv[2]);
    }

    Mat src_image = imread(argv[1], 1);
    hu_moments_t hu_moments = hu_moments_compute(src_image, canny_threshold);

    std::cout << "Hu moments:" << std::endl;
    print_vector(hu_moments);
    
    return 0;
}

hu_moments_t hu_moments_compute(Mat& src, const int canny_threshold)
{
    hu_moments_t hu_vector;

    Mat gray_image;
    cvtColor(src, gray_image, COLOR_BGR2GRAY);

    // Detect edges using Canny
    Mat edges_image;
    Canny(gray_image, edges_image, canny_threshold, canny_threshold*threshold_ratio, sobel_kernel_size);
    
    // Find all the contours in the thresholded image
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(edges_image, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    for (size_t i = 0; i < contours.size(); ++i)
    {
        // Calculate the area of each contour
        double area = contourArea(contours[i]);
        // Ignore contours that are too small or too large
        if (area < 1e3 || area > 1e7) continue;

        printf("area = %f for contour %lu\n", area, i);

        /* Prepare empty image on which the shape will be drawn.
           The image will be used by cv::moments() and thus has to be
           1-channel or with the channel of interest selected. */
        Mat shape(gray_image);
        shape = (Scalar(0,0,0));
        // Function fillPoly() expects an array of polygons - we pass only one polygon
        const Point* contour_vertices[1] = {&contours[i][0]};
        int vertices_num[1] = {static_cast<int>(contours[i].size())};
        // Get shape by filling a contour
        fillPoly(shape, contour_vertices, vertices_num, 1, Scalar(255, 255, 255));

        if(SHOW_IMAGES)
        {
            imshow("Filled contour", shape);
            waitKey(0);
        }

        // Compute Hu moments the filled shape
        Moments mu = moments( shape, false );
        double hu[HU_MOMENTS_NUM];
        HuMoments(mu, hu);

        printf("Hu invariants for contour %zu:\n", i);
        for( int j = 0; j < HU_MOMENTS_NUM; j++ )
            printf("[%d]=%.4e ", j+1, hu[j]);
        printf("\n");
        
        // We return Hu moments for the first contour of a given area
        assert(hu);
        hu_vector.assign(hu, hu + HU_MOMENTS_NUM);
        break;
    }

    return hu_vector;
}

