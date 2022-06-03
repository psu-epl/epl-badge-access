#include "device_state_impl.h"

// DeviceState *DeviceBadgeIn::clientEvent(Device *device, uint32_t eventType, void *eventData)
// {
//     log_i("badgeRead client event");
//     DeviceState *newState = NULL;

//     if (device->getActiveUser() == NULL)
//     {
//         log_i("active user is null");
//         if (eventType == LabpassClientEventType::AuthSuccess)
//         {
//             log_i("auth success");
//             device->setActiveUser((AuthResponse *)eventData);

//             if (device->getActiveUser()->isManager)
//             {
//                 newState = &DeviceManagerActive::getInstance();
//             }
//             else
//             {
//                 newState = &DeviceUserActive::getInstance();
//             }
//         }
//         else if (eventType == LabpassClientEventType::AuthFail)
//         {
//             log_i("auth fail");
//             newState = &DeviceIdle::getInstance();
//         }
//     }
//     else
//     {
//         log_i("active user is not null");
//         AuthResponse *newAuthResponse = (AuthResponse *)eventData;
//         if (eventType == LabpassClientEventType::AuthSuccess)
//         {
//             log_i("auth success");
//             if (device->getActiveUser()->badgeId == newAuthResponse->badgeId)
//             {
//                 newState = &DeviceIdle::getInstance();
//                 delete device->getActiveUser();
//                 device->setActiveUser(NULL);
//             }
//         }
//     }

//     return newState;
// }

// DeviceState *DeviceBadgeOut::clientEvent(Device *device, uint32_t eventType, void *eventData)
// {
//     log_i("badgeRead client event");
//     DeviceState *newState = NULL;

//     if (device->getActiveUser() != NULL)
//     {
//         log_i("active user is not null");
//         if (eventType == LabpassClientEventType::AuthSuccess)
//         {
//             AuthResponse *authResponse = (AuthResponse *) eventData;
//             log_i("auth success");

//             if (authResponse->badgeId == device->getActiveUser()->badgeId)
//             {
//                 delete device->getActiveUser();
//                 device->setActiveUser(NULL);
//                 newState = &DeviceIdle::getInstance();
//             }
//             else
//             {
//                 //enroll
//             }
//         }
//         else if (eventType == LabpassClientEventType::AuthFail)
//         {
//             // auth failed; indicate but don't change state
//         }
//     }

//     return newState;
// }

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
    log_i("DeviceNetReset enter");
    WiFi.disconnect(true, false);
}

void DeviceNetReset::exit(Device *device)
{
    log_i("DeviceNetReset exit");
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

DeviceState *DeviceNetReset::badgeEvent(Device *device, uint32_t eventType, String *tag)
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
    log_i("DeviceInit enter");
    WiFi.enableSTA(true);
}

void DeviceInit::exit(Device *device)
{
    log_i("DeviceInit exit");
}

DeviceState & DeviceInit::getInstance()
{
    static DeviceInit singleton;
    return singleton;
}

String DeviceInit::name()
{
    return String("INIT");
}

DeviceState *DeviceInit::badgeEvent(Device *device, uint32_t eventType, String *tag)
{
    return ignoreThisEvent();
}

DeviceState * DeviceInit::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{
    log_i("init: event: %d", wifiEvent);
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
    log_i("DeviceAPConnect enter");
    WiFi.begin(device->getSSID().c_str(), device->getPassword().c_str());
}

void DeviceAPConnect::exit(Device *device)
{
    log_i("DeviceAPConnect exit");
}


DeviceState & DeviceAPConnect::getInstance()
{
    static DeviceAPConnect singleton;
    return singleton;
}

String DeviceAPConnect::name()
{
    return String("INIT");
}

DeviceState *DeviceAPConnect::badgeEvent(Device *device, uint32_t eventType, String *tag)
{
    return ignoreThisEvent();
}

DeviceState * DeviceAPConnect::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{
    log_i("state APConnect, got event %d", wifiEvent);
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
    ("DeviceIPAddress enter");
}

void DeviceIPAddress::exit(Device *device)
{
    ("DeviceIPAddress exit");
}

DeviceState & DeviceIPAddress::getInstance()
{
    static DeviceIPAddress singleton;
    return singleton;
}

String DeviceIPAddress::name()
{
    return String("IP_ADDRESS");
}

DeviceState *DeviceIPAddress::badgeEvent(Device *device, uint32_t eventType, String *tag)
{
    return ignoreThisEvent();
}

DeviceState * DeviceIPAddress::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{
    DeviceState * newState = NULL;
    PingReqResp pingReqResponse;

    switch (wifiEvent)
    {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        uint32_t delayMS = 250;      // 250 ms
        uint32_t maxDelay = 30000;   // about 30 seconds
        uint8_t maxIterations = 17; // about 5 minutes, 20 seconds
        uint8_t counter = 0;
        for (;;)
        {
            counter = 1;
            device->client_->ping(&pingReqResponse);
            if (pingReqResponse.isError)
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
        }
    }

    return newState;
}

/******************************************************************************

    DeviceIdle

******************************************************************************/
void DeviceIdle::enter(Device *device)
{
    log_i("DeviceIdle enter");
}

void DeviceIdle::exit(Device *device)
{
    log_i("DeviceIdle exit");
}

DeviceState & DeviceIdle::getInstance()
{
    static DeviceIdle singleton;
    return singleton;
}

String DeviceIdle::name()
{
    return String("IDLE");
}

DeviceState *DeviceIdle::badgeEvent(Device *device, uint32_t eventType, String *tag)
{
    log_i("Badge Event in state Device Idle");
    log_i("tag: %s",  tag->c_str());

    AuthReqResp authReqResp;
    device->client_->authorize(&authReqResp);
    if (authReqResp.isError)
    {
        // TODO: indicate error
        return &DeviceInit::getInstance();
    }
    else if (!authReqResp.isAuthorized)
    {
        // TODO: indicate unauthorized
        return &DeviceInit::getInstance();
    }
    else
    {
        if (authReqResp.isManager)
        {
            if (eventType == labpassFakeReaderEventType::shortBadge)
            {
                return &DeviceManagerActive::getInstance();
            }
            else
            {
                return &DeviceEnroll::getInstance();
            }
        }
        else
        {
            return &DeviceUserActive::getInstance();
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
    log_i("DeviceUserActive enter");
}

void DeviceUserActive::exit(Device *device)
{
    log_i("DeviceUserActive exit");
}

DeviceState &DeviceUserActive::getInstance()
{
    static DeviceUserActive singleton;
    return singleton;
}

String DeviceUserActive::name()
{
    return String("UserActive");
}

DeviceState *DeviceUserActive::badgeEvent(Device *device, uint32_t eventType, String *tag)
{
    log_i("Badge Event in state Device Idle");
    log_i("tag: %s", tag->c_str());

    if (eventType == labpassFakeReaderEventType::shortBadge)
    {
        AuthReqResp authReqResponse;
        device->client_->authorize(&authReqResponse);
        if (authReqResponse.isError)
        {
            // TODO: indicate error
            return &DeviceInit::getInstance();
        }
        else if (!authReqResponse.isAuthorized)
        {
            // TODO: indicate unauthorized
            return &DeviceInit::getInstance();
        }
        else
        {
            if (!authReqResponse.isManager && authReqResponse.isLoggedOut)
            {
                return &DeviceIdle::getInstance();
            }
            else {
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
    log_i("DeviceManagerActive enter");
}

void DeviceManagerActive::exit(Device *device)
{
    log_i("DeviceManagerActive exit");
}

DeviceState &DeviceManagerActive::getInstance()
{
    static DeviceManagerActive singleton;
    return singleton;
}

String DeviceManagerActive::name()
{
    return String("ManagerActive");
}

DeviceState *DeviceManagerActive::badgeEvent(Device *device, uint32_t eventType, String *tag)
{
    log_i("Badge Event in state Device Idle");
    log_i("tag: %s", tag->c_str());

    AuthReqResp authReqResponse;
    device->client_->authorize(&authReqResponse);
    if (authReqResponse.isError)
    {
        // TODO: indicate error
        return &DeviceInit::getInstance();
    }
    else if (!authReqResponse.isAuthorized)
    {
        // TODO: indicate unauthorized
        return &DeviceInit::getInstance();
    }
    else
    {
        if (authReqResponse.isManager && authReqResponse.isLoggedOut)
        {
            return &DeviceIdle::getInstance();
        }
        else
        {
            // TODO: indicate error
            return NULL;
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
    log_i("DeviceManagerActive enter");
}

void DeviceEnroll::exit(Device *device)
{
    log_i("DeviceManagerActive exit");
}

DeviceState &DeviceEnroll::getInstance()
{
    static DeviceEnroll singleton;
    return singleton;
}

String DeviceEnroll::name()
{
    return String("ManagerActive");
}

DeviceState *DeviceEnroll::badgeEvent(Device *device, uint32_t eventType, String *tag)
{
    log_i("Badge Event in state Device Idle");
    log_i("tag: %s", tag->c_str());

    AuthReqResp authReqResponse;
    device->client_->authorize(&authReqResponse);
    if (authReqResponse.isError)
    {
        // TODO: indicate error
        return &DeviceInit::getInstance();
    }
    else if (!authReqResponse.isAuthorized)
    {
        // TODO: indicate unauthorized
        return &DeviceInit::getInstance();
    }
    else
    {
        if (authReqResponse.isManager && authReqResponse.isLoggedOut)
        {
            return &DeviceIdle::getInstance();
        }
        else
        {
            // TODO: indicate error
            return NULL;
        }
    }

    return &DeviceIdle::getInstance();
}

DeviceState *DeviceEnroll::wifiEvent(Device *device, WiFiEvent_t wifiEvent, WiFiEventInfo_t info)
{
    return backwardWifiTrans(wifiEvent);
}
