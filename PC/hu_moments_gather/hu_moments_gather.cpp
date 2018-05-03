#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <json.hpp>
#include "vector_op.h"

#define HU_MOMENTS_NUM  7

using namespace cv;
using json = nlohmann::json;

// Type for Hu moments
typedef std::vector<float> hu_moments_t;

// Functions
static hu_moments_t hu_moments_compute(const Mat& src, const int canny_threshold, const int upper_threshold);

// Configuration constants
const std::string output_json_file = "../svm/data/dataset_training_hu_needs_label.json";

// CLI arguments
int canny_threshold;
bool show_images_cli_arg;
bool label_images_cli_arg;
int upper_canny_threshold;
int sobel_kernel_size;
int label = 0;

int main(int argc, char** argv)
{
    const char* keys = {
        "{h | help  | false | print this message     }"
        "{i | input |       | input images list file }"
        "{l | lower | 150   | lower canny threshold  }"
        "{u | upper | 300   | upper canny threshold  }"
        "{k | kernel| 3     | sobel kernel size  }"
        "{m | mark  | false | mark images with acorn orientation }"
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
    label_images_cli_arg = parser.get<bool>("mark");

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

    // For each image, extract Hu moments and add them to JSON array
    auto json_objects = json::array();
    for (std::string image_file : images_to_process)
    {
        Mat src_image = imread(image_file, 1);

        hu_moments_t hu_moments = hu_moments_compute(src_image, canny_threshold, upper_canny_threshold);

        print_vector(hu_moments);

        // Append object with Hu moments to json
        // A human needs to fill in the "label" afterwards to prepare for learning the classifier
        json hu_json;

        std::string hu_moments_string = vector_to_string(hu_moments);

        std::cout << image_file << " ->  Hu moments string: " << hu_moments_string << std::endl;

        hu_json["image"] = image_file;
        hu_json["hu_moments"] = hu_moments_string;
        hu_json["label"] = label;

        label = 0;

        json_objects.push_back(hu_json);
    }
    // Write to JSON file
    std::ofstream output_training_data(output_json_file);

    output_training_data << json_objects;
    std::cout << "Computed Hu moments and saved them to a JSON." << std::endl;
    std::cout << "Please open the JSON and fill the labels for learning" << std::endl;

    return 0;
}

hu_moments_t hu_moments_compute(const Mat& src, const int canny_threshold, const int upper_threshold)
{
    hu_moments_t hu_vector;

    Mat gray_image, gray_bg;
    cvtColor(src, gray_image, COLOR_BGR2GRAY);

    Mat bg = imread("bg.png");
    cvtColor(bg, gray_bg, COLOR_BGR2GRAY);

    Mat diff(gray_image);
    absdiff(gray_image, gray_bg, diff);

    // Detect edges using Canny
    Mat edges_image;
    Canny(diff, edges_image, canny_threshold, upper_threshold, sobel_kernel_size);
//    Canny(gray_image, edges_image, canny_threshold, upper_threshold, sobel_kernel_size);

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
        double area = contourArea(contours[i]);
        // Fix contours that are too small or too large
        const int imageArea = edges_image.rows * edges_image.cols;

        if (area / imageArea < 0.001)
        {
            std::cout << "Area too small for contour.\n";

            continue;
        }

        std::cout << "area = " << area << " for contour " << i << '\n';

        if(show_images_cli_arg)
        {
            if(label_images_cli_arg)
            {
                imshow("Filled contour", diff);
                waitKey(50);
                std::cout << "Please provide orientation for acorn\n";
                std::cout << "1: right, -1: left, 0: no decision\n";
                std::cin >> label;
            }
        }

        // Compute Hu moments the filled shape
        Moments mu = moments(contours[i], true);
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

