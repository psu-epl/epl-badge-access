#include "low_freq_impl/low_freq_impl.h"

static volatile bool timer_active = false;
static volatile uint64_t last_badge = 0;

using namespace std;

#define TIMER_DIVIDER (16)                           //  Hardware timer clock divider
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER) // convert counter value to seconds

LowFrequencyImpl::LowFrequencyImpl(esp_event_loop_handle_t eventLoop, uint8_t rx, uint8_t tx) : rx_(rx),
                                                                                                tx_(tx),
                                                                                                edgeQueue_(xQueueCreate(1024, sizeof(uint32_t))),
                                                                                                eventLoop_(eventLoop),
                                                                                                badgeDuration_(0),
                                                                                                currentState_(&LFStateSync::getInstance()),
                                                                                                lastTagMutex_(xSemaphoreCreateMutex()),
                                                                                                stateChangeMutex_(xSemaphoreCreateMutex()),
                                                                                                getQueueMutex_(xSemaphoreCreateMutex())

{
    pinMode(tx_, OUTPUT);
    pinMode(rx_, INPUT);

    esp_timer_create_args_t createArgs{
        .callback = LowFrequencyImpl::timerExpire,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "badge_duration_timer",
        .skip_unhandled_events = true};

    ESP_ERROR_CHECK(esp_timer_create(&createArgs, &timerHandle_));
}

QueueHandle_t LowFrequencyImpl::getEdgeQueue()
{
    QueueHandle_t queueHandle;
    xSemaphoreTake(getQueueMutex_, portMAX_DELAY);
    queueHandle = edgeQueue_;
    xSemaphoreGive(getQueueMutex_);
    return edgeQueue_;
}

esp_err_t LowFrequencyImpl::start()
{
    carrierOn();

    mcpwm_pin_config_t pinConfig = {
        .mcpwm_cap0_in_num = rx_};

    mcpwm_capture_config_t capConfig = {
        .cap_edge = MCPWM_POS_EDGE,
        .cap_prescale = 1,
        .capture_cb = LowFrequencyImpl::edgeCallback,
        .user_data = this
    };


    ESP_ERROR_CHECK(mcpwm_capture_enable_channel(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, &capConfig));
    ESP_ERROR_CHECK(mcpwm_set_pin(MCPWM_UNIT_0, &pinConfig));


    xTaskCreate(
        edgeTask,
        "edge_task_loop",
        4096,
        this,
        8,
        NULL);

    log_i("starting lf rx loop");
    return 0;
}

void LowFrequencyImpl::edgeTask(void *p)
{
    LowFrequencyImpl *that = (LowFrequencyImpl *)p;
    LFState::EdgeType edgeType;
    LFState::NextState nextState = LFState::NoChange;
    QueueHandle_t queueHandle = that->getEdgeQueue();
    vector<LFState::EdgeType> fragBuf;

    for (;;)
    {
        if (xQueueReceive(queueHandle, &edgeType, portMAX_DELAY))
        {
            if (fragBuf.empty())
            {
                fragBuf.push_back(edgeType);
            }
            else
            {
                if (edgeType == LFState::EdgeType::E1)
                {
                    fragBuf.push_back(edgeType);

                    if (fragBuf.size() == 5)
                    {
                        nextState = that->getCurrentState()->edgeEvent(that, fragBuf[0]);

                        switch (nextState)
                        {
                            case LFState::NoChange:
                                break;
                            case LFState::Sync:
                                that->setState(LFStateSync::getInstance());
                                break;
                            case LFState::Padding:
                                that->setState(LFStatePadding::getInstance());
                                break;
                            case LFState::Payload:
                                that->setState(LFStatePayload::getInstance());
                                break;
                            default:
                                break;
                            }

                        fragBuf.clear();
                    }
                }
                else
                {
                    fragBuf.clear();
                }
            }
        }
        else
        {
            fragBuf.clear();
        }
    }
}

void LowFrequencyImpl::setState(LFState &newState)
{
    currentState_->exit(this);
    currentState_ = &newState;
    currentState_->enter(this);
}

Tag LowFrequencyImpl::getLastTag()
{
    xSemaphoreTake(lastTagMutex_, portMAX_DELAY);
    Tag newTag(lastTag_);
    xSemaphoreGive(lastTagMutex_);
    return newTag;
}

void LowFrequencyImpl::setLastTag(Tag tag)
{
    xSemaphoreTake(lastTagMutex_, portMAX_DELAY);
    lastTag_ = tag;
    xSemaphoreGive(lastTagMutex_);
}

void LowFrequencyImpl::sendTag(Tag *tag)
{
    if (esp_timer_is_active(timerHandle_))
    {
        if (millis() - badgeDuration_ > 5000)
        {
            esp_event_post_to(
                eventLoop_,
                LabpassReaderEvent,
                LabpassEvent::LongLFBadge,
                (void *)&tag,
                sizeof(tag),
                portMAX_DELAY);

            esp_timer_stop(timerHandle_);
            last_badge = millis();
        }
        else
        {
            esp_timer_stop(timerHandle_);
            esp_timer_start_once(timerHandle_, 400000);
            setLastTag(*tag);
        }
    }
    else
    {
        esp_timer_start_once(timerHandle_, 400000);
        badgeDuration_ = millis();
        setLastTag(*tag);
    }
}

void LowFrequencyImpl::timerExpire(void *p)
{
    LowFrequencyImpl *that = (LowFrequencyImpl *)p;
    last_badge = millis();

    Tag lastTag = that->getLastTag();
    log_i("timer expired SENDING");
    esp_event_post_to(
        that->getEventLoop(),
        LabpassReaderEvent,
        LabpassEvent::ShortLFBadge,
        (void *)&lastTag,
        sizeof(lastTag),
        portMAX_DELAY);
}

bool IRAM_ATTR LowFrequencyImpl::edgeCallback(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata, void *user_data)
{
    static uint32_t previousEdgeTimestamp = 0;
    BaseType_t high_task_wakeup = pdFALSE;
    LowFrequencyImpl *that = (LowFrequencyImpl *)user_data;

    uint32_t edgePeriod = edata->cap_value - previousEdgeTimestamp;
    previousEdgeTimestamp = edata->cap_value;

    LFState::EdgeType edgeType;

    if (IN_GROUP(edgePeriod))
    {
        edgeType = LFState::EdgeType::E1;
    }
    else if (ZERO_BIT(edgePeriod))
    {
        edgeType = LFState::EdgeType::E01;
    }
    else if (TWO_ZERO_BITS(edgePeriod))
    {
        edgeType = LFState::EdgeType::E001;
    }
    else if (ZEROS_START_MARKER(edgePeriod))
    {
        edgeType = LFState::EdgeType::E0001;
    }
    else
    {
        return high_task_wakeup;
    }

    high_task_wakeup = xQueueSend(that->getEdgeQueue(), &edgeType, portMAX_DELAY);
    return high_task_wakeup;
}

void LowFrequencyImpl::carrierOn()
{
    ledc_timer_config_t timerConfig{
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_1_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 125000};

    ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_timer_config(&timerConfig));

    ledc_channel_config_t channelConfig{
        .gpio_num = TX,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = PWM_DUTYCYCLE,
        .hpoint = 0xffff,
    };

    ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_channel_config(&channelConfig));

    ledcAttachPin(TX, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, PWM_DUTYCYCLE);
}
