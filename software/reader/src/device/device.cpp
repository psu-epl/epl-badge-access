#include "device.h"

ESP_EVENT_DEFINE_BASE(LabpassReaderEvent);

#define MAX_BACKOFF 30000

Device::Device(const String &ssid, const String &password, esp_event_loop_handle_t loop_handle,
               LabpassClient *client, Indicators *indicators, String stationID) : ssid_(ssid),
                                                                                  password_(password),
                                                                                  backoff_(0),
                                                                                  loop_handle_(loop_handle),
                                                                                  client_(client),
                                                                                  currentState_(&DeviceInit::getInstance()),
                                                                                  indicators_(indicators),
                                                                                  stationID_(stationID),
                                                                                  stateChangeMutex_(xSemaphoreCreateMutex())

{
    ESP_ERROR_CHECK(esp_event_handler_register_with(loop_handle, LabpassReaderEvent, ESP_EVENT_ANY_ID, &Device::labpassReaderEventHdr, (void *)this));
    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info)
                 { this->wifiEventHdr(event, info); });

    esp_timer_create_args_t enrollExitTimerArgs{
        .callback = Device::enrollExitTimerExpire,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "enroll_exit_timer",
        .skip_unhandled_events = true};

    esp_timer_create(&enrollExitTimerArgs, &enrollExitTimer_);
}

void Device::start()
{
    setState(DeviceAPConnect::getInstance());
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

void Device::setState(DeviceState &newState)
{
    xSemaphoreTake(stateChangeMutex_, portMAX_DELAY);
    doBackoff(newState);
    currentState_->exit(this);
    currentState_ = &newState;
    currentState_->enter(this);
    xSemaphoreGive(stateChangeMutex_);
}

esp_event_loop_handle_t Device::getEventLoop()
{
    return loop_handle_;
}

void Device::labpassReaderEventHdr(void *event_handler_arg, esp_event_base_t event_base, int event_id, void *event_data)
{
    Device *device = (Device *)event_handler_arg;

    Tag *tag = (Tag *)event_data;

    DeviceState *nextState = device->getCurrentState()->badgeEvent(
        (Device *)event_handler_arg,
        event_id,
        (Tag *)event_data);

    if (nextState)
    {
        device->setState(*nextState);
    } else
    {
    }
}

void Device::wifiEventHdr(WiFiEvent_t event, WiFiEventInfo_t info)
{
    DeviceState * nextState = currentState_->wifiEvent(this, event, info);

    if (nextState)
    {
        setState(*nextState);
    }
}

void Device::enrollExitTimerExpire(void *p)
{
    Device *that = (Device *)p;
    that->getIndicators()->statusLEDShutdown();
    that->setState(DeviceIdle::getInstance());
}

void Device::startEnrollTimer()
{
    esp_timer_start_once(enrollExitTimer_, 10000000);
}

void Device::stopEnrollTimer()
{
    esp_timer_stop(enrollExitTimer_);
}
