#ifndef COMMON_H
#define COMMON_H

#include "json.hpp"

#define DATA_DIMENSIONS 2 // for Hu moments, it will be 7

using json =nlohmann::json;

std::vector<float> svm_build_single_element(json &element, bool debug = false)
{
    std::vector<float> training_element;
    if(debug)
        std::cout << "element: " << element << std::endl; 
    training_element.push_back(element["x"]);
    training_element.push_back(element["y"]);
    return training_element;
}

void print_vector(std::vector<float> &vect)
{
    std::cout << "std::vector [ ";
    for (auto vect_elem : vect)
    {
        std::cout << vect_elem << " ";
    }
    std::cout << "]\n";
}

cv::Mat prepare_data_mat(const nlohmann::json& test_data_json)
{
    // Set up training data
    std::vector<float> training_data;

    for (auto element : test_data_json)
    {
        // Read test data
        std::vector<float> training_element = svm_build_single_element(element);
        // Put new element at the end of vector containing all data
        training_data.insert(training_data.end(),
                             training_element.begin(),
                             training_element.end()
                            );
    }

    // Create a matrix from vector and reshape matrix to have one training element in one row
    // It is done this way because there's no conversion between vector<vector<float> > and cv::Mat
    assert(DATA_DIMENSIONS*test_data_json.size() == training_data.size());
    
    return cv::Mat(training_data).reshape(0, test_data_json.size());
}

bool test_svm_with_data(const CvSVM& svm, const nlohmann::json& test_data_json)
{
    const cv::Mat test_data_mat = prepare_data_mat(test_data_json);
    
    for(auto  i = 0; i < test_data_mat.rows; ++i)
    {
        int expected = test_data_json[i]["label"];
        int predicted = svm.predict(test_data_mat.row(i));
        
        if(expected != predicted)
        {
            std::cout << "Incorrect result at row " << i << " with data: "  
                      << test_data_mat.row(i) << "\n";
            std::cout << "Predicted: " << predicted << " expected: " << expected << "\n";
            return false;
        }
    }
    return true;
}

#endif
