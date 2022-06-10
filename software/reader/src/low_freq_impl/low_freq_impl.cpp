#include "low_freq_impl/low_freq_impl.h"

static volatile bool timer_active = false;
static volatile uint64_t last_badge = 0;

using namespace std;
ESP_EVENT_DEFINE_BASE(LabpassLFReaderEvent);

#define TIMER_DIVIDER (16)                           //  Hardware timer clock divider
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER) // convert counter value to seconds

LowFrequencyImpl::LowFrequencyImpl(esp_event_loop_handle_t eventLoop, uint8_t rx, uint8_t tx) : rx_(rx),
                                                                                                tx_(tx),
                                                                                                edgeQueue_(xQueueCreate(4096, sizeof(uint32_t))),
                                                                                                eventLoop_(eventLoop),
                                                                                                badgeDuration_(0),
                                                                                                fragBuf_(vector<LFState::EdgeType>()),
                                                                                                currentState_(&LFStateSync::getInstance()),
                                                                                                lastTagMutex_(xSemaphoreCreateMutex())

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

esp_err_t LowFrequencyImpl::start()
{
    if (esp_err_t cerr = LowFrequencyImpl::carrierOn())
    {
        return cerr;
    }
    
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
        "edge task loop",
        4096,
        this,
        5,
        NULL);

    log_i("starting lf rx loop");
    return 0;
}

void LowFrequencyImpl::edgeTask(void *p)
{
    LowFrequencyImpl *that = (LowFrequencyImpl *)p;
    LFState::EdgeType edgeType;
    LFState *nextState = NULL;

    for (;;)
    {
        if (xQueueReceive(that->getEdgeQueue(), &edgeType, portMAX_DELAY))
        {
            if (that->fragBuf_.empty())
            {
                that->fragBuf_.push_back(edgeType);
            }
            else
            {
                if (edgeType == LFState::EdgeType::E1)
                {
                    that->fragBuf_.push_back(edgeType);
                    if (that->fragBuf_.size() == 5)
                    {
                        that->getCurrentState()->edgeEvent(that, that->fragBuf_[0], &nextState);
                        if (nextState)
                        {
                            that->setState(nextState);
                        }
                        that->fragBuf_.clear();
                    }
                }
            }

        }
    }
}

void LowFrequencyImpl::edgeTask2(void *p)
{
    LowFrequencyImpl *that = (LowFrequencyImpl *)p;
    uint32_t edge;

    for (;;)
    {
        if (xQueueReceive(that->getEdgeQueue(), &edge, portMAX_DELAY))
        {
            printf("%d\n", edge);
        }
    }
}

void LowFrequencyImpl::setState(LFState *newState)
{
    currentState_->exit(this);
    currentState_ = newState;
    currentState_->enter(this);
}

Tag LowFrequencyImpl::getLastTag()
{
    Tag newTag;
    xSemaphoreTake(lastTagMutex_, portMAX_DELAY);
    newTag = lastTag_;
    xSemaphoreGive(lastTagMutex_);
    return newTag;
}

void LowFrequencyImpl::setLastTag(Tag tag)
{
    xSemaphoreTake(lastTagMutex_, portMAX_DELAY);
    lastTag_ = tag;
    xSemaphoreGive(lastTagMutex_);
}

void LowFrequencyImpl::sendTag(Tag &tag)
{
    if (millis() - last_badge < 1000)
    {
        return;
    }

    if (esp_timer_is_active(timerHandle_))
    {
        if (millis() - badgeDuration_ > 5000)
        {            
            esp_event_post_to(
                eventLoop_,
                LabpassReaderEvent,
                LabpassEvent::LongBadge,
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
            setLastTag(tag);
        }
    }
    else
    {
        esp_timer_start_once(timerHandle_, 400000);
        badgeDuration_ = millis();
        setLastTag(tag);
    }
}

void IRAM_ATTR LowFrequencyImpl::timerExpire(void *p)
{
    LowFrequencyImpl *that = (LowFrequencyImpl *)p;
    last_badge = millis();

    Tag lastTag = that->getLastTag();

    esp_event_post_to(
        that->getEventLoop(),
        LabpassReaderEvent,
        LabpassEvent::ShortBadge,
        (void *)&lastTag,
        sizeof(lastTag),
        portMAX_DELAY);
}

bool LowFrequencyImpl::edgeCallback2(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata, void *user_data)
{
    static uint32_t previousEdgeTimestamp = 0;
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t edgeQueue = (QueueHandle_t)user_data;

    uint32_t edgePeriod = edata->cap_value - previousEdgeTimestamp;
    previousEdgeTimestamp = edata->cap_value;
    if (edgePeriod < 1500)
    {
        return high_task_wakeup;
    }

    xQueueSendFromISR(edgeQueue, &edgePeriod, &high_task_wakeup);
    return high_task_wakeup;
}

bool IRAM_ATTR LowFrequencyImpl::edgeCallback(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata, void *user_data)
{
    // log_i("edge");
    static uint32_t previousEdgeTimestamp = 0;
    BaseType_t high_task_wakeup = pdFALSE;
    LowFrequencyImpl *that = (LowFrequencyImpl *)user_data;

    uint32_t edgePeriod = edata->cap_value - previousEdgeTimestamp;
    previousEdgeTimestamp = edata->cap_value;
    if (edgePeriod < 1500)
    {
        return high_task_wakeup;
    }

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

    xQueueSendFromISR(that->getEdgeQueue(), &edgeType, &high_task_wakeup);
    return high_task_wakeup;
}

esp_err_t LowFrequencyImpl::carrierOn()
{

    // ledc_channel_config_t channelConfig{
    //     .gpio_num = TX,
    //     .speed_mode = LEDC_HIGH_SPEED_MODE,
    //     .channel = LEDC_CHANNEL_0,
    //     .intr_type = LEDC_INTR_DISABLE,
    //     .timer_sel = LEDC_TIMER_0,
    //     .duty = PWM_DUTYCYCLE,
    //     .hpoint = 0xffff,
    // };

    // if (esp_err_t cerr = ledc_channel_config(&channelConfig))
    // {
    //     return cerr;
    // }

    ledcAttachPin(TX, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, PWM_DUTYCYCLE);

    return (esp_err_t) 0;
}
