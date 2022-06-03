#pragma once

#include "device.h"
#include <WiFi.h>
#include <tag.h>
#include "labpass_client/client.h"

class Device;

class DeviceState
{
public:
    virtual void enter(Device *device) = 0;
    virtual void exit(Device *device) = 0;

    virtual DeviceState *badgeEvent(Device *device, uint32_t eventType, Tag *tag) = 0;
    virtual DeviceState * wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info) = 0;

    virtual String name() = 0;
    virtual ~DeviceState(){};

protected:
    uint32_t managerID_;
    uint32_t userID_;
};