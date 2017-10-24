#include <iostream>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include "print_vector.h"

#define HU_MOMENTS_NUM 7

using namespace cv;

// TODO: move to separate file included by all modules working with Hu moments
typedef std::vector<float> hu_moments_t;

hu_moments_t hu_moments_compute(Mat& src, const int thresh);

int canny_threshold;


int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("Usage: ./moments input_png [init_canny_threshold]\n");
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

hu_moments_t hu_moments_compute(Mat& src, const int thresh)
{
    double hu[HU_MOMENTS_NUM];
    Mat gray_image;
    cvtColor(src, gray_image, COLOR_BGR2GRAY);

    // Detect edges using Canny
    Mat edges_image;
    // TODO: do not use magic values
    Canny( gray_image, edges_image, canny_threshold, canny_threshold*2, 3 );
    
    // Find all the contours in the thresholded image
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(edges_image, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    for (size_t i = 0; i < contours.size(); ++i)
    {
        // Calculate the area of each contour
        double area = contourArea(contours[i]);
        // Ignore contours that are too small or too large
        if (area < 1e3 || area > 1e6) continue;

        printf("area = %f for contour %lu\n", area, i);

        // Get shape by filling a contour
        Point contour_vertices[1][contours[i].size()];
        //contour_vertices[0] = &contours[i];
        //Mat shape;
        //fillPoly(shape, contour_vertices, (contours[i].size()), 1, Scalar(255, 255, 255));

        // Compute Hu moments of a single contour
        Moments mu = moments( contours[i], false );
        HuMoments(mu, hu);
        printf("Hu invariants for contour %zu:\n", i);
        for( int j = 0; j < HU_MOMENTS_NUM; j++ )
            printf("[%d]=%.4e ", j+1, hu[j]);
        printf("\n");

    }
    hu_moments_t hu_vector;
    hu_vector.assign(hu, hu + HU_MOMENTS_NUM);
    return hu_vector;
}

