#include "tag.h"

using namespace team17;
using namespace std;

Tag::Tag(TagType type, size_t length) : type_(type),
                                        length_(length),
                                        data_(vector<uint8_t>(0)),
                                        bitCount_(0)
{
}

Tag::Tag(TagType type, size_t length, vector<uint8_t> data) : type_(type),
                                                              length_(length),
                                                              data_(data),
                                                              bitCount_(0)
{
}

string Tag::getType()
{
    switch (type_) {
    case Tag::TagType::HighFrequency:
        return string("High Frequency (13.56 MHz)");
        break;
    case Tag::TagType::LowFrequency:
        return string("Low Frequency (125 kHz)");
        break;
    }
    return string("Unknown Type");
}

size_t Tag::getLength()
{
    return data_.size();
}

vector<uint8_t> * Tag::getData()
{
    return &data_;
}

void Tag::reset()
{
    data_.clear();
}

tag_err_t Tag::addBit(uint8_t b)
{
    if( bitCount_ >= length_ )
    {
        return 1;
    }

    if (bitCount_ % 8 == 0)
    {
        data_.push_back(0);
    }

    data_[data_.size() - 1] |= (b << (7 - (bitCount_ % 8)));

    bitCount_++;

    return 0;
}

tag_err_t Tag::addByte(uint8_t b)
{
    if (data_.size() >= length_)
    {
        return 1;
    }
    
    data_.push_back(b);
    return 0;
}