#include "device.h"

ESP_EVENT_DEFINE_BASE(LabpassClientEvent);
ESP_EVENT_DEFINE_BASE(LabpassFakeReaderEvent);

#define MAX_BACKOFF 30000

Device::Device(const String &ssid, const String &password, esp_event_loop_handle_t loop_handle, LabpassClient *client) : ssid_(ssid),
                                                                                                                         password_(password),
                                                                                                                         mutex_(xSemaphoreCreateMutex()),
                                                                                                                         backoff_(0),
                                                                                                                         loop_handle_(loop_handle),
                                                                                                                         client_(client),
                                                                                                                         currentState_(&DeviceInit::getInstance())

{
    esp_event_handler_register_with(loop_handle_, LabpassFakeReaderEvent, ESP_EVENT_ANY_ID, &Device::labpassFakeReaderEventHdr, (void *)this);
    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info)
                 { this->wifiEventHdr(event, info); });
}

void Device::start()
{
    setstate(DeviceInit::getInstance());
}

void Device::doBackoff(DeviceState &newState)
{
    if (currentState_->name() == newState.name())
    {
        if (backoff_ == 0)
        {
            backoff_ = 250 / portTICK_PERIOD_MS;
        }

        vTaskDelay(backoff_);

        if (backoff_ < MAX_BACKOFF)
        {
            backoff_ *= 2;
        }
    }
    else if (backoff_ > 0)
    {
        backoff_ = 0;
    }
}

void Device::setstate(DeviceState &newState)
{
    doBackoff(newState);
    currentState_->exit(this);
    currentState_ = &newState;
    currentState_->enter(this);
}

esp_event_loop_handle_t & Device::getEventLoop()
{
    return loop_handle_;
}

void Device::labpassFakeReaderEventHdr(void *event_handler_arg, esp_event_base_t event_base, int event_id, void *event_data)
{
    Device *device = (Device *)event_handler_arg;

    String *foo = (String *)event_data;
    log_i("fake reader event handler, tag: %s", foo->c_str());
    log_i("fake reader event handler, tag: %s", ((String *) event_data)->c_str());
    log_i("fake reader event handler, tag: %s", ((String *)event_data)->c_str());

    DeviceState * nextState = device->getCurrentState()->badgeEvent(
        (Device *) event_handler_arg, 
        event_id, 
        (String *) event_data);

    if (nextState)
    {
        device->setstate(*nextState);
    }

}

void Device::wifiEventHdr(WiFiEvent_t event, WiFiEventInfo_t info)
{
    log_i("event handler, sent event: %d", event);
    DeviceState * nextState = currentState_->wifiEvent(this, event, info);

    if (nextState)
    {
        setstate(*nextState);
    }
}
