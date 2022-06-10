#include <config.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "device/device.h"
#include "labpass_client/client.h"
#include <WiFi.h>

#include "indicators/indicators.h"
#include "low_freq_impl/low_freq_impl.h"
#include "high_freq/high_freq.h"

esp_event_loop_handle_t loopHandle;
Indicators::Config *indicatorsConfig;
Indicators *indicators;
MFRC522_SPI *mfrc522spi;
HighFrequency *highFrequency;
LowFrequencyImpl *lowFreuqency;
Device *device;
LabpassClient *client;
char statsBuf[1024];

void setup()
{
    
    Serial.begin(115200);

    String ssid = "labpass";
    String password = "abc123abc123";
    String baseURL = "http://10.11.12.1";

    String stationID = "228cd011-0744-47f3-bf31-898f3441b34e";
    String jwtToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjIyODQy"
        "MDM1MTgsImZ1bGxOYW1lIjoic29sZGVyaW5nLWlyb24iLCJpZCI6IjcyYTcyNzc3LWU3ZGEtND"
        "gzNy1hODcwLTJmOWFmZGJjZmZlOCIsInJvbGUiOiJzdGF0aW9uIn0.J5OE_OtbA2wjvY3bggsn"
        "P8iFVux212m5YhAGRu5hiKI";

    mfrc522spi = new MFRC522_SPI(HF_SS_PIN, HF_RST_PIN);

    ledc_timer_config_t timerConfig{
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_1_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 125000};

    ESP_ERROR_CHECK(ledc_timer_config(&timerConfig));

    indicatorsConfig = new Indicators::Config{
        .powerRedGPIO = LED_POWER_RED_GPIO,
        .powerGreenGPIO = LED_POWER_GREEN_GPIO,
        .powerBlueGPIO = LED_POWER_BLUE_GPIO,
        .statusRedGPIO = LED_STATUS_RED_GPIO,
        .statusGreenGPIO = LED_STATUS_GREEN_GPIO,
        .statusBlueGPIO = LED_STATUS_BLUE_GPIO,
        .buzzerGPIO = BUZZER_GPIO,
        .relayGPIO = RELAY_GPIO};

    indicators = new Indicators(indicatorsConfig);

    esp_event_loop_args_t loop_args{
        .queue_size = 15,
        .task_name = "labpass_loop_task",
        .task_priority = 5,
        .task_stack_size = 32768,
        .task_core_id = ESP_TASK_MAIN_CORE};

    ESP_ERROR_CHECK(esp_event_loop_create(&loop_args, &loopHandle));

    lowFreuqency = new LowFrequencyImpl(loopHandle, LF_RX, LF_TX);
    highFrequency = new HighFrequency(loopHandle, mfrc522spi);

    client = new LabpassClient(&baseURL, &jwtToken);
    device = new Device(ssid, password, loopHandle, client, indicators, stationID);

    device->start();
    vTaskDelay(pdMS_TO_TICKS(1000));

    highFrequency->start();
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_ERROR_CHECK(lowFreuqency->start());
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(2000));
}
