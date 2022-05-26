#pragma once

#include <Arduino.h>
#include <bitset>
#include <string>
#include <optional>
#include "driver/mcpwm.h"
#include "tag.h"
#include "esp_event.h"

#include "driver/ledc.h"

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

enum labpassLFReaderEventType
{
    shortBadge,
    longBadge
};

namespace team17 {

    enum class StateName
    {
        invalid = 0,
        idle = 1,
        starting = 2,
        started = 4,
    };

    class LowFrequency
    {
    public:
        static bool edgeCallback(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata, void *user_data);
        LowFrequency(esp_event_loop_handle_t event_loop, uint8_t din);
        esp_err_t start();
        void edge(uint32_t edge);
        QueueHandle_t getEdgeQueue();

    private:

        uint8_t din_;
        StateName state_;
        uint8_t edgeCount_;
        std::bitset<HEADER_BIT_SIZE> header_;
        Tag tag_;
        QueueHandle_t edgeQueue_;

        static void edgeTask(void *p);
        esp_err_t carrierOn();
        void resetState();
        esp_event_loop_handle_t event_loop_;
    };
}
