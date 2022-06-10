#pragma once
#include <unistd.h>
#include <Arduino.h>
#include <driver/timer.h>
#include <map>
#include <config.h>

#define LEDC_BASE_FREQ 5000

using namespace std;

class Indicators {

public:

    struct Config {
        uint8_t powerRedGPIO;
        uint8_t powerGreenGPIO;
        uint8_t powerBlueGPIO;
        uint8_t statusRedGPIO;
        uint8_t statusGreenGPIO;
        uint8_t statusBlueGPIO;
        uint8_t buzzerGPIO;
        uint8_t relayGPIO;
    };

    enum Color
    {
        ColorRED,
        ColorGREEN,
        ColorBLUE,
        ColorYELLOW,
        ColorORANGE
    };


    enum BuzzerType
    {
        ShortBadge,
        LongBadge
    };

    Indicators(Config &config);

    void doShortBadge();
    void doLongBadge();

    void powerLEDOn(Color color);
    void powerLEDOff();
    void powerLEDShutdown();

    void powerLEDBlinkStart(Color color, uint64_t delay);

    void statusLEDOn(Color color);
    void statusLEDOff();
    void statusLEDShutdown();

    void statusLEDBlinkStart(Color color, uint64_t delay);

    void relayOn();
    void relayOff();

    void buzzerOn();
    void buzzerOff();
    void buzzerShutdown();

    void buzzerBlinkStart(uint64_t delay);

private:
    Config &config_;
    uint32_t duty_;
    uint32_t buzzerDuty_;
    static void powerBlinkTimerExpire(void *);
    static void powerDurationTimerExpire(void *);
    static void statusBlinkTimerExpire(void *);
    static void statusDurationTimerExpire(void *);
    static void buzzerBlinkTimerExpire(void *);
    static void buzzerDurationTimerExpire(void *);

    uint32_t dutyPct_(uint8_t percent);
    esp_timer_handle_t powerBlinkTimerHandle_;
    esp_timer_handle_t statusBlinkTimerHandle_;
    esp_timer_handle_t powerDurationTimerHandle_;
    esp_timer_handle_t statusDurationTimerHandle_;
    esp_timer_handle_t buzzerBlinkTimerHandle_;
    esp_timer_handle_t buzzerDurationTimerHandle_;

};

struct LEDState
{
    Indicators::Color color;
    bool on;
};
