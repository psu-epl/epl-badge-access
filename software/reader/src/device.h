#pragma once

#include "device_state.h"
#include <unistd.h>
#include <WiFi.h>
class DeviceState;

class Device
{
public:
    Device(const char *ssid, const char *password);
    inline DeviceState* getCurrentState() const { return currentState; }
    void setstate(DeviceState &newState);
    void init();
    void wifiReconnect();

private:
    DeviceState *currentState;
    const char *ssid_;
    const char *password_;
    SemaphoreHandle_t mutex_;
    void wifiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
};
