#pragma once

#include "device.h"
#include <WiFi.h>

class Device;

class DeviceState
{
public:
    virtual void enter(Device *device) = 0;
    virtual void exit(Device *device) = 0;
    virtual ~DeviceState() {};
};