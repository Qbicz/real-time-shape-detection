#ifndef __PRINT_VECTOR_H__
#define __PRINT_VECTOR_H__

#include <vector>
#include <string>

template <class T> void print_vector(const std::vector<T> &vect);
template <class T> std::string vector_to_string(const std::vector<T>& vect);
template <class T> std::vector<T> string_to_vector(const std::string& str);

template <class T> std::vector<T> vector_subset(const std::vector<T>& vect, const std::vector<int>& chosen_elements);

#endif /* __PRINT_VECTOR_H__ */

