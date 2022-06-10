#pragma once
#include <lf_state.h>
#include <bitset>
#include <tag.h>
#include <low_freq.h>
#include <string>
#include <unistd.h>

using namespace std;

class LowFrequency;

class LFStateSync : public LFState
{
public:
    void enter(LowFrequency * lf);
    void exit(LowFrequency * lf);
    LFState::NextState edgeEvent(LowFrequency * lf, EdgeType edgeType);
    static LFState &getInstance();
    string name();

private:
    LFStateSync(){};
    LFStateSync(const LFStateSync &other);
    LFStateSync &operator=(LFStateSync &other);
    uint8_t header_;
};

class LFStatePadding : public LFState
{
public:
    void enter(LowFrequency * lf);
    void exit(LowFrequency * lf);
    LFState::NextState edgeEvent(LowFrequency *lf, EdgeType edgeType);

    static LFState &getInstance();
    string name();


private:
    LFStatePadding(){};
    LFStatePadding(const LFStatePadding &other);
    LFStatePadding &operator=(LFStatePadding &other);
    uint8_t zerosCount_;
    uint8_t edgeCount_;
};

class LFStatePayload : public LFState
{
public:
    void enter(LowFrequency * lf);
    void exit(LowFrequency * lf);
    LFState::NextState edgeEvent(LowFrequency *lf, EdgeType edgeType);

    static LFState &getInstance();
    string name();

private:
    LFStatePayload(){};
    LFStatePayload(const LFStatePayload &other);
    LFStatePayload &operator=(LFStatePayload &other);
    bitset<74> payload_;
    uint8_t pos_;
    bool checkParity(Tag **tag);
    uint8_t edgeCount_;
};
