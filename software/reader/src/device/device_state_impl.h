#pragma once

#include "device_state.h"
#include "device.h"
#include <unistd.h>
#include "labpass_client/client.h"

class DeviceNetReset : public DeviceState
{
public:
    void enter(Device *device);
    void exit(Device *device);
    DeviceState *badgeEvent(Device *device, uint32_t eventType, Tag *tag);
    DeviceState *wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info);

    static DeviceState &getInstance();
    String name();

private:
    DeviceNetReset(){};
    DeviceNetReset(const DeviceNetReset &other);
    DeviceNetReset &operator=(DeviceNetReset &other);
};

class DeviceInit : public DeviceState
{
public:
    void enter(Device *device);
    void exit(Device *device);
    DeviceState *  badgeEvent(Device *device, uint32_t eventType,Tag *tag);
    DeviceState *  wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info);

    static DeviceState &getInstance();
    String name();

private:
    DeviceInit() {};
    DeviceInit(const DeviceInit &other);
    DeviceInit &operator=(DeviceInit &other);
};

class DeviceAPConnect : public DeviceState
{
public:
    void enter(Device *device);
    void exit(Device *device);
    DeviceState *  badgeEvent(Device *device, uint32_t eventType,Tag *tag);
    DeviceState *  wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info);
    static DeviceState &getInstance();
    String name();

private:
    DeviceAPConnect() {}
    DeviceAPConnect(const DeviceAPConnect &other);
    DeviceAPConnect &operator=(const DeviceAPConnect &other);
};

class DeviceIPAddress : public DeviceState
{
public:
    void enter(Device *device);
    void exit(Device *device);
    DeviceState *badgeEvent(Device *device, uint32_t eventType, Tag *tag);
    DeviceState *  wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info);
    static DeviceState &getInstance();
    String name();

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
    DeviceState *badgeEvent(Device *device, uint32_t eventType, Tag *tag);
    DeviceState *  wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info);
    static DeviceState &getInstance();
    String name();

private:
    DeviceIdle() {}
    DeviceIdle(const DeviceIdle &other);
    DeviceIdle &operator=(const DeviceIdle &other);
};

class DeviceUserActive : public DeviceState
{
public:
    void enter(Device *device);
    void exit(Device *device);
    DeviceState *badgeEvent(Device *device, uint32_t eventType, Tag *tag);
    DeviceState *wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info);
    static DeviceState &getInstance();
    String name();

private:
    DeviceUserActive() {}
    DeviceUserActive(const DeviceUserActive &other);
    DeviceUserActive &operator=(const DeviceUserActive &other);
};

class DeviceManagerActive : public DeviceState
{
public:
    void enter(Device *device);
    void exit(Device *device);
    DeviceState *badgeEvent(Device *device, uint32_t eventType, Tag *tag);
    DeviceState *wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info);
    static DeviceState &getInstance();
    String name();

private:
    DeviceManagerActive() {}
    DeviceManagerActive(const DeviceManagerActive &other);
    DeviceManagerActive &operator=(const DeviceManagerActive &other);
};

class DeviceEnroll : public DeviceState
{
public:
    void enter(Device *device);
    void exit(Device *device);
    DeviceState *badgeEvent(Device *device, uint32_t eventType, Tag *tag);
    DeviceState *wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info);
    static DeviceState &getInstance();
    String name();

private:
    DeviceEnroll() {}
    DeviceEnroll(const DeviceEnroll &other);
    DeviceEnroll &operator=(const DeviceEnroll &other);
};
