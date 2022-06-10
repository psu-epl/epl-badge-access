#include "tag.h"
#include <cstring>
#include <array>

Tag::Tag()
{}

Tag::Tag(uint32_t id, uint32_t facility) : id_(id),
                                           facility_(facility),
                                           type_(Type::LF)
{

}

Tag::Tag(byte * uid, size_t length) : id_(0),
                                      facility_(0),
                                      uidLength_(length),
                                      type_(Type::HF)
{
    bzero(uid_, 10);
    memcpy(uid_, uid, 4);
}

String Tag::getID()
{
    switch (type_)
    {
        case LF:
            return String(id_);
        case HF:
            static const char table[] = "0123456789ABCDEF";
            std::array<char, 21> dst;
            const uint8_t *srcPtr = &uid_[0];
            char *dstPtr = &dst[0];

            for (auto count = uidLength_; count > 0; --count)
            {
                unsigned char c = *srcPtr++;
                *dstPtr++ = table[c >> 4];
                *dstPtr++ = table[c & 0x0f];
            }
            *dstPtr = 0;
            return &dst[0];
    }

    return "";
}

String Tag::getFacility()
{
    return String(facility_);
}
