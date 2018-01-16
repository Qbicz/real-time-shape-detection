#ifndef REALTIME_VOTER_H_
#define REALTIME_VOTER_H_

#include "Events.h"

class Voter
{
public:
    void add_orientation(Orientation orientation);
    Orientation make_decision();
    void reset();
private:
    int decisions[ORIENTATIONS_NUMBER] = {0};
};

#endif /* REALTIME_VOTER_H_ */
