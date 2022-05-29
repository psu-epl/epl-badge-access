#pragma once
#include <Arduino.h>
#include <esp_event.h>

ESP_EVENT_DECLARE_BASE(LabpassFakeReaderEvent);

enum labpassFakeReaderEventType
{
    shortBadge,
    longBadge
};

class FakeReader
{

public:
    FakeReader(gpio_num_t buttonGPIO, esp_event_loop_handle_t event_loop);
    esp_event_loop_handle_t getEventLoop();

private:
    gpio_num_t ButtonGPIO_;
    esp_event_loop_handle_t event_loop_;
};