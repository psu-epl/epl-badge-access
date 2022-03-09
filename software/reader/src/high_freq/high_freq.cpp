#include "high_freq.h"

using namespace team17;
using namespace std;

#define WAKEUP_POLL_INTERVAL_MS 1
#define ANTENNA_GAIN 0x02 << 4 // 23 dB
                               // http://www.nxp.com/documents/data_sheet/MFRC522.pdf

HighFrequency::HighFrequency(QueueHandle_t tagQueue, MFRC522_SPI &mfrc522Spi) : mfrc522Spi_(mfrc522Spi),
                                                                                mfrc522_(&mfrc522Spi),
                                                                                tagQueue_(tagQueue)
{
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
    log_i("antenna gain byte: %d", gain);
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
        default:
            const __FlashStringHelper *fs = that->mfrc522_.GetStatusCodeName(statusCode);
            log_w("card wakeup returned: `%s`", fs);
        }
        that->mfrc522_.PICC_HaltA();
        vTaskDelay(portTICK_PERIOD_MS * WAKEUP_POLL_INTERVAL_MS);
    }

    vTaskDelete(NULL);
}

Tag * HighFrequency::processTag()
{
    if (mfrc522_.PICC_ReadCardSerial() && mfrc522_.uid.size > 0)
    {
        Tag *tag = new Tag(Tag::TagType::HighFrequency, mfrc522_.uid.size);
        for (int i = 0; i < mfrc522_.uid.size; i++)
        {
            tag->addByte(mfrc522_.uid.uidByte[i]);
        };
        xQueueSend(tagQueue_, &tag, portMAX_DELAY);
    }

    return NULL;
}
