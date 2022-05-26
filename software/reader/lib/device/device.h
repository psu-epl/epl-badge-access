#pragma once

#include <device_state.h>
#include <unistd.h>
#include <WiFi.h>
#include <client.h>
#include <unistd.h>
#include <device_state_impl.h>
#include <fake_reader.h>
#include <labpass_event.h>

class DeviceState;

class Device
{
public:
    Device(const String &ssid, const String &password, esp_event_loop_handle_t loop_handle, LabpassClient *client);
    inline DeviceState* getCurrentState() const { return currentState_; }
    void setstate(DeviceState &newState);
    void start();
    void wifiReconnect();
    esp_err_t postEvent(esp_event_base_t event_base, int32_t event_id);
    esp_event_loop_handle_t & getEventLoop();
    LabpassClient *client_;
    inline const String & getSSID() { return ssid_; }
    inline const String &getPassword() { return password_; }

private:
    DeviceState *currentState_;
    const String &ssid_;
    const String &password_;
    SemaphoreHandle_t mutex_;
    void wifiEventHdr(WiFiEvent_t event, WiFiEventInfo_t info);
    static void labpassFakeReaderEventHdr(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    static void labpassClientEventHdr(void *event_handler_arg, esp_event_base_t event_base, int event_id, void *event_data);
    esp_event_loop_handle_t loop_handle_;
    u32_t backoff_;
    void doBackoff(DeviceState &newState);
};
