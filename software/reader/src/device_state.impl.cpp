#include "device_state_impl.h"

/*
    DeviceInit
*/

void DeviceInit::enter(Device *device)
{
    Serial.printf("DeviceInit enter\n");
}

void DeviceInit::exit(Device *device)
{
    Serial.printf("DeviceInit exit\n");
}

DeviceState &DeviceInit::getInstance()
{
    static DeviceInit singleton;
    return singleton;
}

/*
    DeviceAPConnect
*/

void DeviceAPConnect::enter(Device *device)
{
    Serial.printf("DeviceAPConnect enter\n");
    device->wifiReconnect();
}

void DeviceAPConnect::exit(Device *device)
{
    Serial.printf("DeviceAPConnect exit\n");
}


DeviceState &DeviceAPConnect::getInstance()
{
    static DeviceAPConnect singleton;
    return singleton;
}

/*
    DeviceAPConnect
*/

void DeviceIPAddress::enter(Device *device)
{
    Serial.printf("DeviceIPAddress enter\n");
}

void DeviceIPAddress::exit(Device *device)
{
    Serial.printf("DeviceIPAddress exit\n");
}

DeviceState &DeviceIPAddress::getInstance()
{
    static DeviceIPAddress singleton;
    return singleton;
}

/*
    DeviceIdle
*/

void DeviceIdle::enter(Device *device)
{
    Serial.printf("DeviceIdle enter\n");
}

void DeviceIdle::exit(Device *device)
{
    Serial.printf("DeviceIdle exit\n");
}

DeviceState &DeviceIdle::getInstance()
{
    static DeviceIdle singleton;
    return singleton;
}
