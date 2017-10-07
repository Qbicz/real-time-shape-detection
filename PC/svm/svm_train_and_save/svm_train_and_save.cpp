#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#include <json.hpp>

#define DATA_DIMENSIONS 2 // for Hu moments, it will be 7

using namespace cv;
using json = nlohmann::json;

int main()
{
    // Read input training data to JSON object
    std::ifstream input_training_data("../dataset_training.json");
    json training_data;
    input_training_data >> training_data;
 
    std::cout << "Training data set: " << training_data << std::endl;
    std::cout << "Data set size: " << training_data.size() << std::endl;

    // Set up training data
    float labels[training_data.size()] = {1.0, 1.0, -1.0, 1.0};
    float trainingData[training_data.size()][DATA_DIMENSIONS] = { {501, 10}, {255, 10}, {501, 255}, {10, 501} };
    for (auto element : training_data)
    {
        std::cout << "element: " << element << std::endl; 
        
    }

    // Put training data in format for OpenCV SVM
    Mat labelsMat(4, 1, CV_32FC1, labels);
    Mat trainingDataMat(4, 2, CV_32FC1, trainingData);

    // Set up SVM's parameters
    CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::LINEAR;
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

    // Train the SVM
    CvSVM SVM;
    SVM.train(trainingDataMat, labelsMat, Mat(), Mat(), params);

    SVM.save("trained.svm");
}

