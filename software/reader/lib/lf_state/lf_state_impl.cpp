#include <lf_state_impl.h>

/******************************************************************************

    StateSync

******************************************************************************/

void LFStateSync::enter(LowFrequency * lf)
{
    header_ = 0x00;
}

void LFStateSync::exit(LowFrequency *lf)
{
    header_ = 0x00;
}

LFState &LFStateSync::getInstance()
{
    static LFStateSync singleton;
    return singleton;
}

string LFStateSync::name()
{
    return string("StateSync");
}

void LFStateSync::edgeEvent(LowFrequency *lf, EdgeType edgeType, LFState **outNextState)
{
    *outNextState = NULL;

    switch (edgeType)
    {
        case E0001:
            header_ = 0b00000100;
            break;
        case E001:
            *outNextState = &LFStateSync::getInstance();
            break;
        case E01:
            *outNextState = &LFStateSync::getInstance();
            break;
        case E1:
            if (header_ == 0b00000100)
            {
                header_ = 0b00000110;
            }
            else if (header_ == 0b00000110)
            {
                *outNextState = &LFStatePadding::getInstance();
            }
            else
            {
                *outNextState = &LFStateSync::getInstance();
            }
            break;
        default:
            *outNextState = &LFStateSync::getInstance();
    }
}

/******************************************************************************

    StatePadding

******************************************************************************/

void LFStatePadding::enter(LowFrequency *lf)
{
    zerosCount_ = 0;
}

void LFStatePadding::exit(LowFrequency *lf)
{
    zerosCount_ = 0;
}

LFState &LFStatePadding::getInstance()
{
    static LFStatePadding singleton;
    return singleton;
}

string LFStatePadding::name()
{
    return string("StatePadding");
}

void LFStatePadding::edgeEvent(LowFrequency *lf, EdgeType edgeType, LFState **outNextState)
{
    *outNextState = NULL;

    switch(edgeType)
    {
    case E0001:
        *outNextState = &LFStateSync::getInstance();
        break;
    case E001:
        *outNextState = &LFStateSync::getInstance();
        break;
    case E01:
        zerosCount_++;
        if (zerosCount_ == 8)
        {
            *outNextState = &LFStatePayload::getInstance();
        }
        break;
    case E1:
        *outNextState = &LFStateSync::getInstance();
        break;
    default:
        *outNextState = &LFStateSync::getInstance();
    }
}

/******************************************************************************

    StatePayload

******************************************************************************/

void LFStatePayload::enter(LowFrequency *lf)
{
    payload_.reset();
    pos_ = payload_.size() - 1;
}

void LFStatePayload::exit(LowFrequency *lf)
{
    payload_.reset();
    pos_ = payload_.size() - 1;
}

LFState &LFStatePayload::getInstance()
{
    static LFStatePayload singleton;
    return singleton;
}

string LFStatePayload::name()
{
    return string("StatePayload");
}

void LFStatePayload::edgeEvent(LowFrequency *lf, EdgeType edgeType, LFState **outNextState)
{
    *outNextState = NULL;

    switch(edgeType)
    {
    case E0001:
        *outNextState = &LFStateSync::getInstance();
        break;
    case E001:
        payload_[pos_--] = 0;
        payload_[pos_--] = 0;
        payload_[pos_--] = 1;
        if (pos_ == 0)
        {
            payload_[pos_--] = 0;
        }
        if (pos_ == 255)
        {
            Tag * tag = new Tag;
            if(checkParity(tag))
            {
                lf->sendTag(*tag);
                *outNextState = &LFStateSync::getInstance();
            }
            else
            {
                *outNextState = &LFStateSync::getInstance();
            }
        }
        else if (pos_ > payload_.size() -1)
        {
            *outNextState = &LFStateSync::getInstance();
        }
        break;
    case E01:
        payload_[pos_--] = 0;
        payload_[pos_--] = 1;
        if (pos_ == 0)
        {
            payload_[pos_--] = 0;
        }
        if (pos_ == 255)
        {
            Tag *tag = new Tag;
            if (checkParity(tag))
            {
                lf->sendTag(*tag);
                *outNextState = &LFStateSync::getInstance();
            }
            else
            {
                *outNextState = &LFStateSync::getInstance();
            }
        }
        else if (pos_ > payload_.size() - 1)
        {
            *outNextState = &LFStateSync::getInstance();
        }
        break;
    case E1:
        payload_[pos_--] = 1;
        if (pos_ == 0)
        {
            payload_[pos_--] = 0;
        }
        if (pos_ == 255)
        {
            Tag *tag = new Tag;
            if (checkParity(tag))
            {
                lf->sendTag(*tag);
                *outNextState = &LFStateSync::getInstance();
            }
            else
            {
                *outNextState = &LFStateSync::getInstance();
            }
        }
        else if (pos_ > payload_.size() - 1)
        {
            *outNextState = &LFStateSync::getInstance();
        }
        break;
        *outNextState = &LFStateSync::getInstance();
    default:
        *outNextState = &LFStateSync::getInstance();
    }
}

bool LFStatePayload::checkParity(Tag *tag)
{
    bitset<74> mask = 0x03;
    bitset<37> buf = 0x00;
    for (int i = 0; i < 37; i++)
    {
        unsigned long result = (payload_ & mask).to_ulong();
        if (result == 0x01)
        {
            buf[i] = 0;
        }
        else if (result == 0x02)
        {
            buf[i] = 1;
        }
        else
        {
            return false;
        }
        payload_ >>= 2;
    }

    bool even = payload_[buf.size() - 1];
    bool odd = buf[0];

    bitset<37> evenParityMask = 0b0111111111111111111000000000000000000;
    bitset<37> oddParityMask =  0b0000000000000000001111111111111111110;

    if ((buf & evenParityMask).count() % 2 == 0 != even)
    {
        return false;
    }

    if ( ((buf & oddParityMask).count() % 2 == 1) == odd)
    {
        return false;
    }

    bitset<37> facilityMask = 0b0111111111111111100000000000000000000;
    bitset<37> idMask =       0b0000000000000000011111111111111111110;

    tag->lfTag.facility = ((buf & facilityMask) >> 20).to_ulong();
    tag->lfTag.id = ((buf & idMask) >> 1).to_ulong();

    return true;
}
