#pragma once

#include <tag.h>
#include <string>

using namespace std;

class LowFrequency;


class LFState

{
public:

    enum EdgeType
    {
        E0001,
        E001,
        E0011,
        E01,
        E011,
        E1,
    };

    virtual void enter(LowFrequency *lf) = 0;
    virtual void exit(LowFrequency *lf) = 0;
    virtual LFState *edgeEvent(LowFrequency *lf, EdgeType edgeType) = 0;

    virtual string name() = 0;
    virtual ~LFState(){};
};