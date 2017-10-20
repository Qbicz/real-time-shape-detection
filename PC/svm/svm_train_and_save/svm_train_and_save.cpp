#include <fstream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#include <json.hpp>
#include "common.h"

using namespace cv;
using json = nlohmann::json;

int main()
{
    // Read input training data to JSON object
    std::ifstream input_training_data("../data/dataset_training.json");
    json training_data_json;
    input_training_data >> training_data_json;
 
    std::cout << "Training data set: " << training_data_json << std::endl;
    std::cout << "Data set size: " << training_data_json.size() << std::endl;

    // Set up training data
    std::vector<float> labels;
            
    // Read training label
    for (auto element : training_data_json)
    {
        labels.push_back(element["label"]);
    }

    Mat training_data_mat = prepare_data_mat(training_data_json);

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

