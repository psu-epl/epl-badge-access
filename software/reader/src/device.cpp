#include "device.h"
#include "device_state_impl.h"

Device::Device(const char *ssid, const char *password) : ssid_(ssid),
                                                         password_(password),
                                                         mutex_(xSemaphoreCreateMutex())
{
    currentState = &DeviceInit::getInstance();
}

void Device::wifiReconnect()
{
    WiFi.reconnect();
}

void Device::init()
{
    WiFi.begin(ssid_, password_);
    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info)
                 { this->wifiEvent(event, info); },
                 WiFiEvent_t::ARDUINO_EVENT_MAX);
}

void Device::setstate(DeviceState &newState)
{
    xSemaphoreTake(mutex_, portMAX_DELAY);
    currentState->exit(this);
    currentState = &newState;
    currentState->enter(this);
    xSemaphoreGive(mutex_);
}

void Device::wifiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event)
    {
    case ARDUINO_EVENT_WIFI_READY:
        Serial.println("WiFi interface ready");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
        Serial.println("Completed scan for access points");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("WiFi client started");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        Serial.println("WiFi clients stopped");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("Connected to access point");
        this->setstate(DeviceIPAddress::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("Disconnected from WiFi access point");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        Serial.println("Authentication mode of access point has changed");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.print("Obtained IP address: ");
        Serial.println(WiFi.localIP());
        this->setstate(DeviceIdle::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        Serial.println("Lost IP address and IP address is reset to 0");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
        Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
        Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
        Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WPS_ER_PIN:
        Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_AP_START:
        Serial.println("WiFi access point started");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
        Serial.println("WiFi access point  stopped");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        Serial.println("Client connected");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
        Serial.println("Client disconnected");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
        Serial.println("Assigned IP address to client");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
        Serial.println("Received probe request");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
        Serial.println("AP IPv6 is preferred");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        Serial.println("STA IPv6 is preferred");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_ETH_GOT_IP6:
        Serial.println("Ethernet IPv6 is preferred");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_ETH_START:
        Serial.println("Ethernet started");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_ETH_STOP:
        Serial.println("Ethernet stopped");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_ETH_CONNECTED:
        Serial.println("Ethernet connected");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        Serial.println("Ethernet disconnected");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    case ARDUINO_EVENT_ETH_GOT_IP:
        Serial.println("Obtained IP address");
        this->setstate(DeviceAPConnect::getInstance());
        break;
    default:
        break;
    }
}
