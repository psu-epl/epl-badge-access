#pragma once
#include <tag.h>

class LowFrequency
{
public:
    virtual void sendTag(Tag &tag) = 0;
    ~LowFrequency() {}
};