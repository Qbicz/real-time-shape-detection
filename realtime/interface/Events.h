#ifndef EVENTS_H_
#define EVENTS_H_

#include "../modules/tinyfsm.hpp"

enum Orientation {
    UNKNOWN_ORIENTATION,
    LEFT_ORIENTED,
    RIGHT_ORIENTED,
    ORIENTATIONS_NUMBER //Nice hack, isn't it?
};

struct AcornDetected   : tinyfsm::Event {
    AcornDetected(Orientation orient) : orientation(orient) {}
    Orientation orientation;
};
struct AcornMissing : tinyfsm::Event { };

#endif /* REALTIME_EVENTS_H_ */
