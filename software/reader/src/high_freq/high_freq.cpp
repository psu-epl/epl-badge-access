#include "high_freq.h"
#include <cstring>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

using namespace std;

static volatile bool timer_active = false;
static volatile uint64_t last_badge = 0;

#define WAKEUP_POLL_INTERVAL_MS 1
#define ANTENNA_GAIN 0x02 << 4 // 23 dB
                               // http://www.nxp.com/documents/data_sheet/MFRC522.pdf

HighFrequency::HighFrequency(esp_event_loop_handle_t eventLoop, MFRC522_SPI *mfrc522Spi) : mfrc522Spi_(mfrc522Spi),
                                                                                           mfrc522_(mfrc522Spi),
                                                                                           eventLoop_(eventLoop),
                                                                                           lastTagMutex_(xSemaphoreCreateMutex()),
                                                                                           badgeDuration_(0),
                                                                                           lastTag_(Tag())

{
    esp_timer_create_args_t createArgs{
        .callback = HighFrequency::timerExpire,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "hf_badge_duration_timer",
        .skip_unhandled_events = true};

    ESP_ERROR_CHECK(esp_timer_create(&createArgs, &timerHandle_));
}

void HighFrequency::start()
{
    SPI.begin();         // Init SPI bus
    mfrc522_.PCD_Init(); // Init MFRC522 card
    mfrc522_.PCD_SetAntennaGain(ANTENNA_GAIN);

    xTaskCreate(
        wakeupTagTask,
        "HF Tag Wakeup",
        4096,
        this,
        5,
        NULL);

    byte gain = mfrc522_.PCD_GetAntennaGain();
    log_i("starting hf rx loop");
}

void HighFrequency::wakeupTagTask(void *p)
{
    byte atqa_answer[2];
    byte atqa_size;
    HighFrequency *that = ((HighFrequency *)p);

    for (;;)
    {
        memset(atqa_answer, 0, sizeof(atqa_answer));
        byte atqa_size = sizeof(atqa_answer);
        MFRC522::StatusCode statusCode = that->mfrc522_.PICC_WakeupA(atqa_answer, &atqa_size);

        switch (statusCode)
        {
        case MFRC522::STATUS_OK:
            that->processTag();
            break;
        case MFRC522::STATUS_TIMEOUT:
            break;
        }
        that->mfrc522_.PICC_HaltA();

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    vTaskDelete(NULL);
}

void HighFrequency::processTag()
{
    Tag tag;
    if (mfrc522_.PICC_ReadCardSerial() && mfrc522_.uid.size > 0)
    {
        tag = Tag(mfrc522_.uid.uidByte, mfrc522_.uid.size);
    }
    else
    {
        return;
    }

    if (esp_timer_is_active(timerHandle_))
    {
        if (millis() - badgeDuration_ > 5000)
        {
            ESP_ERROR_CHECK(esp_event_post_to(
                eventLoop_,
                LabpassReaderEvent,
                LabpassEvent::LongHFBadge,
                (void *)&tag,
                sizeof(tag),
                portMAX_DELAY));

            esp_timer_stop(timerHandle_);
            last_badge = millis();
        }
        else
        {
            esp_timer_stop(timerHandle_);
            esp_timer_start_once(timerHandle_, 400000);
            setLastTag(&tag);
        }
    }
    else
    {
        esp_timer_start_once(timerHandle_, 400000);
        badgeDuration_ = millis();
        setLastTag(&tag);
    }
}

void HighFrequency::timerExpire(void *p)
{
    HighFrequency *that = (HighFrequency *)p;
    last_badge = millis();
    Tag lastTag;
    that->getLastTag(&lastTag);

    ESP_ERROR_CHECK(esp_event_post_to(
        that->getEventLoop(),
        LabpassReaderEvent,
        LabpassEvent::ShortHFBadge,
        (void *)&lastTag,
        sizeof(lastTag),
        portMAX_DELAY));
}

void HighFrequency::getLastTag(Tag *tag)
{
    xSemaphoreTake(lastTagMutex_, portMAX_DELAY);
    *tag = lastTag_;
    xSemaphoreGive(lastTagMutex_);
}

void HighFrequency::setLastTag(Tag *tag)
{
    xSemaphoreTake(lastTagMutex_, portMAX_DELAY);
    lastTag_ = *tag;
    xSemaphoreGive(lastTagMutex_);
}
