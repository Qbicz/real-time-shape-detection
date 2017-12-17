#ifndef __PRINT_VECTOR_H__
#define __PRINT_VECTOR_H__

#include <vector>
#include <string>

void print_vector(const std::vector<float> &vect);
std::string vector_to_string(const std::vector<float>& vect);
std::vector<float> string_to_vector(const std::string& str);

std::vector<float> vector_subset(const std::vector<float>& vect, const std::vector<int> chosen_elements);

#endif /* __PRINT_VECTOR_H__ */

