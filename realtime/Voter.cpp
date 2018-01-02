#include <iostream>
#include <string>
#include "Voter.h"

void Voter::addOrientation(int orientation)
{
    decisions[orientation]++;
}

std::string Voter::make_decision()
{
    if(decisions[ORIENTATION_UNKNOWN] > 1)
    {
        return "No decision can be made ;(";
    }
    else if(decisions[LEFT_ORIENTED] == decisions[RIGHT_ORIENTED])
    {
        return "Same amount of left and right decisions ;(";
    }
    else if(decisions[LEFT_ORIENTED] > decisions[RIGHT_ORIENTED])
    {
        return "LEFT ORIENTED ACORN";
    }
    else
    {
        return "RIGHT ORIENTED ACORN";
    }
}

void Voter::reset()
{
    for(int& elem : decisions)
    {
        elem = 0;
    }
}

