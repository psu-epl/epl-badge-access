#include "indicators.h"

static volatile LEDState powerBlinkState;
static volatile LEDState statusBlinkState;
static volatile BuzzerState buzzerBlinkState;

Indicators::Indicators(Config *config) : config_(config)
{
    ledcSetup(INDICATOR_LEDC_CHAN_P_RED, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_P_GREEN, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_P_BLUE, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_S_RED, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_S_GREEN, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_S_BLUE, 12000, 8);
    ledcSetup(INDICATOR_LEDC_CHAN_B, 200, 8);

    ledcAttachPin(config_->powerRedGPIO, INDICATOR_LEDC_CHAN_P_RED);
    ledcAttachPin(config_->powerGreenGPIO, INDICATOR_LEDC_CHAN_P_GREEN);
    ledcAttachPin(config_->powerBlueGPIO, INDICATOR_LEDC_CHAN_P_BLUE);
    ledcAttachPin(config_->statusRedGPIO, INDICATOR_LEDC_CHAN_S_RED);
    ledcAttachPin(config_->statusGreenGPIO, INDICATOR_LEDC_CHAN_S_GREEN);
    ledcAttachPin(config_->statusBlueGPIO, INDICATOR_LEDC_CHAN_S_BLUE);
    ledcAttachPin(config_->buzzerGPIO, INDICATOR_LEDC_CHAN_B);

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

void Indicators::doShortBadgeInAuthOk()
{
    statusLEDOn(ColorGREEN);
    buzzerOn(INDICATOR_BUZZER_GOOD_FREQ);
    esp_timer_start_once(buzzerDurationTimerHandle_, SHORT_BADGE_DURATION);
}

void Indicators::doShortBadgeOutAuthOk()
{
    statusLEDOff();
    buzzerOn(INDICATOR_BUZZER_GOOD_FREQ);
    esp_timer_start_once(buzzerDurationTimerHandle_, SHORT_BADGE_DURATION);
}

void Indicators::doNotAuthorized()
{
    statusLEDOn(ColorRED);
    buzzerOn(INDICATOR_BUZZER_BAD_FREQ);
    esp_timer_start_once(statusDurationTimerHandle_, SHORT_BADGE_DURATION);
    esp_timer_start_once(buzzerDurationTimerHandle_, SHORT_BADGE_DURATION);
}

void Indicators::doAuthError()
{
    statusLEDBlinkStart(ColorRED, LONG_BADGE_RATE);
    buzzerBlinkStart(200, LONG_BADGE_RATE);
    esp_timer_start_once(statusDurationTimerHandle_, LONG_BADGE_DURATION);
    esp_timer_start_once(buzzerDurationTimerHandle_, LONG_BADGE_DURATION);
}

void Indicators::doEnrollState()
{
    statusLEDBlinkStart(ColorBLUE, LONG_BADGE_RATE);
    buzzerBlinkStart(8000, LONG_BADGE_RATE);
    esp_timer_start_once(buzzerDurationTimerHandle_, LONG_BADGE_DURATION);
}

void Indicators::doEnrollSuccess()
{
    statusLEDOn(ColorBLUE);
    buzzerOn(INDICATOR_BUZZER_GOOD_FREQ);
    esp_timer_start_once(statusDurationTimerHandle_, SHORT_BADGE_DURATION);
    esp_timer_start_once(buzzerDurationTimerHandle_, SHORT_BADGE_DURATION);
}

void Indicators::doEnrollContinue()
{
    statusLEDBlinkStart(ColorBLUE, LONG_BADGE_RATE);
}

void Indicators::doPowerNetReset() // solid red
{
    powerLEDOn(ColorRED);
}

void Indicators::doPowerInit()      // blinking red
{
    powerLEDBlinkStart(ColorRED, POWER_BLINK_RATE_SLOW);
}

void Indicators::doPowerAPConnect()     // blinking orange
{
    powerLEDBlinkStart(ColorORANGE, POWER_BLINK_RATE_SLOW);
}

void Indicators::doPowerIPAddress() // slow blinking yellow
{
    powerLEDBlinkStart(ColorYELLOW, POWER_BLINK_RATE_SLOW);
}

void Indicators::doPowerPingWait() // fast blinking yellow
{
    powerLEDBlinkStart(ColorYELLOW, POWER_BLINK_RATE_FAST);
}

void Indicators::doPowerIdle() // solid green
{
    powerLEDOn(ColorGREEN);
}

void Indicators::powerLEDOn(Color color)
        {
            switch (color)
            {
            case ColorRED:
                ledcWrite(INDICATOR_LEDC_CHAN_P_RED, 32);
                ledcWrite(INDICATOR_LEDC_CHAN_P_GREEN, 0);
                ledcWrite(INDICATOR_LEDC_CHAN_P_BLUE, 0);
                powerBlinkState.color = ColorRED;
                break;
            case ColorGREEN:
                ledcWrite(INDICATOR_LEDC_CHAN_P_RED, 0);
                ledcWrite(INDICATOR_LEDC_CHAN_P_GREEN, 32);
                ledcWrite(INDICATOR_LEDC_CHAN_P_BLUE, 0);
                powerBlinkState.color = ColorGREEN;
                break;
            case ColorBLUE:
                ledcWrite(INDICATOR_LEDC_CHAN_P_RED, 0);
                ledcWrite(INDICATOR_LEDC_CHAN_P_GREEN, 0);
                ledcWrite(INDICATOR_LEDC_CHAN_P_BLUE, 32);
                powerBlinkState.color = ColorBLUE;
                break;
            case ColorORANGE:
                ledcWrite(INDICATOR_LEDC_CHAN_P_RED, 32);
                ledcWrite(INDICATOR_LEDC_CHAN_P_GREEN, 3);
                ledcWrite(INDICATOR_LEDC_CHAN_P_BLUE, 0);
                powerBlinkState.color = ColorORANGE;
                break;
            case ColorYELLOW:
                ledcWrite(INDICATOR_LEDC_CHAN_P_RED, 32);
                ledcWrite(INDICATOR_LEDC_CHAN_P_GREEN, 8);
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
        ledcWrite(INDICATOR_LEDC_CHAN_S_RED, 32);
        ledcWrite(INDICATOR_LEDC_CHAN_S_GREEN, 0);
        ledcWrite(INDICATOR_LEDC_CHAN_S_BLUE, 0);
        statusBlinkState.color = ColorRED;
        break;
    case ColorGREEN:
        ledcWrite(INDICATOR_LEDC_CHAN_S_RED, 0);
        ledcWrite(INDICATOR_LEDC_CHAN_S_GREEN, 32);
        ledcWrite(INDICATOR_LEDC_CHAN_S_BLUE, 0);
        statusBlinkState.color = ColorGREEN;
        break;
    case ColorBLUE:
        ledcWrite(INDICATOR_LEDC_CHAN_S_RED, 0);
        ledcWrite(INDICATOR_LEDC_CHAN_S_GREEN, 0);
        ledcWrite(INDICATOR_LEDC_CHAN_S_BLUE, 32);
        statusBlinkState.color = ColorBLUE;
        break;
    case ColorORANGE:
        ledcWrite(INDICATOR_LEDC_CHAN_S_RED, 32);
        ledcWrite(INDICATOR_LEDC_CHAN_S_GREEN, 3);
        ledcWrite(INDICATOR_LEDC_CHAN_S_BLUE, 0);
        statusBlinkState.color = ColorORANGE;
        break;
    case ColorYELLOW:
        ledcWrite(INDICATOR_LEDC_CHAN_S_RED, 32);
        ledcWrite(INDICATOR_LEDC_CHAN_S_GREEN, 8);
        ledcWrite(INDICATOR_LEDC_CHAN_S_BLUE, 0);
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
    digitalWrite(config_->relayGPIO, HIGH);
}

void Indicators::relayOff()
{
    digitalWrite(config_->relayGPIO, LOW);
}

void Indicators::buzzerBlinkStart(uint32_t freq, uint64_t duration)
{
    buzzerOn(freq);
    esp_timer_start_periodic(buzzerBlinkTimerHandle_, duration);
}

void Indicators::buzzerOn(uint32_t freq)
{
    ledcSetup(INDICATOR_LEDC_CHAN_B, freq, 8);
    ledcWrite(INDICATOR_LEDC_CHAN_B, 192);
    buzzerBlinkState.on = true;
    buzzerBlinkState.freq = freq;
}

void Indicators::buzzerOff()
{
    ledcWrite(INDICATOR_LEDC_CHAN_B, 0);
    buzzerBlinkState.on = false;
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
    if (buzzerBlinkState.on)
    {
        that->buzzerOff();
    }
    else
    {
        that->buzzerOn(buzzerBlinkState.freq);
    }
}

void Indicators::buzzerDurationTimerExpire(void *p)
{
    Indicators *that = (Indicators *)p;
    that->buzzerShutdown();
}
