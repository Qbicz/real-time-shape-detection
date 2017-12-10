#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <json.hpp>
#include "print_vector.h"

#define HU_MOMENTS_NUM  7

using namespace cv;
using json = nlohmann::json;

// Type for Hu moments
typedef std::vector<float> hu_moments_t;

// Functions
static hu_moments_t hu_moments_compute(Mat& src, const int thresh);

// Configuration constants
const int threshold_ratio = 2;
const int sobel_kernel_size = 3;
const std::string output_json_file = "../svm/data/dataset_training_hu_needs_label.json";

// CLI arguments
int canny_threshold;         // required argument
int show_images_cli_arg = 0; // optional argument: by default don't show images


int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cout << "Usage: ./hu_moments_gather <input_images_list_file> <init_canny_threshold> [show_images]\n";
        exit(1);
    }

    if(argc >= 3)
    {
        canny_threshold = atoi(argv[2]);
    }
    if(argc == 4)
    {
        show_images_cli_arg = atoi(argv[3]);
        std::cout << "Show images: " << (show_images_cli_arg ? "yes" : "no") << std::endl;
    }

    // Read a list of images from file. The list file has to be in the same folder as the program for the imported paths to be valid
    std::vector<std::string> images_to_process;
    std::ifstream images_list(argv[1]);

    std::cout << "Adding images ";
    for (std::string line; std::getline(images_list, line); )
    {
        images_to_process.push_back(line);
        std::cout << " " << line;
    }
    std::cout << '\n';

    // For each image, extract Hu moments and add them to JSON array
    auto json_objects = json::array();
    for (std::string image_file : images_to_process)
    {
        Mat src_image = imread(image_file, 1);

        hu_moments_t hu_moments = hu_moments_compute(src_image, canny_threshold);

        print_vector(hu_moments);

        // Append object with Hu moments to json
        // A human needs to fill in the "label" afterwards to prepare for learning the classifier
        json hu_json;

        std::string hu_moments_string = vector_to_string(hu_moments);

        std::cout << image_file << " ->  Hu moments string: " << hu_moments_string << std::endl;

        hu_json["image"] = image_file;
        hu_json["hu_moments"] = hu_moments_string;
        hu_json["label"] = 0;

        json_objects.push_back(hu_json);
    }
    // Write to JSON file
    std::ofstream output_training_data(output_json_file);

    output_training_data << json_objects;
    std::cout << "Computed Hu moments and saved them to a JSON." << std::endl;
    std::cout << "Please open the JSON and fill the labels for learning" << std::endl;

    return 0;
}

vector<Point> largestContourConvexHull(const vector<vector<Point> >& contours )
{
    vector<Point> result, pts;
    std::pair<double, vector<Point>> maxArea;

    for ( size_t i = 0; i < contours.size(); i++)
    {
        vector<Point> tmpPts;
        for ( size_t j = 0; j< contours[i].size(); j++)
        {
            tmpPts.push_back(contours[i][j]);
            pts.push_back(contours[i][j]);
        }

        auto contourAreaSize = contourArea(tmpPts);

        if(contourAreaSize > maxArea.first)
        {
            maxArea.first = contourAreaSize;
            maxArea.second = tmpPts;
        }
    }

//  Calculate for the largest contour only
    convexHull(maxArea.second, result );
    return result;
}

hu_moments_t hu_moments_compute(Mat& src, const int canny_threshold)
{
    hu_moments_t hu_vector;

    Mat gray_image;
    cvtColor(src, gray_image, COLOR_BGR2GRAY);

    // Detect edges using Canny
    Mat edges_image;
    Canny(gray_image, edges_image, canny_threshold, canny_threshold*threshold_ratio, sobel_kernel_size);

    if(show_images_cli_arg)
    {
        // Images displayed only when invoked with [show_images=1]
        imshow("Filled contour", edges_image);
        waitKey(0);
    }
    // Find all the contours in the thresholded image
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(edges_image, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    for (size_t i = 0; i < contours.size(); ++i)
    {
        // Calculate the area of each contour
        double area = contourArea(contours[i]);
        // Fix contours that are too small or too large
        if (area < 1e3 || area > 1e7)
        {
            std::cout << "Area too small for contour, trying convex hull...\n";

            vector<Point> convexHullPoints =  largestContourConvexHull(contours);

            contours[i] = convexHullPoints;
            area = contourArea(contours[i]);
            if (area < 1e3 || area > 1e7)
            {
                std::cout << "Failed to set the convex hull!\n";
                continue;
            }
        }

        std::cout << "area = " << area << " for contour " << i << '\n';

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

        if(show_images_cli_arg)
        {
            // Images displayed only when invoked with [show_images=1]
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

