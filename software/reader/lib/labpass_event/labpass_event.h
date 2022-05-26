#pragma once
#include <esp_event.h>

struct LabpassEvent
{
    esp_event_base_t eventBase;
    int event_type;
    void *event_data;
};
