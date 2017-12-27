#ifndef __SVM_H__
#define __SVM_H__

#include "json.hpp"

using json = nlohmann::json;

std::vector<int> svm_prepare_labels_from_json(const json& data_json);
cv::Mat svm_prepare_data_from_json(const json& data_json, const std::vector<int>& interesting_moments_indexes);
bool svm_test(const CvSVM& svm, const json& test_data_json, const std::vector<int>& interesting_moments_indexes);

#endif
