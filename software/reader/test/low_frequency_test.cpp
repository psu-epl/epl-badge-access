#include "unity.h"
#include <tag.h>
#include <low_freq.h>
#include <state.h>
#include <tag_state_impl.h>

class LowFrequencyImpl : public LowFrequency
{
public:
    void sendTag(Tag *tag){
        lastTag = tag;
    };
    Tag *lastTag;
};

LowFrequencyImpl *lowFreqImpl;

void setUp()
{
    lowFreqImpl = new LowFrequencyImpl();
    lowFreqImpl->lastTag = NULL;
}

void tearDown()
{
    delete lowFreqImpl;
}

void testSyncStateTransitionSuccess()
{
    State *state = &StateSync::getInstance();

    TEST_ASSERT_TRUE(state->edgeEvent(lowFreqImpl, E0001) == NULL);
    TEST_ASSERT_TRUE(state->edgeEvent(lowFreqImpl, E1) == NULL);
    state = state->edgeEvent(lowFreqImpl, E1);

    TEST_ASSERT_TRUE(state->name() == "StatePadding");
}

void testSyncStateTransitionFail1()
{
    State *state = &StateSync::getInstance();

    TEST_ASSERT_TRUE(state->edgeEvent(lowFreqImpl, E0001) == NULL);
    state = state->edgeEvent(lowFreqImpl, E001);
    TEST_ASSERT_TRUE(state->name() == "StateSync");
}

void testSyncStateTransitionFail2()
{
    State *state = &StateSync::getInstance();

    TEST_ASSERT_TRUE(state->edgeEvent(lowFreqImpl, E0001) == NULL);
    TEST_ASSERT_TRUE(state->edgeEvent(lowFreqImpl, E1) == NULL);
    state = state->edgeEvent(lowFreqImpl, E001);
    TEST_ASSERT_TRUE(state->name() == "StateSync");
}

void testPaddingStateTransitionSuccess()
{
    State *state = &StateSync::getInstance();
    State *newState = NULL;

    state->edgeEvent(lowFreqImpl, E0001);
    state->edgeEvent(lowFreqImpl, E1);
    newState = state->edgeEvent(lowFreqImpl, E1);

    state->exit(lowFreqImpl);
    state = newState;
    state->enter(lowFreqImpl);

    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state = state->edgeEvent(lowFreqImpl, E01);

    TEST_ASSERT_TRUE(state->name() == "StatePayload");
}

void testPaddingStateTransitionFailure()
{
    State *state = &StateSync::getInstance();

    state->edgeEvent(lowFreqImpl, E0001);
    state->edgeEvent(lowFreqImpl, E1);
    state = state->edgeEvent(lowFreqImpl, E1);

    TEST_ASSERT_TRUE(state->name() == "StatePadding");

    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state = state->edgeEvent(lowFreqImpl, E001);

    TEST_ASSERT_TRUE(state->name() == "StateSync");
}

void testPayloadStateTransition01Success()
{
    State *state = &StateSync::getInstance();
    State *newState = NULL;

    state->edgeEvent(lowFreqImpl, E0001);
    state->edgeEvent(lowFreqImpl, E1);
    newState = state->edgeEvent(lowFreqImpl, E1);

    state->exit(lowFreqImpl);
    state = newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_TRUE(state->name() == "StatePadding");

    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    newState = state->edgeEvent(lowFreqImpl, E01);

    state->exit(lowFreqImpl);
    state = newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_FALSE(state == NULL);
    TEST_ASSERT_TRUE(state->name() == "StatePayload");

    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E1);
    newState = state->edgeEvent(lowFreqImpl, E001);

    state->exit(lowFreqImpl);
    state = newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_FALSE(state == NULL);
    TEST_ASSERT_TRUE(state->name() == "StateSync");
    TEST_ASSERT_FALSE(lowFreqImpl->lastTag == NULL);

    TEST_ASSERT_TRUE(lowFreqImpl->lastTag->lfTag.facility == 258);
    TEST_ASSERT_TRUE(lowFreqImpl->lastTag->lfTag.id == 194717);
}

void testPayloadStateTransition10Success()
{
    State *state = &StateSync::getInstance();
    State *newState = NULL;

    state->edgeEvent(lowFreqImpl, E0001);
    state->edgeEvent(lowFreqImpl, E1);
    newState = state->edgeEvent(lowFreqImpl, E1);

    state->exit(lowFreqImpl);
    state = newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_TRUE(state->name() == "StatePadding");

    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    newState = state->edgeEvent(lowFreqImpl, E01);

    state->exit(lowFreqImpl);
    state = newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_FALSE(state == NULL);
    TEST_ASSERT_TRUE(state->name() == "StatePayload");

    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E1);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E01);
    state->edgeEvent(lowFreqImpl, E001);
    state->edgeEvent(lowFreqImpl, E1);
    newState = state->edgeEvent(lowFreqImpl, E01);

    state->exit(lowFreqImpl);
    state = newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_FALSE(state == NULL);
    TEST_ASSERT_TRUE(state->name() == "StateSync");
    TEST_ASSERT_FALSE(lowFreqImpl->lastTag == NULL);

    TEST_ASSERT_TRUE(lowFreqImpl->lastTag->lfTag.facility == 258);
    TEST_ASSERT_TRUE(lowFreqImpl->lastTag->lfTag.id == 186525);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(testSyncStateTransitionSuccess);
    RUN_TEST(testSyncStateTransitionFail1);
    RUN_TEST(testSyncStateTransitionFail2);
    RUN_TEST(testPaddingStateTransitionSuccess);
    RUN_TEST(testPaddingStateTransitionFailure);
    RUN_TEST(testPayloadStateTransition01Success);
    RUN_TEST(testPayloadStateTransition10Success);
    UNITY_END();
}
