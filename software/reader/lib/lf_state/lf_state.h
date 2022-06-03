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
        E01,
        E1,
    };

    virtual void enter(LowFrequency *lf) = 0;
    virtual void exit(LowFrequency *lf) = 0;
    virtual void edgeEvent(LowFrequency *lf, EdgeType edgeType, LFState **outNextState) = 0;

    virtual string name() = 0;
    virtual ~LFState(){};
};