#include "indicators.h"

static volatile LEDState powerBlinkState;
static volatile LEDState statusBlinkState;
static volatile bool buzzerBlinkState;

Indicators::Indicators(Config &config) : config_(config)
{
    ledcSetup(INDICATOR_LEDC_CHAN_P_RED, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_P_GREEN, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_P_BLUE, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_S_RED, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_S_GREEN, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_S_BLUE, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_B, 12000, 8);

    ledcAttachPin(config_.powerRedGPIO, INDICATOR_LEDC_CHAN_P_RED);
    ledcAttachPin(config_.powerGreenGPIO, INDICATOR_LEDC_CHAN_P_GREEN);
    ledcAttachPin(config_.powerBlueGPIO, INDICATOR_LEDC_CHAN_P_BLUE);
    ledcAttachPin(config_.statusRedGPIO, INDICATOR_LEDC_CHAN_S_RED);
    ledcAttachPin(config_.statusGreenGPIO, INDICATOR_LEDC_CHAN_S_GREEN);
    ledcAttachPin(config_.statusBlueGPIO, INDICATOR_LEDC_CHAN_S_BLUE);
    ledcAttachPin(config_.buzzerGPIO, INDICATOR_LEDC_CHAN_B);

    buzzerDuty_ = (4095 / 255) * min(192, 255);

    esp_timer_create_args_t createPowerBlinkTimerArgs{
        .callback = Indicators::powerBlinkTimerExpire,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "power_led_blink_timer",
        .skip_unhandled_events = true};

    esp_timer_create_args_t createPowerDurationTimerArgs{
        .callback = Indicators::powerDurationTimerExpire,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "power_led_duration_timer",
        .skip_unhandled_events = true};

    esp_timer_create_args_t createStatusBlinkTimerArgs{
        .callback = Indicators::statusBlinkTimerExpire,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "status_led_blink_timer",
        .skip_unhandled_events = true};

    esp_timer_create_args_t createStatusDurationTimerArgs{
        .callback = Indicators::statusDurationTimerExpire,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "status_led_duration_timer",
        .skip_unhandled_events = true};

    esp_timer_create_args_t createBuzzerBlinkTimerArgs{
        .callback = Indicators::buzzerBlinkTimerExpire,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "buzzer_blink_timer",
        .skip_unhandled_events = true};

    esp_timer_create_args_t createBuzzerDurationTimerArgs{
        .callback = Indicators::buzzerDurationTimerExpire,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "buzzer_duration_timer",
        .skip_unhandled_events = true};

    esp_timer_create(&createPowerBlinkTimerArgs, &powerBlinkTimerHandle_);
    esp_timer_create(&createPowerDurationTimerArgs, &powerDurationTimerHandle_);
    esp_timer_create(&createStatusBlinkTimerArgs, &statusBlinkTimerHandle_);
    esp_timer_create(&createStatusDurationTimerArgs, &statusDurationTimerHandle_);
    esp_timer_create(&createBuzzerBlinkTimerArgs, &buzzerBlinkTimerHandle_);
    esp_timer_create(&createBuzzerDurationTimerArgs, &buzzerDurationTimerHandle_);
}

void Indicators::doShortBadge()
{
    statusLEDOn(ColorGREEN);
    buzzerOn();
    esp_timer_start_once(statusDurationTimerHandle_, SHORT_BADGE_DURATION);
    esp_timer_start_once(buzzerDurationTimerHandle_, SHORT_BADGE_DURATION);
}

void Indicators::doLongBadge()
{
    statusLEDBlinkStart(ColorGREEN, LONG_BADGE_RATE);
    buzzerBlinkStart(LONG_BADGE_RATE);
    esp_timer_start_once(statusDurationTimerHandle_, 8000000);
    esp_timer_start_once(buzzerDurationTimerHandle_, LONG_BADGE_DURATION);
}

void Indicators::powerLEDOn(Color color)
{
    switch (color)
    {
        case ColorRED:
            ledcWrite(INDICATOR_LEDC_CHAN_P_RED, 200);
            ledcWrite(INDICATOR_LEDC_CHAN_P_GREEN, 0);
            ledcWrite(INDICATOR_LEDC_CHAN_P_BLUE, 0);
            powerBlinkState.color = ColorRED;
        case ColorGREEN:
            ledcWrite(INDICATOR_LEDC_CHAN_P_RED, 0);
            ledcWrite(INDICATOR_LEDC_CHAN_P_GREEN, 200);
            ledcWrite(INDICATOR_LEDC_CHAN_P_BLUE, 0);
            powerBlinkState.color = ColorGREEN;
        case ColorBLUE:
            ledcWrite(INDICATOR_LEDC_CHAN_P_RED, 0);
            ledcWrite(INDICATOR_LEDC_CHAN_P_GREEN, 0);
            ledcWrite(INDICATOR_LEDC_CHAN_P_BLUE, 200);
            powerBlinkState.color = ColorBLUE;
        case ColorORANGE:
            ledcWrite(INDICATOR_LEDC_CHAN_P_RED, 0);
            ledcWrite(INDICATOR_LEDC_CHAN_P_GREEN, 0);
            ledcWrite(INDICATOR_LEDC_CHAN_P_BLUE, 0);
            powerBlinkState.color = ColorORANGE;
        case ColorYELLOW:
            ledcWrite(INDICATOR_LEDC_CHAN_P_RED, 0);
            ledcWrite(INDICATOR_LEDC_CHAN_P_GREEN, 0);
            ledcWrite(INDICATOR_LEDC_CHAN_P_BLUE, 0);
            powerBlinkState.color = ColorYELLOW;
        }
        powerBlinkState.on = true;
}

void Indicators::powerLEDOff()
{
    ledcWrite(INDICATOR_LEDC_CHAN_P_RED, 0);
    ledcWrite(INDICATOR_LEDC_CHAN_P_GREEN, 0);
    ledcWrite(INDICATOR_LEDC_CHAN_P_BLUE, 0);
    powerBlinkState.on = false;
}

void Indicators::powerLEDShutdown()
{
    esp_timer_stop(powerBlinkTimerHandle_);
    esp_timer_stop(powerDurationTimerHandle_);
    powerLEDOff();
}

void Indicators::powerLEDBlinkStart(Color color, uint64_t delay)
{
    powerLEDOn(color);
    esp_timer_start_periodic(powerBlinkTimerHandle_, delay);
}

void Indicators::statusLEDOn(Color color)
{
    switch (color)
    {
    case ColorRED:
        ledcWrite(INDICATOR_LEDC_CHAN_S_RED, dutyPct_(100));
        ledcWrite(INDICATOR_LEDC_CHAN_S_GREEN, dutyPct_(0));
        ledcWrite(INDICATOR_LEDC_CHAN_S_BLUE, dutyPct_(0));
        statusBlinkState.color = ColorRED;
    case ColorGREEN:
        ledcWrite(INDICATOR_LEDC_CHAN_S_RED, dutyPct_(0));
        ledcWrite(INDICATOR_LEDC_CHAN_S_GREEN, dutyPct_(100));
        ledcWrite(INDICATOR_LEDC_CHAN_S_BLUE, dutyPct_(0));
        statusBlinkState.color = ColorGREEN;
    case ColorBLUE:
        ledcWrite(INDICATOR_LEDC_CHAN_S_RED, dutyPct_(0));
        ledcWrite(INDICATOR_LEDC_CHAN_S_GREEN, dutyPct_(0));
        ledcWrite(INDICATOR_LEDC_CHAN_S_BLUE, dutyPct_(100));
        statusBlinkState.color = ColorBLUE;
    case ColorORANGE:
        ledcWrite(INDICATOR_LEDC_CHAN_S_RED, dutyPct_(100));
        ledcWrite(INDICATOR_LEDC_CHAN_S_GREEN, dutyPct_(59));
        ledcWrite(INDICATOR_LEDC_CHAN_S_BLUE, duty_);
        statusBlinkState.color = ColorORANGE;
    case ColorYELLOW:
        ledcWrite(INDICATOR_LEDC_CHAN_S_RED, dutyPct_(100));
        ledcWrite(INDICATOR_LEDC_CHAN_S_GREEN, dutyPct_(100));
        ledcWrite(INDICATOR_LEDC_CHAN_S_BLUE, duty_);
        statusBlinkState.color = ColorYELLOW;
    }
    statusBlinkState.on = true;
}

void Indicators::statusLEDOff()
{
    ledcWrite(INDICATOR_LEDC_CHAN_S_RED, 0);
    ledcWrite(INDICATOR_LEDC_CHAN_S_GREEN, 0);
    ledcWrite(INDICATOR_LEDC_CHAN_S_BLUE, 0);
    statusBlinkState.on = false;
}

void Indicators::statusLEDShutdown()
{
    esp_timer_stop(statusBlinkTimerHandle_);
    esp_timer_stop(statusDurationTimerHandle_);
    statusLEDOff();
}

void Indicators::statusLEDBlinkStart(Color color, uint64_t delay)
{
    statusLEDOn(color);
    esp_timer_start_periodic(statusBlinkTimerHandle_, delay);
}

void Indicators::relayOn()
{
    digitalWrite(config_.relayGPIO, HIGH);
}

void Indicators::relayOff()
{
    digitalWrite(config_.relayGPIO, LOW);
}

void Indicators::buzzerBlinkStart(uint64_t duration)
{
    buzzerOn();
    esp_timer_start_periodic(buzzerBlinkTimerHandle_, duration);
}

void Indicators::buzzerOn()
{
    ledcWrite(INDICATOR_LEDC_CHAN_B, buzzerDuty_);
    buzzerBlinkState = true;
}

void Indicators::buzzerOff()
{
    ledcWrite(INDICATOR_LEDC_CHAN_B, 0);
    buzzerBlinkState = false;
}

void Indicators::buzzerShutdown()
{
    esp_timer_stop(buzzerBlinkTimerHandle_);
    esp_timer_stop(buzzerDurationTimerHandle_);
    buzzerOff();
}

void Indicators::powerBlinkTimerExpire(void *p)
{
    Indicators *that = (Indicators *)p;
    if (powerBlinkState.on)
    {
        that->powerLEDOff();
    }
    else
    {
        that->powerLEDOn(powerBlinkState.color);
    }
}

void Indicators::powerDurationTimerExpire(void *p)
{
    Indicators *that = (Indicators *)p;
    that->powerLEDShutdown();
}

void Indicators::statusBlinkTimerExpire(void *p)
{
    Indicators *that = (Indicators *)p;
    if (statusBlinkState.on)
    {
        that->statusLEDOff();
    }
    else
    {
        that->statusLEDOn(statusBlinkState.color);
    }
}

void Indicators::statusDurationTimerExpire(void *p)
{
    Indicators *that = (Indicators *)p;
    that->statusLEDShutdown();
}

void Indicators::buzzerBlinkTimerExpire(void *p)
{
    Indicators *that = (Indicators *)p;
    if (buzzerBlinkState)
    {
        that->buzzerOff();
    }
    else
    {
        that->buzzerOn();
    }
}

void Indicators::buzzerDurationTimerExpire(void *p)
{
    Indicators *that = (Indicators *)p;
    that->buzzerShutdown();
}

uint32_t Indicators::dutyPct_(uint8_t percent)
{
    return (4095 / 255) * min(int(percent), 255);
}
