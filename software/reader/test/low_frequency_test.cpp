#include "unity.h"
#include <tag.h>
#include <low_freq.h>
#include <lf_state.h>
#include <lf_state_impl.h>
#include <lf_state.h>

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
    LFState *state = &LFStateSync::getInstance();

    TEST_ASSERT_TRUE(state->edgeEvent(lowFreqImpl, LFState::E0001) == NULL);
    TEST_ASSERT_TRUE(state->edgeEvent(lowFreqImpl, E1) == NULL);
    state->edgeEvent(lowFreqImpl, E1, &state);

    TEST_ASSERT_TRUE(state->name() == "StatePadding");
}

void testSyncStateTransitionFail1()
{
    LFState *&LFStateSync::getInstance();

    TEST_ASSERT_TRUE(state->edgeEvent(lowFreqImpl, E0001) == NULL);
    state->edgeEvent(lowFreqImpl, E001, &state);
    TEST_ASSERT_TRUE(state->name() == "LFStateSync");
}

void testSyncStateTransitionFail2()
{
    LFState *&LFStateSync::getInstance();

    TEST_ASSERT_TRUE(state->edgeEvent(lowFreqImpl, E0001) == NULL);
    TEST_ASSERT_TRUE(state->edgeEvent(lowFreqImpl, E1) == NULL);
    state->edgeEvent(lowFreqImpl, E001, &state);
    TEST_ASSERT_TRUE(state->name() == "LFStateSync");
}

void testPaddingStateTransitionSuccess()
{
    LFState *&LFStateSync::getInstance();
    LFState *newLFState = NULL;

    state->edgeEvent(lowFreqImpl, E0001, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    newLFState = state->edgeEvent(lowFreqImpl, E1);

    state->exit(lowFreqImpl);
    newState;
    state->enter(lowFreqImpl);

    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);

    TEST_ASSERT_TRUE(state->name() == "StatePayload");
}

void testPaddingStateTransitionFailure()
{
    LFState *&LFStateSync::getInstance();

    state->edgeEvent(lowFreqImpl, E0001, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);

    TEST_ASSERT_TRUE(state->name() == "StatePadding");

    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);

    TEST_ASSERT_TRUE(state->name() == "LFStateSync");
}

void testPayloadStateTransition01Success()
{
    LFState *&LFStateSync::getInstance();
    LFState *newLFState = NULL;

    state->edgeEvent(lowFreqImpl, E0001, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    newLFState = state->edgeEvent(lowFreqImpl, E1);

    state->exit(lowFreqImpl);
    newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_TRUE(state->name() == "StatePadding");

    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    newLFState = state->edgeEvent(lowFreqImpl, E01);

    state->exit(lowFreqImpl);
    newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_FALSE(state == NULL);
    TEST_ASSERT_TRUE(state->name() == "StatePayload");

    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E001), &state;
    state->edgeEvent(lowFreqImpl, E1, &state);
    newLFState = state->edgeEvent(lowFreqImpl, E001, &state);

    state->exit(lowFreqImpl);
    newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_FALSE(state == NULL);
    TEST_ASSERT_TRUE(state->name() == "LFStateSync");
    TEST_ASSERT_FALSE(lowFreqImpl->lastTag == NULL);

    TEST_ASSERT_TRUE(lowFreqImpl->lastTag->lfTag.facility == 258);
    TEST_ASSERT_TRUE(lowFreqImpl->lastTag->lfTag.id == 194717);
}

void testPayloadStateTransition10Success()
{
    LFState *&LFStateSync::getInstance();
    LFState *newLFState = NULL;

    state->edgeEvent(lowFreqImpl, E0001, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    newLFState = state->edgeEvent(lowFreqImpl, E1);

    state->exit(lowFreqImpl);
    newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_TRUE(state->name() == "StatePadding");

    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    newLFState = state->edgeEvent(lowFreqImpl, E01);

    state->exit(lowFreqImpl);
    newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_FALSE(state == NULL);
    TEST_ASSERT_TRUE(state->name() == "StatePayload");

    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E01, &state);
    state->edgeEvent(lowFreqImpl, E001, &state);
    state->edgeEvent(lowFreqImpl, E1, &state);
    newLFState = state->edgeEvent(lowFreqImpl, E01);

    state->exit(lowFreqImpl);
    newState;
    state->enter(lowFreqImpl);

    TEST_ASSERT_FALSE(state == NULL);
    TEST_ASSERT_TRUE(state->name() == "LFStateSync");
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
