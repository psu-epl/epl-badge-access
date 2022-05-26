#pragma once

#include <vector>
#include <stdint.h>
#include <string>

#define MAX_TAG_BYTES 128 // 1024 bit
typedef int tag_err_t;

namespace team17
{

    class Tag
    {

    public:
        enum TagType
        {
            LowFrequency,
            HighFrequency
        };

        Tag(TagType tagType, size_t length);
        Tag(TagType tagType, size_t length, std::vector<uint8_t> data);
        size_t getBitCount();
        std::vector<uint8_t> * getData();
        tag_err_t addBit(uint8_t b);
        tag_err_t addByte(uint8_t b);
        std::string getType();
        void reset();

    private:
        TagType type_;
        uint16_t length_;
        std::vector<uint8_t> data_;
        size_t bitCount_;
    };
}
