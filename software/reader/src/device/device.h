#pragma once

#include "device_state.h"
#include <unistd.h>
#include <WiFi.h>
#include "labpass_client/client.h"
#include <unistd.h>
#include "device_state_impl.h"
#include "labpass_event/labpass_event.h"
#include "low_freq_impl/low_freq_impl.h"
#include "high_freq/high_freq.h"
#include "indicators/indicators.h"
#include "labpass_event/labpass_event.h"
#include <string>

using namespace std;

class DeviceState;

class Device
{
public:
    Device(const String &ssid, const String &password, esp_event_loop_handle_t loop_handle, 
        LabpassClient *client, Indicators * indicators, String StationID);
    inline DeviceState* getCurrentState() const { return currentState_; }
    void setstate(DeviceState &newState);
    void start();
    void wifiReconnect();
    esp_err_t postEvent(esp_event_base_t event_base, int32_t event_id);
    esp_event_loop_handle_t & getEventLoop();
    inline const String & getSSID() { return ssid_; }
    inline const String &getPassword() { return password_; }
    inline Indicators *getIndicators() { return indicators_; }
    inline String getStationID() { return stationID_; }
    inline LabpassClient * getClient() { return client_; }


private:
    void wifiEventHdr(WiFiEvent_t event, WiFiEventInfo_t info);
    static void labpassReaderEventHdr(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    static void labpassClientEventHdr(void *event_handler_arg, esp_event_base_t event_base, int event_id, void *event_data);
    void doBackoff(DeviceState &newState);

    DeviceState *currentState_;
    const String &ssid_;
    const String &password_;
    SemaphoreHandle_t mutex_;
    esp_event_loop_handle_t loop_handle_;
    u32_t backoff_;
    Indicators *indicators_;
    String stationID_;
    LabpassClient *client_;

};
