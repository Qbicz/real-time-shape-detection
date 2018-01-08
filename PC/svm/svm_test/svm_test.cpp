#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <fstream>

#include "svm.h"
#include "vector_op.h"

//see https://docs.opencv.org/2.4/modules/ml/doc/support_vector_machines.html#cvsvm-predict for reference

using namespace cv;
using json = nlohmann::json;

int main(int argc, char* argv[])
{
    const char* keys = {
        "{h  | help             | false | print this message }"
        "{t  | trained_svm      |       | trained SVM file }"
        "{d  | testing_dataset  |       | data used for testing of SVM recognition }"
        "{f  | feature          |       | feature used for SVM testing. Can be 'hu' or 'com' }"
    };

    CommandLineParser parser(argc, argv, keys);

    if( argc < 5 || parser.get<bool>("help"))
    {
        parser.printParams();
        exit(1);
    }

    std::string svmFile         = parser.get<std::string>("trained_svm");
    std::string testing_dataset = parser.get<std::string>("testing_dataset");
    std::string feature         = parser.get<std::string>("feature");

    CvSVM svm;
    std::cout << "Loading SVM from file " << svmFile  << "... ";
    svm.load(svmFile.c_str());

    std::cout << "SUCCESS!\n";

    for (int i = 0; i < svm.get_support_vector_count(); ++i)
    {
        auto v = svm.get_support_vector(i);
        std::cout << "Support vector #" << i << " " << *v << "\n";
    }

    // Read input training data to JSON object
    std::ifstream test_data(testing_dataset);
    json test_data_json;
    test_data >> test_data_json;

    std::cout << "Test points: " << test_data_json.size() << "\n";
    std::cout << "Testing trained svm... ";

    std::vector<int> interesting_moments = {};
    std::cout << (svm_test(svm, test_data_json, feature, interesting_moments) ? "OK" : "NOT OK") << std::endl;

    return 0;
}
