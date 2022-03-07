#include <unity.h>
#include "tag/tag.h"
#include <bitset>
#include <stdio.h>

using namespace team17;
using namespace std;

void testNewTag()
{
    Tag * tag = new Tag(Tag::TagType::LowFrequency, 12);
    TEST_ASSERT_NOT_NULL(tag);

    free(tag);
}

void testNewHFTag()
{
    vector<uint8_t> wanted = vector<uint8_t>({0x0D, 0x0E, 0x0A, 0x0D});    
    Tag *tag = new Tag(Tag::TagType::HighFrequency, 4);

    for (size_t i = 0; i < wanted.size(); i++)
    {
        TEST_ASSERT_EQUAL(i, tag->getLength());
        TEST_ASSERT_EQUAL(0, tag->addByte(wanted[i]));
    }
    TEST_ASSERT_EQUAL(4, tag->getLength());
    TEST_ASSERT_TRUE(wanted == *(tag->getData()));
    
    free(tag);
}

void testHFTagAddTooMany()
{
    vector<uint8_t> wanted = vector<uint8_t>({0x0D, 0x0E, 0x0A, 0x0D});
    Tag *tag = new Tag(Tag::TagType::HighFrequency, 4);

    for (size_t i = 0; i < wanted.size(); i++)
    {
        tag->addByte(wanted[i]);
    }

    TEST_ASSERT_EQUAL(1, tag->addByte(0xFF));
    free(tag);
}

void testNewLFTag()
{
    vector<uint8_t> wantedVec = vector<uint8_t>({0x0F, 0x00, 0xC3, 0xFF, 0xC3,
                                                 0xCF, 0xCC, 0x3C, 0xC0, 0xC3,
                                                 0xC0, 0xFF});

    Tag *tag = new Tag(Tag::TagType::LowFrequency, 96);
    bitset<96> wanted = bitset<96>(string("0000111100000000110000111111"
                                          "11111100001111001111110011000011110011000000110000111100000011111111"));

    for (size_t i = 0; i < wanted.size(); i++)
    {
        TEST_ASSERT_EQUAL(0, tag->addBit(wanted[wanted.size() -1 - i]));
    }

    TEST_ASSERT_TRUE(wantedVec == *(tag->getData()));
}

void testNewLFTagTooMany()
{
    vector<uint8_t> wantedVec = vector<uint8_t>({0x0F, 0x00, 0xC3, 0xFF, 0xC3,
                                                 0xCF, 0xCC, 0x3C, 0xC0, 0xC3,
                                                 0xC0, 0xFF});

    Tag *tag = new Tag(Tag::TagType::LowFrequency, 96);
    bitset<96> wanted = bitset<96>(string("0000111100000000110000111111"
                                          "11111100001111001111110011000011110011000000110000111100000011111111"));

    for (size_t i = 0; i < wanted.size(); i++)
    {
        TEST_ASSERT_EQUAL(0, tag->addBit(wanted[ wanted.size() - 1 - i ]));
    }
    TEST_ASSERT_TRUE(wantedVec == *(tag->getData()));

    TEST_ASSERT_EQUAL(1, tag->addBit(1));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(testNewTag);
    RUN_TEST(testNewHFTag);
    RUN_TEST(testHFTagAddTooMany);
    RUN_TEST(testNewLFTag);
    RUN_TEST(testNewLFTagTooMany);

    UNITY_END();
}