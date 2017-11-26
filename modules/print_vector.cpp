#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include "print_vector.h"

void print_vector(std::vector<float> &vect)
{
    std::cout << "std::vector [ ";
    for (auto vect_elem : vect)
    {
        std::cout << vect_elem << " ";
    }
    std::cout << "]\n";
}

std::string vector_to_string(std::vector<float> &vect)
{
    std::ostringstream oss;

    if (!vect.empty())
    {
        // Convert all but the last element to avoid a trailing ","
        std::copy(vect.begin(), vect.end()-1,
            std::ostream_iterator<float>(oss, ","));

        // Now add the last element with no delimiter
        oss << vect.back();

        return oss.str();
    }
    else
    {
        return "";
    }
}
