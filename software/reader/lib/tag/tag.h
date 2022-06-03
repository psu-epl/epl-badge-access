#pragma once
#include <unistd.h>

struct LFTag {
    uint32_t facility;
    uint32_t id;
};

struct HFTag {
    uint32_t uuid;
};

union Tag {
    LFTag lfTag;
    HFTag hfTag;
};