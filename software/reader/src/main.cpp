#include <config.h>
#include "setup.h"
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "device/device.h"
#include "labpass_client/client.h"

#include "indicators/indicators.h"
#include "low_freq_impl/low_freq_impl.h"
#include "high_freq/high_freq.h"

esp_event_loop_handle_t loopHandle;
Indicators * indicators;
MFRC522_SPI *mfrc522spi;
HighFrequency *highFrequency;
LowFrequencyImpl *lowFreuqency;

Indicators::Config indicatorsConfig{
    .powerRedGPIO = LED_POWER_RED_GPIO,
    .powerGreenGPIO = LED_POWER_GREEN_GPIO,
    .powerBlueGPIO = LED_POWER_BLUE_GPIO,
    .statusRedGPIO = LED_STATUS_RED_GPIO,
    .statusGreenGPIO = LED_STATUS_GREEN_GPIO,
    .statusBlueGPIO = LED_STATUS_BLUE_GPIO,
    .buzzerGPIO = BUZZER_GPIO,
    .relayGPIO = RELAY_GPIO};

void cardEvent(void *event_handler_arg, esp_event_base_t event_base, int event_id, void *event_data);

#include <WiFi.h>

String ssid = "7yv89tndqr_2g";
String password = "exoticcarrot227";
String baseURL = "http://192.168.1.63";
String stationID = "72a72777-e7da-4837-a870-2f9afdbcffe8";
String jwtToken = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjIyODQyMDM1MTgsImZ1bGxOYW1lIjoic29sZGVyaW5nLWlyb24iLCJpZCI6IjcyYTcyNzc3LWU3ZGEtNDgzNy1hODcwLTJmOWFmZGJjZmZlOCIsInJvbGUiOiJzdGF0aW9uIn0.J5OE_OtbA2wjvY3bggsnP8iFVux212m5YhAGRu5hiKI";
esp_event_loop_handle_t loop_handle;

Device *device;
LabpassClient *client;

void setup()
{
    Serial.begin(115200);

    mfrc522spi = new MFRC522_SPI(HF_SS_PIN, HF_RST_PIN);

    ledc_timer_config_t timerConfig{
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_1_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 125000};

    ESP_ERROR_CHECK(ledc_timer_config(&timerConfig));

    setupGPIO();

    indicators = new Indicators(indicatorsConfig);


    esp_event_loop_args_t *loop_args = new esp_event_loop_args_t{
        .queue_size = 5,
        .task_name = "labpass_loop_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 3072,
        .task_core_id = tskNO_AFFINITY};

    esp_event_loop_create(loop_args, &loopHandle);

    esp_event_handler_register_with(
        loopHandle,
        LabpassLFReaderEvent,
        ESP_EVENT_ANY_ID,
        cardEvent,
        NULL);

    esp_event_handler_register_with(
        loopHandle,
        LabpassHFReaderEvent,
        ESP_EVENT_ANY_ID,
        cardEvent,
        NULL);

    lowFreuqency = new LowFrequencyImpl(loopHandle, LF_RX, LF_TX);
    ESP_ERROR_CHECK(lowFreuqency->start());

    highFrequency = new HighFrequency(loopHandle, mfrc522spi);
    highFrequency->start();

    client = new LabpassClient(baseURL, stationID, jwtToken);
    device = new Device(ssid, password, loop_handle, client);

    device->start();
}

void loop()
{
    delay(500);
}

void cardEvent(void *event_handler_arg, esp_event_base_t event_base, int event_id, void *event_data)
{
    Tag *tag = (Tag *)event_data;
    if (event_base == LabpassHFReaderEvent)
    {
        switch (event_id)
        {
        case HighFrequency::labpassHFReaderEventType::longBadge:
            log_i("long badge, MIFARE: %d", tag->hfTag.uuid);
            indicators->doLongBadge();
            break;
        case HighFrequency::labpassHFReaderEventType::shortBadge:
            log_i("short badge, MIFARE: %d", tag->hfTag.uuid);
            indicators->doShortBadge();
            break;
        }
    }
    else if (event_base == LabpassLFReaderEvent)
    {
        switch (event_id)
        {
        case LowFrequencyImpl::labpassLFReaderEventType::longBadge:
            log_i("long badge: %d, %d", tag->lfTag.facility, tag->lfTag.id);
            indicators->doLongBadge();
            break;
        case LowFrequencyImpl::labpassLFReaderEventType::shortBadge:
            log_i("short badge: %d, %d", tag->lfTag.facility, tag->lfTag.id);
            indicators->doShortBadge();
            break;
        }
    }

}
