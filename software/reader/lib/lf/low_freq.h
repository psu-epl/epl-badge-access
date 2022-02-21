#pragma once
#include <Arduino.h>

#include "include/etl/bitset.h"
#include "include/etl/string.h"
#include "include/etl/optional.h"
#include "driver/mcpwm.h"

#include "driver/ledc.h"

#define TAG_BIT_SIZE 96
#define HEADER_BIT_SIZE 3

#define TAG_PREFIX "000111"
#define TAG_PREFIX_LENGTH 6

#define IN_GROUP(i) (i > 5000 && i < 7000)
#define ZERO_BIT(i) (i > 35000 && i < 45000)
#define TWO_ZERO_BITS(i) (i > 65000 && i < 75000)
#define ZEROS_START_MARKER(i) (i > 103000 && i < 104000)

#define PWM_CHANNEL 0
#define PWM_FREQUENCY 125000
#define PWM_DUTYCYCLE 0
#define PWM_RESOLUTION 1

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
        LowFrequency(){};
        LowFrequency(QueueHandle_t tagQueue, uint8_t din);

        static bool edgeCallback(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata, void *user_data);
        esp_err_t start();
        void edge(uint32_t edge);
        unsigned long elapsed_;

    private:
        bool inGroup(unsigned long currentMicros);
        esp_err_t carrierOn();

        uint8_t din_;
        StateName state_;
        uint8_t edgeCount_;
        etl::bitset<TAG_BIT_SIZE> tag_;
        etl::bitset<HEADER_BIT_SIZE> header_;
        void resetState();
        bool gotTag();
        const etl::string<TAG_PREFIX_LENGTH> tagPrefix_;
        QueueHandle_t edgeQueue_;
        QueueHandle_t tagQueue_;
        uint32_t buf[4096];
    };
}
