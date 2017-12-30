#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <json.hpp>
#include "vector_op.h"

using namespace cv;
using json = nlohmann::json;

// Functions
float center_of_mass_position_compute(const Mat& src, const int canny_threshold, const int upper_threshold, int& label);

// Configuration constants
const std::string output_json_file = "../svm/data/dataset_training_com.json";

// CLI arguments
int canny_threshold;
bool show_images_cli_arg;
bool label_images_cli_arg;
int upper_canny_threshold;
int sobel_kernel_size;

int main(int argc, char** argv)
{
    const char* keys = {
        "{h | help  | false | print this message     }"
        "{i | input |       | input images list file }"
        "{l | lower | 150   | lower canny threshold  }"
        "{u | upper | 300   | upper canny threshold  }"
        "{k | kernel| 3     | sobel kernel size  }"
        "{m | label | false | label images with acorn orientation during program run }"
        "{s | show  | false | show images  }"
    };

    CommandLineParser parser(argc, argv, keys);

    if( argc < 2 || parser.get<bool>("help"))
    {
        parser.printParams();
        exit(1);
    }

    canny_threshold = parser.get<int>("lower");
    upper_canny_threshold = parser.get<int>("upper");
    sobel_kernel_size = parser.get<int>("kernel");
    show_images_cli_arg = parser.get<bool>("show");
    label_images_cli_arg = parser.get<bool>("label");

    std::cout << "Show images: " << (show_images_cli_arg ? "yes" : "no") << std::endl;

    // Read a list of images from file. The list file has to be in the same folder as the program for the imported paths to be valid
    std::vector<std::string> images_to_process;
    std::ifstream images_list(parser.get<std::string>("input"));

    std::cout << "Adding images ";
    for (std::string line; std::getline(images_list, line); )
    {
        images_to_process.push_back(line);
        std::cout << " " << line;
    }
    std::cout << '\n';

    // For each image, extract Center of Mass displacement and add them to JSON array
    auto json_objects = json::array();
    for (std::string image_file : images_to_process)
    {
        int label = 0;
        Mat src_image = imread(image_file, 1);
        std::cout << "File: " << image_file << std::endl;

        // label is passed by reference and modified
        float center_of_mass_position = center_of_mass_position_compute(src_image, canny_threshold, upper_canny_threshold, label);

        std::cout << "Object's center of mass horizontal position: " << center_of_mass_position;

        // Append object with Hu moments to json
        // A human needs to fill in the "label" afterwards to prepare for learning the classifier
        json com_json;

        com_json["image"] = image_file;
        com_json["center_of_mass_position"] = center_of_mass_position;
        com_json["label"] = label;

        json_objects.push_back(com_json);
    }
    // Write to JSON file
    std::ofstream output_training_data(output_json_file);

    output_training_data << json_objects;
    std::cout << "Computed mass centers and saved them to a JSON: " << output_json_file << std::endl;
    if (label_images_cli_arg)
        std::cout << "Data in JSON is labeled." << std::endl;
    else
        std::cout << "Please open the JSON and fill the labels for learning." << std::endl;

    return 0;
}

float center_of_mass_position_compute(const Mat& src, const int canny_threshold, const int upper_threshold, int& label)
{
    float center_of_mass_position = 0.0f;

    Mat gray_image, gray_bg;
    cvtColor(src, gray_image, COLOR_BGR2GRAY);

    Mat bg = imread("bg.png");
    cvtColor(bg, gray_bg, COLOR_BGR2GRAY);

    // Subtract background to remove not interesting objects
    Mat diff(gray_image);
    absdiff(gray_image, gray_bg, diff);

    // Detect edges using Canny
    Mat edges_image;
    Canny(diff, edges_image, canny_threshold, upper_threshold, sobel_kernel_size);
    // Canny(gray_image, edges_image, canny_threshold, upper_threshold, sobel_kernel_size);

    if(show_images_cli_arg && !label_images_cli_arg)
    {
        imshow("Filled contour", diff);
        waitKey(0);
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
        float area = contourArea(contours[i]);
        // Fix contours that are too small or too large
        const int imageArea = edges_image.rows * edges_image.cols;

        if ((area/imageArea) < 0.002)
        {
            std::cout << "Area " << (area/imageArea) << " too small for contour, discarding.\n";
            continue;
        }

        std::cout << "area = " << area << " for contour " << i << '\n';

        /* Prepare empty image on which the shape will be drawn.
           The image will be used by cv::moments() and thus has to be
           1-channel or with the channel of interest selected. */
        Mat shape(gray_image);
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

        if(show_images_cli_arg)
        {
            namedWindow("Filled contour", WINDOW_NORMAL);
            // draw center of mass of the shape
            Mat drawing(shape);
            circle(drawing, mc, 4, Scalar(0), -1, 8, 0);
            imshow("Filled contour", drawing);
            waitKey(100);

            if(label_images_cli_arg)
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

        float object_length = right_end - left_end;
        center_of_mass_position = (mc.x - left_end) / object_length;

        std::cout << "Center of mass is at " << 100*center_of_mass_position << "% of length\n";

        // Return the first successfully computed result
        return center_of_mass_position;
    }

}

