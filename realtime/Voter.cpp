#include <iostream>
#include <string>
#include "Voter.h"

void Voter::add_orientation(Orientation orientation)
{
    decisions[orientation]++;
}

Orientation Voter::make_decision()
{
    if(decisions[UNKNOWN_ORIENTATION] > 1 || (decisions[LEFT_ORIENTED] == decisions[RIGHT_ORIENTED]))
    {
        return UNKNOWN_ORIENTATION;
    }
    else if(decisions[LEFT_ORIENTED] > decisions[RIGHT_ORIENTED])
    {
        return LEFT_ORIENTED;
    }
    else
    {
        return RIGHT_ORIENTED;
    }
}

void Voter::reset()
{
    for(int& elem : decisions)
    {
        elem = 0;
    }
}
