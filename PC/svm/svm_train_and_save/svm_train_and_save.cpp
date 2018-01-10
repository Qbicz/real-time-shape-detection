#include <fstream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

#include <json.hpp>

#include "svm.h"
#include "vector_op.h"

const int JSON_TAB_SIZE = 4;

using namespace cv;
using json = nlohmann::json;

int main(int argc, char** argv)
{
    // Parse command line arguments
    std::string input_filename;
    std::string learning_feature;

    const char* keys = {
        "{h | help                | false | print this message     }"
        "{i | input_training_data |       | input training JSON file }"
        "{f | feature             |       | feature used for learning & classification. Can be 'com' or 'hu'}"
    };

    CommandLineParser parser(argc, argv, keys);

    if( argc < 2 || parser.get<bool>("help"))
    {
        parser.printParams();
        exit(1);
    }

    input_filename = parser.get<std::string>("input_training_data");
    learning_feature = parser.get<std::string>("feature");

    // Prepare file with input training data
    std::ifstream input_training_data(input_filename);
    // Read training data to JSON object
    json training_data_json;
    input_training_data >> training_data_json;

    std::cout << "Training data set: " << training_data_json.dump(JSON_TAB_SIZE) << std::endl;
    std::cout << "Data set size: " << training_data_json.size() << std::endl;
    Mat training_data_mat;
    std::vector<int> interesting_moments;

    // Get training data in format for OpenCV SVM
    training_data_mat = svm_prepare_data_from_json(training_data_json, learning_feature, interesting_moments);
    std::cout << "Training data Mat:\n" << training_data_mat << std::endl;

    // Put training labels in format for OpenCV SVM
    std::vector<int> labels = svm_prepare_labels_from_json(training_data_json);
    const Mat labels_mat(labels);
    std::cout << "Labels Mat:\n" << labels_mat << std::endl;

    // Set up SVM's parameters
    CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::LINEAR;
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 500, 1e-6);

    // Train the SVM
    CvSVM svm;
    svm.train_auto(training_data_mat, labels_mat, Mat(), Mat(), params);
    // TODO: minimize computational complexity of train_auto by constraining parameters

    svm.save("auto_trained.svm");

    std::cout << "Information about trained SVM:" << std::endl;
    for (int i = 0; i < svm.get_support_vector_count(); ++i)
    {
        auto v = svm.get_support_vector(i);
        std::cout << "Support vector #" << i << " " << *v << "\n";
    }
}

