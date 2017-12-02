#include <fstream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

#include <json.hpp>

#include "print_vector.h"

#define DATA_DIMENSIONS 7 // there are 7 Hu moments

using namespace cv;
using json = nlohmann::json;

static std::vector<float> svm_build_single_element(json &element)
{
    std::vector<float> training_element;
    std::cout << "element: " << element << std::endl; 
    training_element.push_back(element["hu"]);
    return training_element;
}

int main()
{
    // Read input training data to JSON object
    std::ifstream input_training_data("../data/dataset_training_labeled_canny150.json");
    json training_data_json;
    input_training_data >> training_data_json;
 
    std::cout << "Training data set: " << training_data_json << std::endl;
    std::cout << "Data set size: " << training_data_json.size() << std::endl;

    // Set up training data
    std::vector<float> labels;
    std::vector<float> training_data;

    for (auto element : training_data_json)
    {
        // Ignore not labeled elements
        if (element["label"] == 0)
        {
            continue;
        }
        else
        {
            // Read training label
            labels.push_back(element["label"]);
        }

        // Read training data
        std::vector<float> training_element = svm_build_single_element(element);
        // Put new element at the end of vector containing all data
        training_data.insert(training_data.end(),
                             training_element.begin(),
                             training_element.end()
                            );
        print_vector(training_data);
    }

    // Create a matrix from vector and reshape matrix to have one training element in one row
    // It is done this way because there's no conversion between vector<vector<float> > and cv::Mat
    assert(DATA_DIMENSIONS*training_data_json.size() == training_data.size());
    Mat training_data_mat = Mat(training_data).reshape(0, training_data_json.size());

    // Put training data labels in format for OpenCV SVM
    print_vector(labels);
    assert(labels.size() == training_data_json.size());
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

