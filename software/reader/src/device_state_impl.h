#pragma once

#include "device_state.h"
#include "device.h"
#include <unistd.h>

class DeviceInit : public DeviceState
{
public:
    void enter(Device *device);
    void exit(Device *device);
    static DeviceState &getInstance();

private:
    DeviceInit() {};
    DeviceInit(const DeviceInit &other);
    DeviceInit &operator=(const DeviceInit &other);
};

class DeviceAPConnect : public DeviceState
{
public:
    void enter(Device *device);
    void exit(Device *device);
    static DeviceState &getInstance();
    useconds_t wifiDelay_;

private:
    DeviceAPConnect() { wifiDelay_ = 100000; }
    DeviceAPConnect(const DeviceAPConnect &other);
    DeviceAPConnect &operator=(const DeviceAPConnect &other);
};

class DeviceIPAddress : public DeviceState
{
public:
    void enter(Device *device);
    void exit(Device *device);
    static DeviceState &getInstance();

private:
    DeviceIPAddress() {}
    DeviceIPAddress(const DeviceIPAddress &other);
    DeviceIPAddress &operator=(const DeviceIPAddress &other);
};

class DeviceIdle : public DeviceState
{
public:
    void enter(Device *device);
    void exit(Device *device);
    static DeviceState &getInstance();

private:
    DeviceIdle() {}
    DeviceIdle(const DeviceIdle &other);
    DeviceIdle &operator=(const DeviceIdle &other);
};
