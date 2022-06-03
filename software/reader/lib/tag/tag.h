#pragma once
#include <unistd.h>

// struct LFTag {
//     uint32_t facility;
//     uint32_t id;
// };

// struct HFTag {
//     uint32_t uuid;
// };

// union Tag {
//     LFTag lfTag;
//     HFTag hfTag;
// };

class Tag {

public:
    enum Type
    {
        LF,
        HF
    };
    Tag(uint32_t id, uint32_t facility = 0);
    uint32_t getTagID();

private:
    uint32_t id_;
    uint32_t facility_;
    Type type_;
};