#ifndef __FSM__
#define __FSM__

#include "../modules/tinyfsm.hpp"
#include "Voter.h"

struct AcornDetected   : tinyfsm::Event {
    AcornDetected(int orient) : orientation(orient) {}
    int orientation;
};
struct AcornMissing : tinyfsm::Event { };

class OrientationDetectionFsm : public tinyfsm::Fsm<OrientationDetectionFsm>
{
public:
    void setAllowedEmptyFrames(int size);

    virtual ~OrientationDetectionFsm() = default;
    virtual void react(AcornDetected const& e);
    virtual void react(AcornMissing const& e);
    virtual void entry(void);
    void         exit(void);

protected:
    static int allowed_empty_frames;
    static int found_empty_frames;
    static int sequence_length;
    static Voter voter;
};

class Idle : public OrientationDetectionFsm
{
public:
    virtual void entry(void);
    virtual void react(const AcornDetected&) override;
    virtual void react(const AcornMissing&) override;
};

class InGatheringSequence : public OrientationDetectionFsm
{
public:
    virtual void entry(void);
    virtual void react(const AcornDetected&) override;
    virtual void react(const AcornMissing&) override;
};

#endif


