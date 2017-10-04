#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#include <json.hpp>

using namespace cv;
using json = nlohmann::json;

int main()
{
    // Read input training data to JSON object
    std::ifstream input_training_data("../dataset_training.json");
    json training_data;
    input_training_data >> training_data;
 
    std::cout << training_data;

    // Set up training data
    float labels[4] = {1.0, 1.0, -1.0, 1.0};
    Mat labelsMat(4, 1, CV_32FC1, labels);

    float trainingData[4][2] = { {501, 10}, {255, 10}, {501, 255}, {10, 501} };
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

