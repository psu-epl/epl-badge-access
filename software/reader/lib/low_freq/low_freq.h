#pragma once
#include <tag.h>

class LowFrequency
{
public:
    virtual void sendTag(Tag *) = 0;
    ~LowFrequency() {}
};