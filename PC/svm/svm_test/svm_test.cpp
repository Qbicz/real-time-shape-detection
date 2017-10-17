#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <fstream>

#include "common.h"

//see https://docs.opencv.org/2.4/modules/ml/doc/support_vector_machines.html#cvsvm-predict for reference

#define DATA_DIMENSIONS 2 // for Hu moments, it will be 7

using namespace cv;
using json = nlohmann::json;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cout << "Please provide trained svm file\n";
        exit(1);
    }

    std::string svmFile(argv[1]);
    std::cout << "Loading SVM from file " << svmFile  << "\n";

    CvSVM svm;
    
    svm.load(svmFile.c_str());
    auto c = svm.get_support_vector_count();

    std::cout << "Successfully loaded with parameters:\n";
    
    for (int i = 0; i < c; ++i)
    {
        const float* v = svm.get_support_vector(i);
        std::cout << "Support vector #" << i << " " << *v << "\n";
    }

// Read input training data to JSON object
    std::ifstream test_data("../data/dataset_training.json");
    json training_data_json;
    test_data >> training_data_json;
 
    std::cout << "Test data set: " << training_data_json << std::endl;
    std::cout << "Data set size: " << training_data_json.size() << std::endl;

    // Set up training data
    std::vector<float> labels;
    std::vector<float> training_data;

    for (auto element : training_data_json)
    {
        // Read training data
        std::vector<float> training_element = svm_build_single_element(element);
        // Put new element at the end of vector containing all data
        training_data.insert(training_data.end(),
                             training_element.begin(),
                             training_element.end()
                            );
        print_vector(training_data);

        // Read training label
        labels.push_back(element["label"]);
    }

    // Create a matrix from vector and reshape matrix to have one training element in one row
    // It is done this way because there's no conversion between vector<vector<float> > and cv::Mat
    assert(DATA_DIMENSIONS*training_data_json.size() == training_data.size());
    Mat training_data_mat = Mat(training_data).reshape(0, training_data_json.size());

    // Put training data labels in format for OpenCV SVM
    print_vector(labels);
    assert(labels.size() == training_data_json.size());
    Mat labels_mat(labels);

    svm.predict(training_data_mat, labels_mat);




    return 0;
}
