#include "device_state_impl.h"

DeviceState *backwardWifiTrans(WiFiEvent_t wifiEvent)
{
    DeviceState *newState = NULL;
    switch (wifiEvent)
    {
    case ARDUINO_EVENT_WIFI_STA_STOP:
        newState = &DeviceNetReset::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        newState = &DeviceNetReset::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        newState = &DeviceNetReset::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        newState = &DeviceNetReset::getInstance();
        break;
    default:
        break;
    }

    return newState;
}

DeviceState *ignoreThisEvent()
{
    return NULL;
}

/******************************************************************************

    DeviceNetReset

******************************************************************************/

void DeviceNetReset::enter(Device *device)
{
    device->getIndicators()->doPowerNetReset();
    WiFi.disconnect(true, false);
}

void DeviceNetReset::exit(Device *device)
{
    device->getIndicators()->powerLEDShutdown();
}

DeviceState &DeviceNetReset::getInstance()
{
    static DeviceNetReset singleton;
    return singleton;
}

String DeviceNetReset::name()
{
    return String("NetReset");
}

DeviceState *DeviceNetReset::badgeEvent(Device *device, uint32_t eventType, Tag *tag)
{
    return ignoreThisEvent();
}

DeviceState *DeviceNetReset::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{
    DeviceState *newState = NULL;

    switch (wifiEvent)
    {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        newState = &DeviceInit::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        newState = &DeviceNetReset::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        newState = &DeviceNetReset::getInstance();
        break;
    default:
        break;
    }

    return newState;
}

/******************************************************************************

    DeviceInit

******************************************************************************/
void DeviceInit::enter(Device *device)
{
    device->getIndicators()->doPowerInit();
    WiFi.enableSTA(true);
}

void DeviceInit::exit(Device *device)
{
    device->getIndicators()->powerLEDShutdown();
}

DeviceState & DeviceInit::getInstance()
{
    static DeviceInit singleton;
    return singleton;
}

String DeviceInit::name()
{
    return String("DeviceInit");
}

DeviceState *DeviceInit::badgeEvent(Device *device, uint32_t eventType, Tag *tag)
{
    return ignoreThisEvent();
}

DeviceState * DeviceInit::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{
    DeviceState *newState = NULL;

    switch (wifiEvent)
    {
    case ARDUINO_EVENT_WIFI_READY:
        break;
    case ARDUINO_EVENT_WIFI_STA_START:
        newState = &DeviceAPConnect::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        newState = &DeviceNetReset::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        newState = &DeviceNetReset::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        newState = &DeviceNetReset::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        newState = &DeviceNetReset::getInstance();
        break;
    default:
        break;
    }

    return newState;
}

/******************************************************************************

    DeviceAPConnect

******************************************************************************/
void DeviceAPConnect::enter(Device *device)
{
    device->getIndicators()->doPowerAPConnect();
    WiFi.begin(device->getSSID().c_str(), device->getPassword().c_str());
}

void DeviceAPConnect::exit(Device *device)
{
    device->getIndicators()->powerLEDShutdown();
}


DeviceState & DeviceAPConnect::getInstance()
{
    static DeviceAPConnect singleton;
    return singleton;
}

String DeviceAPConnect::name()
{
    return String("DeviceAPConnect");
}

DeviceState *DeviceAPConnect::badgeEvent(Device *device, uint32_t eventType, Tag *tag)
{
    return ignoreThisEvent();
}

DeviceState * DeviceAPConnect::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{
    DeviceState *newState = NULL;
    switch (wifiEvent)
    {
    case WIFI_EVENT_STA_CONNECTED:
        newState = &DeviceIPAddress::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        newState = &DeviceNetReset::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        newState = &DeviceNetReset::getInstance();
        break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        newState = &DeviceNetReset::getInstance();
        break;
    default:
        break;
    }

    return newState;
}

/******************************************************************************

    DeviceIPAddress

******************************************************************************/
void DeviceIPAddress::enter(Device *device)
{
    device->getIndicators()->doPowerIPAddress();
}

void DeviceIPAddress::exit(Device *device)
{
    device->getIndicators()->powerLEDShutdown();
}

DeviceState & DeviceIPAddress::getInstance()
{
    static DeviceIPAddress singleton;
    return singleton;
}

String DeviceIPAddress::name()
{
    return String("DeviceIPAddress");
}

DeviceState *DeviceIPAddress::badgeEvent(Device *device, uint32_t eventType, Tag *tag)
{
    return ignoreThisEvent();
}

DeviceState * DeviceIPAddress::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{

    switch (wifiEvent)
    {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        device->getIndicators()->doPowerPingWait();
        DeviceState *newState = NULL;
        PingReqResp pingReqResponse;
        pingReqResponse.stationId = device->getStationID();

        uint32_t delayMS = 250;      // 250 ms
        uint32_t maxDelay = 30000;   // about 30 seconds
        uint8_t maxIterations = 17; // about 5 minutes, 20 seconds
        uint8_t counter = 0;

        counter = 1;

        for (;;)
        {
            device->getClient()->ping(&pingReqResponse);
            if (!pingReqResponse.isError)
            {
                return &DeviceIdle::getInstance();
            }
            delay(delayMS);
            if (delayMS < maxDelay)
            {
                delayMS *= 2;
            }
            if (counter == maxIterations)
            {
                return &DeviceNetReset::getInstance();
            }
            counter++;
        }
    }

    return NULL;
}

/******************************************************************************

    DeviceIdle

******************************************************************************/
void DeviceIdle::enter(Device *device)
{
    device->getIndicators()->doPowerIdle();
}

void DeviceIdle::exit(Device *device)
{
    device->getIndicators()->powerLEDShutdown();
}

DeviceState & DeviceIdle::getInstance()
{
    static DeviceIdle singleton;
    return singleton;
}

String DeviceIdle::name()
{
    return String("DeviceIdle");
}

DeviceState *DeviceIdle::badgeEvent(Device *device, uint32_t eventType, Tag *tag)
{
    
    if (eventType == LabpassEvent::ShortLFBadge || eventType == LabpassEvent::ShortHFBadge)
    {
        AuthReqResp authReqResponse;
        authReqResponse.badgeId = tag->getID();
        authReqResponse.stationId = device->getStationID();
        device->getClient()->authorize(&authReqResponse);
        if (authReqResponse.isError)
        {
            device->getIndicators()->doAuthError();
            return NULL;
        }
        else if (!authReqResponse.isAuthorized)
        {
            device->getIndicators()->doNotAuthorized();
            return NULL;
        }
        else if (authReqResponse.isAuthorized)
        {
            if (authReqResponse.isManager)
            {
                device->getIndicators()->doShortBadgeInAuthOk();
                device->setManagerBadgeId(tag->getID());
                return &DeviceManagerActive::getInstance();
            }
            else
            {
                device->getIndicators()->doShortBadgeInAuthOk();
                device->setUserBadgeId(tag->getID());
                return &DeviceUserActive::getInstance();
            }
        }
    }
    return NULL;
}

DeviceState * DeviceIdle::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{
    return backwardWifiTrans(wifiEvent);
}


/******************************************************************************

    DeviceUserActive

******************************************************************************/
void DeviceUserActive::enter(Device *device)
{
    device->getIndicators()->doPowerIdle();
}

void DeviceUserActive::exit(Device *device)
{
    device->getIndicators()->powerLEDShutdown();
}

DeviceState &DeviceUserActive::getInstance()
{
    static DeviceUserActive singleton;
    return singleton;
}

String DeviceUserActive::name()
{
    return String("DeviceUserActive");
}

DeviceState *DeviceUserActive::badgeEvent(Device *device, uint32_t eventType, Tag *tag)
{

    if (eventType == LabpassEvent::ShortLFBadge || eventType == LabpassEvent::ShortHFBadge)
    {
        AuthReqResp authReqResponse;
        authReqResponse.badgeId = tag->getID();
        authReqResponse.stationId = device->getStationID();
        device->getClient()->authorize(&authReqResponse);
        if (authReqResponse.isError)
        {
            device->getIndicators()->doAuthError();
            return NULL;
        }
        else if (!authReqResponse.isAuthorized)
        {
            device->getIndicators()->doAuthError();
            return NULL;
        }
        else
        {
            if (!authReqResponse.isManager && authReqResponse.isLoggedOut)
            {
                device->setUserBadgeId("");
                device->getIndicators()->doShortBadgeOutAuthOk();
                return &DeviceIdle::getInstance();
            }
            else
            {
                device->getIndicators()->doAuthError();
                // TODO: indicate error
                return NULL;
            }
        }
    }

    return NULL;
}

DeviceState *DeviceUserActive::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{
    return backwardWifiTrans(wifiEvent);
}

/******************************************************************************

    DeviceManagerActive

******************************************************************************/
void DeviceManagerActive::enter(Device *device)
{
    device->getIndicators()->doPowerIdle();
}

void DeviceManagerActive::exit(Device *device)
{
    device->getIndicators()->powerLEDShutdown();
}

DeviceState &DeviceManagerActive::getInstance()
{
    static DeviceManagerActive singleton;
    return singleton;
}

String DeviceManagerActive::name()
{
    return String("DeviceManagerActive");
}

DeviceState *DeviceManagerActive::badgeEvent(Device *device, uint32_t eventType, Tag *tag)
{

    if (eventType == LabpassEvent::ShortLFBadge || eventType == LabpassEvent::ShortHFBadge)
    {
        AuthReqResp authReqResponse;
        authReqResponse.badgeId = tag->getID();
        authReqResponse.stationId = device->getStationID();
        device->getClient()->authorize(&authReqResponse);
        if (authReqResponse.isError)
        {
            device->getIndicators()->doAuthError();
            return &DeviceInit::getInstance();
        }
        else if (!authReqResponse.isAuthorized)
        {
            device->getIndicators()->doAuthError();
            return &DeviceInit::getInstance();
        }
        else
        {
            if (authReqResponse.isManager && authReqResponse.isLoggedOut)
            {
                device->setManagerBadgeId("");
                device->getIndicators()->doShortBadgeOutAuthOk();
                return &DeviceIdle::getInstance();
            }
            else
            {
                device->getIndicators()->doAuthError();
                return NULL;
            }
        }
    }
    else if (eventType == LabpassEvent::LongLFBadge || eventType == LabpassEvent::LongHFBadge)
    {
        AuthReqResp authReqResponse;
        authReqResponse.badgeId = tag->getID();
        authReqResponse.stationId = device->getStationID();
        device->getClient()->authorize(&authReqResponse);
        if (authReqResponse.isError)
        {
            device->getIndicators()->doAuthError();
            return NULL;
        }
        else if (!authReqResponse.isAuthorized)
        {
            device->getIndicators()->doNotAuthorized();
            return NULL;
        }
        else if (authReqResponse.isAuthorized && authReqResponse.isManager)
        {
            device->getIndicators()->statusLEDShutdown();
            device->getIndicators()->doEnrollState();

            return &DeviceEnroll::getInstance();
        }
    }

    return NULL;
}

DeviceState *DeviceManagerActive::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{
    return backwardWifiTrans(wifiEvent);
}

/******************************************************************************

    DeviceEnroll

******************************************************************************/
void DeviceEnroll::enter(Device *device)
{
    device->getIndicators()->doPowerIdle();
    device->stopEnrollTimer();
    device->startEnrollTimer();
}

void DeviceEnroll::exit(Device *device)
{
    device->getIndicators()->powerLEDShutdown();
}

DeviceState &DeviceEnroll::getInstance()
{
    static DeviceEnroll singleton;
    return singleton;
}

String DeviceEnroll::name()
{
    return String("DeviceEnroll");
}

DeviceState *DeviceEnroll::badgeEvent(Device *device, uint32_t eventType, Tag *tag)
{

    if (eventType == LabpassEvent::ShortLFBadge || eventType == LabpassEvent::ShortHFBadge)
    {
        if (tag->getID() == device->getManagerBadgeId())
        {
            return NULL;
        }

        EnrollReqResp enrollReqResponse;
        enrollReqResponse.userBadgeId = tag->getID();
        enrollReqResponse.managerBadgeId = device->getManagerBadgeId();
        enrollReqResponse.stationId = device->getStationID();

        device->getClient()->enroll(&enrollReqResponse);
        if (enrollReqResponse.isError)
        {
            device->getIndicators()->doAuthError();
            return &DeviceIdle::getInstance();
        }
        else if (enrollReqResponse.isEnrolled)
        {
            device->getIndicators()->statusLEDShutdown();
            device->getIndicators()->doEnrollSuccess();
            delayMicroseconds(400000);
            device->getIndicators()->statusLEDShutdown();
            device->getIndicators()->doEnrollContinue();
            return &DeviceEnroll::getInstance();
        }
    }

    return &DeviceIdle::getInstance();
}

DeviceState *DeviceEnroll::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{
    return backwardWifiTrans(wifiEvent);
}
