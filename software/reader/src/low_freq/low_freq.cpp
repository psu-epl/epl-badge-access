#include "low_freq.h"

using namespace team17;
using namespace std;

LowFrequency::LowFrequency(QueueHandle_t tagQueue, uint8_t din) : din_(din),
                                                        elapsed_(0),
                                                        state_(StateName::idle),
                                                        edgeCount_(0),
                                                        header_(std::bitset<HEADER_BIT_SIZE>()),
                                                        tag_(Tag(Tag::TagType::LowFrequency, TAG_BIT_SIZE)),
                                                        tagPrefix_(TAG_PREFIX),
                                                        edgeQueue_(NULL),
                                                        tagQueue_(tagQueue)

{
    pinMode(din_, INPUT);
}

esp_err_t LowFrequency::start()
{
    if (esp_err_t cerr = LowFrequency::carrierOn())
    {
        return cerr;
    }

    edgeQueue_ = xQueueCreate(4096, sizeof(uint32_t));
    uint32_t tmpEdge = 0;
    
    mcpwm_pin_config_t pinConfig = {
        .mcpwm_cap0_in_num = din_};

    mcpwm_capture_config_t capConfig = {
        .cap_edge = MCPWM_POS_EDGE,
        .cap_prescale = 1,
        .capture_cb = LowFrequency::edgeCallback,
        .user_data = edgeQueue_
    };

    mcpwm_capture_enable_channel(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, &capConfig);
    mcpwm_set_pin(MCPWM_UNIT_0, &pinConfig);

    for (; ;)
    {
        if(xQueueReceive(edgeQueue_, &tmpEdge, portMAX_DELAY)) {
            edge(tmpEdge);
            //log_i("%d", tmpEdge);
        }
        else
        {
            log_w("receive from edgeQueue failed");
        };
    }
}

void LowFrequency::edge(uint32_t edge)
{
    switch (state_)
    {
    case StateName::invalid:
        log_d("state change to invalid");
        break;

    case StateName::idle:
        // we've got our first edge, add to right of our 14-bit header,
        // go to state starting1
        if (ZEROS_START_MARKER(edge))
        {
            edgeCount_ = 1;
            state_ = StateName::starting;
        }
        break;

    case StateName::starting:
        // we've seen at least one edge already, test whether this
        // edge is in a group
        if (IN_GROUP(edge))
        {
            edgeCount_++;

            if (edgeCount_ == 5)
            {
                header_ = (header_ << 1) |= 1;
                edgeCount_ = 0;

                // this edge is part of a group. If all 14 bits in our header are
                // set, it means this bit is bit 15, which is half of the valid start
                // marker (the other half is 15 0s)
                if (header_.all())
                {
                    // we got our header, start expecting 15 zeros
                    header_.reset();
                    state_ = StateName::started;
                }
            }
        }
        // we were looking for 15 1's in a row, but there was too long a gap between the least
        // 1 and this one, so reset
        else
        {
            resetState();
        }
        break;
    case StateName::started:
        // We've seen a correct start of tag header and now we're reading actual data
        if (IN_GROUP(edge))
        {
            // the current edge is in a group of edges (we've already seen at least one)
            edgeCount_++;

            // If we have 5 edges, that is one data bit, so shift 1 bit into the right of the tag
            if (edgeCount_ == 5)
            {
                tag_.addBit(1);
                edgeCount_ = 0;

                if ( tag_.getLength() == TAG_BIT_SIZE )
                {
                    Tag *newTag = new Tag(Tag::TagType::LowFrequency, TAG_BIT_SIZE, *(tag_.getData()));
                    xQueueSend(tagQueue_, newTag, portMAX_DELAY);
                    resetState();
                }
            }
        }
        else
        {
            // This edge is not in a cluster of edges, so we expect it to be the first one in a cluster
            // because the data is manchester, we expect the preceeding space between this edge and
            // the last edge to be 5 or 10 zeros corresponding to 0101 (5 zeros in between) or 1001 (10
            // zeros in between)
            if (ZERO_BIT(edge))
            {
                // there were 5 zeros (or space enough for 5 zeros - we'll assume that's what was there since
                // we can't see the zeros)
                tag_.addBit(0);
                edgeCount_++;
            }
            else if (TWO_ZERO_BITS(edge))
            {
                // there was space for 10 zeros, so assume there were 10 because we're optimists and things
                // in computers seldom go wrong
                tag_.addBit(0);
                tag_.addBit(0);
                edgeCount_++;
            }
            else
            {
                resetState();
            }
        }
        break;
    }
}

void LowFrequency::resetState()
{
    header_.reset();
    state_ = StateName::idle;
    edgeCount_ = 0;
    tag_.reset();
}

bool LowFrequency::edgeCallback(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata, void *user_data)
{
    static uint32_t previousEdgeTimestamp = 0;
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t edgeQueue = (QueueHandle_t)user_data;

    uint32_t edgePeriod = edata->cap_value - previousEdgeTimestamp;
    previousEdgeTimestamp = edata->cap_value;
    if (edgePeriod < 1500)
    {
        return high_task_wakeup;
    }

    xQueueSendFromISR(edgeQueue, &edgePeriod, &high_task_wakeup);
    return high_task_wakeup;
}

esp_err_t LowFrequency::carrierOn()
{
    ledc_timer_config_t timerConfig{
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_1_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 125000};

    if (esp_err_t terr = ledc_timer_config(&timerConfig))
    {
        return terr;
    }

    ledc_channel_config_t channelConfig{
        .gpio_num = TX,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = PWM_DUTYCYCLE,
        .hpoint = 0xffff,
    };

    if (esp_err_t cerr = ledc_channel_config(&channelConfig))
    {
        return cerr;
    }

    ledcAttachPin(TX, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, PWM_DUTYCYCLE);

    return (esp_err_t) 0;
}
