#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include "vector_op.h"
#include "center_of_mass.h"

using namespace cv;

float center_of_mass_position_compute(const Mat& img_diff,
                                      const int canny_threshold,
                                      const int upper_threshold,
                                      int& label,
                                      const int sobel_kernel_size,
                                      const bool label_images,
                                      const bool show_images,
                                      bool& acorn_detected)
{
    float center_of_mass_position = 0.0f;

    // Detect edges using Canny
    Mat edges_image;
    Canny(img_diff, edges_image, canny_threshold, upper_threshold, sobel_kernel_size);
    // Canny(gray_image, edges_image, canny_threshold, upper_threshold, sobel_kernel_size);
#if TODO
    if(show_images && !label_images)
    {
        imshow("Filled contour", img_diff);
        waitKey(0);
        imshow("Filled contour", edges_image);
        waitKey(0);
    }
#endif
    // Find all the contours in the thresholded image
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(edges_image, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    for (size_t i = 0; i < contours.size(); ++i)
    {
        // Calculate the area of each contour
        float area = contourArea(contours[i]);
        // Fix contours that are too small or too large
        const int imageArea = edges_image.rows * edges_image.cols;

        if ((area/imageArea) < 0.002)
        {
            // std::cout << "Area " << (area/imageArea) << " too small for contour, discarding.\n";
            continue;
        }

        acorn_detected = true;
        std::cout << "area = " << area << " for contour " << i << '\n';

        /* Prepare empty image on which the shape will be drawn.
           The image will be used by cv::moments() and thus has to be
           1-channel or with the channel of interest selected. */
        Mat shape(img_diff);
        shape = (Scalar(0));
        // Function fillPoly() expects an array of polygons - we pass only one polygon
        const Point* contour_vertices[1] = {&contours[i][0]};
        int vertices_num[1] = {static_cast<int>(contours[i].size())};
        // Get shape by filling a contour
        fillPoly(shape, contour_vertices, vertices_num, 1, Scalar(255));

        // Compute Center of Mass for the filled shape
        Moments mu = moments( shape, false );
        Point2f mc = Point2f( mu.m10/mu.m00, mu.m01/mu.m00 );
        // Get length and ends of object in horizonstal axis
        Rect bounding_box = boundingRect(contours[i]);
        float left_end = bounding_box.x;
        float right_end = bounding_box.x + bounding_box.width;

        std::cout << "Center: " << mc << "at image with " << shape.cols << " columns." << std::endl;
#if TODO
        if(show_images)
        {
            namedWindow("Filled contour", WINDOW_NORMAL);
            // draw center of mass of the shape
            Mat drawing(shape);
            circle(drawing, mc, 4, Scalar(0), -1, 8, 0);
            imshow("Filled contour", drawing);
            waitKey(100);

            if(label_images)
            {
                std::cout << "Please provide orientation for acorn\n";
                std::cout << "1: right, -1: left, 0: no decision\n";
                std::cin >> label;
            }
            else
            {
                label = 0;
            }
        }
#endif
        float object_length = right_end - left_end;
        center_of_mass_position = (mc.x - left_end) / object_length;

        std::cout << "Center of mass is at " << 100*center_of_mass_position << "% of length\n";

        // Return the first successfully computed result
        return center_of_mass_position;
    }

}
