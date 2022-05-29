#pragma once

#include <bitset>
#include <string>
#include <optional>
#include "driver/mcpwm.h"
#include <tag.h>
#include <esp_event.h>
#include <lf_state_impl.h>
#include <Arduino.h>
#include "driver/ledc.h"
#include <vector>

#define TAG_BIT_SIZE 96
#define HEADER_BIT_SIZE 3

#define TAG_PREFIX "000111"
#define TAG_PREFIX_LENGTH 6

#define IN_GROUP(i) (i > 5000 && i < 7000)
#define ZERO_BIT(i) (i > 30000 && i < 45000)
#define TWO_ZERO_BITS(i) (i > 60000 && i < 80000)
#define ZEROS_START_MARKER(i) (i > 100000 && i < 104000)

#define PWM_CHANNEL 0
#define PWM_FREQUENCY 125000
#define PWM_DUTYCYCLE 0
#define PWM_RESOLUTION 1

ESP_EVENT_DECLARE_BASE(LabpassLFReaderEvent);

class LowFrequency;

class LowFrequencyImpl : public LowFrequency
{
public:

    enum labpassLFReaderEventType
    {
        shortBadge,
        longBadge
    };

    LowFrequencyImpl(esp_event_loop_handle_t event_loop, uint8_t din);

    static bool edgeCallback(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata, void *user_data);
    static bool edgeCallback2(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata, void *user_data);
    esp_err_t start();
    void edge(uint32_t edge);
    inline QueueHandle_t getEdgeQueue() { return edgeQueue_; }
    inline esp_event_loop_handle_t getEventLoop() { return eventLoop_; }
    void sendTag(Tag *);
    void setState(LFState &newState);
    inline LFState *getCurrentState() { return currentState_; }
    static bool timerExpire(void *);
    inline Tag *getLastTag() { return lastTag_; }
    inline void resetLastTag() { lastTag_ = NULL; }
    void assembleFragment(LFState::EdgeType edgeType);
    vector<LFState::EdgeType> fragBuf_;

private:
    static void edgeTask(void *p);
    static void edgeTask2(void *p);
    esp_err_t carrierOn();

    uint8_t din_;
    QueueHandle_t edgeQueue_;
    esp_event_loop_handle_t eventLoop_;
    LFState * currentState_;
    bool timerActive_;
    uint32_t badgeDuration_;
    Tag * lastTag_;
};