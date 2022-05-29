#include <Arduino.h>
#include "low_freq_impl/low_freq_impl.h"
#include <config.h>

LowFrequencyImpl *lf;
esp_event_loop_handle_t loopHandle;

void lfEvent(void *event_handler_arg, esp_event_base_t event_base, int event_id, void *event_data);

void setup()
{
    Serial.begin(115200);
    esp_event_loop_args_t * loop_args = new esp_event_loop_args_t{
        .queue_size = 5,
        .task_name = "labpass_loop_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 3072,
        .task_core_id = tskNO_AFFINITY};

    esp_event_loop_create(loop_args, &loopHandle);

    esp_event_handler_register_with(
        loopHandle, 
        LabpassLFReaderEvent, 
        ESP_EVENT_ANY_ID, 
        lfEvent, 
        NULL);

    lf = new LowFrequencyImpl(loopHandle, LOW_FREQUENCY_DATA_IN);

    lf->start();
}

void loop()
{
    delay(5000);
}

void lfEvent(void *event_handler_arg, esp_event_base_t event_base, int event_id, void *event_data)
{
    Tag *tag = (Tag *)event_data;
    switch (event_id)
    {
    case LowFrequencyImpl::labpassLFReaderEventType::longBadge:
        Serial.printf("LF tag, long badge, facility: %d, id: %d\n", tag->lfTag.facility, tag->lfTag.id);
        break;
    case LowFrequencyImpl::labpassLFReaderEventType::shortBadge:
        Serial.printf("LF tag short badge, facility: %d, id: %d\n", tag->lfTag.facility, tag->lfTag.id);    
    }
}
