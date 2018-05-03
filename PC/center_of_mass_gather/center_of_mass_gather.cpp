#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <json.hpp>
#include "vector_op.h"
#include "center_of_mass.h"

using namespace cv;
using json = nlohmann::json;

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

