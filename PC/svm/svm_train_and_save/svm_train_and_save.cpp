#include <fstream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

#include <json.hpp>

#include "print_vector.h"

#define DATA_DIMENSIONS 7 // there are 7 Hu moments

using namespace cv;
using json = nlohmann::json;

int main()
{
    // Prepare file with input training data
    std::ifstream input_training_data("../data/dataset_training_labeled_canny150.json");
    // Read training data to JSON object
    json training_data_json;
    input_training_data >> training_data_json;

    std::cout << "Training data set: " << training_data_json.dump(4) << std::endl;
    std::cout << "Data set size: " << training_data_json.size() << std::endl;

    // Set up training data
    std::vector<float> labels;
    std::vector<float> training_data;
    unsigned int labeled_dataset_size = 0;

    // For each acorn read its learning label and Hu moments
    for (auto element : training_data_json)
{
        // Ignore not labeled elements
        std::cout << "element[\"label\"]" << element["label"] << std::endl;
        if (element["label"] == 0)
        {
            continue;
        }
        else
        {
            // Read training label
            labels.push_back(element["label"]);
            labeled_dataset_size++;
        }

        // Read training data
        std::vector<float> training_element = string_to_vector(element["hu_moments"]);

        std::cout << "training_data" << std::endl;
        print_vector(training_data);
        // Put new element at the end of vector containing all data. The data will be then reshaped.
        if (training_element.size() > 0)
        {
            training_data.insert(
                                 training_data.end(),
                                 training_element.begin(),
                                 training_element.end()
                                );
        }
        std::cout << "\nTraining element ";
        print_vector(training_data);
        std::cout << "\n\n";
}

    // Create a matrix from vector and reshape matrix to have one training element in one row
    // It is done this way because there's no conversion between vector<vector<float> > and cv::Mat
    assert(DATA_DIMENSIONS*labeled_dataset_size == training_data.size());
    Mat training_data_mat = Mat(training_data).reshape(0, labeled_dataset_size);

    // Put training data labels in format for OpenCV SVM
    print_vector(labels);
    assert(labels.size() == labeled_dataset_size);
    Mat labels_mat(labels);

    // Set up SVM's parameters
    CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::LINEAR;
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

    // Train the SVM
    CvSVM SVM;
    SVM.train(training_data_mat, labels_mat, Mat(), Mat(), params);

    SVM.save("trained.svm");
}

