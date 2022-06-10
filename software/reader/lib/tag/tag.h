#pragma once
#include <Arduino.h>

class Tag {

public:
    enum Type
    {
        LF,
        HF
    };
    Tag();
    Tag(uint32_t id, uint32_t facility);
    Tag(uint8_t * uid, size_t length);

    String getID();
    String getFacility();

private:
    uint32_t id_;
    uint32_t facility_;
    uint8_t uid_[10];
    size_t uidLength_;
    Type type_;
};