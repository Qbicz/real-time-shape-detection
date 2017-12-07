#include <fstream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

#include <json.hpp>

#include "common.h"
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

    // Get training data in format for OpenCV SVM
    const Mat training_data_mat = svm_read_data_from_json(training_data_json);
    std::cout << "Training data Mat:\n" << training_data_mat << std::endl;

    // Put training labels in format for OpenCV SVM
    std::vector<int> labels = svm_read_labels_from_json(training_data_json);
    const Mat labels_mat(labels);
    std::cout << "Labels Mat:\n" << labels_mat << std::endl;

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

