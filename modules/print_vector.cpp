#include <iostream>
#include <vector>
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

