#pragma once
#include <esp_event.h>

ESP_EVENT_DECLARE_BASE(LabpassReaderEvent);

class LabpassEvent {
public:
    enum Reader
    {
        ShortLFBadge,
        LongLFBadge,
        ShortHFBadge,
        LongHFBadge
    };
};
