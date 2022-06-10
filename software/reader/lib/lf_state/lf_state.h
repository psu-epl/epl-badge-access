#pragma once

#include <tag.h>
#include <string>

using namespace std;

class LowFrequency;


class LFState

{
public:

    enum NextState
    {
        NoChange,
        Sync,
        Padding,
        Payload,
    };

    enum EdgeType
    {
        E0001,
        E001,
        E01,
        E1,
    };

    virtual void enter(LowFrequency *lf) = 0;
    virtual void exit(LowFrequency *lf) = 0;
    virtual LFState::NextState edgeEvent(LowFrequency *lf, EdgeType edgeType) = 0;

    virtual string name() = 0;
    virtual ~LFState(){};
};