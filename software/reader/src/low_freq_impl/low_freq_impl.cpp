#include "low_freq_impl/low_freq_impl.h"
#include <driver/timer.h>

using namespace std;
ESP_EVENT_DEFINE_BASE(LabpassLFReaderEvent);

LowFrequencyImpl::LowFrequencyImpl(esp_event_loop_handle_t eventLoop, uint8_t din) : din_(din),
                                                                                     edgeQueue_(xQueueCreate(4096, sizeof(uint32_t))),
                                                                                     eventLoop_(eventLoop),
                                                                                     badgeDuration_(0),
                                                                                     timerActive_(false),
                                                                                     fragBuf_(vector<LFState::EdgeType>()),
                                                                                     currentState_(&LFStateSync::getInstance())

{
    pinMode(TX, OUTPUT);
    pinMode(din_, INPUT);
}

esp_err_t LowFrequencyImpl::start()
{
    if (esp_err_t cerr = LowFrequencyImpl::carrierOn())
    {
        return cerr;
    }
    
    mcpwm_pin_config_t pinConfig = {
        .mcpwm_cap0_in_num = din_};

    mcpwm_capture_config_t capConfig = {
        .cap_edge = MCPWM_POS_EDGE,
        .cap_prescale = 1,
        .capture_cb = LowFrequencyImpl::edgeCallback,
        .user_data = this
    };

    mcpwm_capture_enable_channel(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, &capConfig);
    mcpwm_set_pin(MCPWM_UNIT_0, &pinConfig);

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
                        // switch (that->fragBuf_[0])
                        // {
                        // case LFState::EdgeType::E0001:
                        //     log_i("E0001");
                        //     break;
                        // case LFState::EdgeType::E001:
                        //     log_i("E001");
                        //     break;
                        // case LFState::EdgeType::E01:
                        //     log_i("E01");
                        //     break;
                        // case LFState::EdgeType::E1:
                        //     log_i("E1");
                        //     break;
                        // default:
                        //     printf("default\n");
                        // }
                        nextState = that->getCurrentState()->edgeEvent(that, that->fragBuf_[0]);
                        if (nextState != NULL)
                        {
                            that->setState(*nextState);
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

void LowFrequencyImpl::setState(LFState &newState)
{
    currentState_->exit(this);
    currentState_ = &newState;
    currentState_->enter(this);
}

void LowFrequencyImpl::sendTag(Tag *tag)
{
    log_i("send tag");
    if (timerActive_)
    {
        log_i("timer active");
        if (millis() - badgeDuration_ > 5000)
        {
            esp_event_post_to(
                eventLoop_,
                LabpassLFReaderEvent,
                labpassLFReaderEventType::longBadge,
                (void *)&tag,
                sizeof(*tag),
                portMAX_DELAY);

            badgeDuration_ = 0;
            timerActive_ = false;
        }
        else
        {
            ESP_ERROR_CHECK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0));

            lastTag_ = tag;
        }
    }
    else
    {
        log_i("timer inactive");
        lastTag_ = tag;

        // init duration
        badgeDuration_ = millis();

        // start timer
        timer_config_t * timerConfig = new timer_config_t{
            .alarm_en = TIMER_ALARM_EN,
            .counter_en = TIMER_START,
            .intr_type = TIMER_INTR_LEVEL,
            .counter_dir = TIMER_COUNT_UP,
            .auto_reload = TIMER_AUTORELOAD_DIS,
            .divider = 80, // 1us per tick
        };

        ESP_ERROR_CHECK(timer_init(TIMER_GROUP_0, TIMER_0, timerConfig));
        timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);
        ESP_ERROR_CHECK(timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 500000));
        timer_enable_intr(TIMER_GROUP_0, TIMER_0);
        ESP_ERROR_CHECK(timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, LowFrequencyImpl::timerExpire, this, 0));
        timer_start(TIMER_GROUP_0, TIMER_0);
    }
        timerActive_ = true;
}

bool LowFrequencyImpl::timerExpire(void * p)
{
    log_i("timer alarm");
    LowFrequencyImpl *that = (LowFrequencyImpl *)p;

    esp_event_post_to(
        that->getEventLoop(),
        LabpassLFReaderEvent,
        labpassLFReaderEventType::shortBadge,
        (void *) that->getLastTag(),
        sizeof(that->getLastTag()),
        portMAX_DELAY);

    that->badgeDuration_ = 0;
    that->timerActive_ = false;
    that->resetLastTag();
    timer_deinit(TIMER_GROUP_0, TIMER_0);

    return false;
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

bool LowFrequencyImpl::edgeCallback(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata, void *user_data)
{
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
    ledc_timer_config_t timerConfig{
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_1_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 125000};

    if (esp_err_t terr = ledc_timer_config(&timerConfig))
    {
        return terr;
    }

    ledc_channel_config_t channelConfig{
        .gpio_num = TX,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = PWM_DUTYCYCLE,
        .hpoint = 0xffff,
    };

    if (esp_err_t cerr = ledc_channel_config(&channelConfig))
    {
        return cerr;
    }

    ledcAttachPin(TX, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, PWM_DUTYCYCLE);

    return (esp_err_t) 0;
}
