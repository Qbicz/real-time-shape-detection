#include <iostream>
#include "FSM.h"

int OrientationDetectionFsm::allowed_empty_frames = 0;
int OrientationDetectionFsm::found_empty_frames = 0;
int OrientationDetectionFsm::sequence_length = 0;
Voter OrientationDetectionFsm::voter;

void OrientationDetectionFsm ::react(AcornDetected const& e) {}
void OrientationDetectionFsm::react(AcornMissing const& e) {}
void OrientationDetectionFsm::entry(void) {}
void OrientationDetectionFsm::exit(void)  {}

void OrientationDetectionFsm::set_allowed_empty_frames(int size)
{
    allowed_empty_frames = size;
}

void Idle::entry(void)
{
    std::cout << "Entering idle\n";
    found_empty_frames = 0;
    sequence_length = 0;
    voter.reset();
}

void Idle::react(const AcornDetected& event)
{
    ++sequence_length;
    voter.add_orientation(event.orientation);

    transit<InGatheringSequence>([]{});
}


void Idle::react(const AcornMissing&)
{
    //do nothing - empty frame
}

void InGatheringSequence::react(const AcornDetected& event)
{
    ++sequence_length;
    voter.add_orientation(event.orientation);
}

void InGatheringSequence::entry(void)
{
    std::cout << "Entering sequence\n";
}

void InGatheringSequence::react(const AcornMissing&)
{
    if(found_empty_frames == allowed_empty_frames)
    {
        std::cout << "Sequence ended with " << sequence_length << " images\n";
        std::cout << "Orientation: " << voter.make_decision() << '\n';

        transit<Idle>([]{});
    }
    else
    {
        ++found_empty_frames;
    }
}

