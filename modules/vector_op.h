#ifndef __PRINT_VECTOR_H__
#define __PRINT_VECTOR_H__

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>

template <class T>
void print_vector(const std::vector<T>& vect)
{
    std::cout << "std::vector [ ";
    for (auto vect_elem : vect)
    {
        std::cout << vect_elem << " ";
    }
    std::cout << "]\n";
}

template <class T>
std::string vector_to_string(const std::vector<T>& vect)
{
    std::ostringstream oss;

    if (!vect.empty())
    {
        // Convert all but the last element to avoid a trailing ","
        std::copy(vect.begin(), vect.end()-1,
            std::ostream_iterator<T>(oss, ","));

        // Now add the last element with no delimiter
        oss << vect.back();

        return oss.str();
    }
    else
    {
        return "";
    }
}

template <class T>
std::vector<T> string_to_vector(const std::string& str)
{
    std::vector<T> vect;
    std::stringstream ss(str);
    float i;

    while (ss >> i)
    {
        vect.push_back(i);

        if (ss.peek() == ',' || ss.peek() == ' ')
            ss.ignore();
    }

    return vect;
}

template <class T>
std::vector<T> vector_subset(const std::vector<T>& vect, const std::vector<int>& chosen_indexes)
{
    std::vector<T> new_vect;

    auto indexes = chosen_indexes;
    std::sort(indexes.begin(), indexes.end());

    for (auto index : indexes)
    {
        new_vect.push_back(vect[index]);
    }

    return new_vect;
}

#endif /* __PRINT_VECTOR_H__ */

