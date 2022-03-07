#include "high_freq.h"

using namespace team17;
using namespace std;

void isr(void *p);

HighFrequency::HighFrequency(QueueHandle_t tagQueue, MFRC522_SPI &mfrc522Spi, uint8_t intPin) : mfrc522Spi_(mfrc522Spi),
                                                                                               mfrc522_(&mfrc522Spi),
                                                                                               tagQueue_(tagQueue),
                                                                                               readQueue_(xQueueCreate(1, sizeof(bool))),
                                                                                               gotRead_(false),
                                                                                               intPin_(intPin)
{

}

QueueHandle_t HighFrequency::getReadQueue()
{
    return this->readQueue_;
}

void HighFrequency::start()
{
    Tag *tag;

    TaskHandle_t taskHandle = xTaskGetCurrentTaskHandle();

    attachInterruptArg(digitalPinToInterrupt(intPin_), isr, &taskHandle, HIGH);

    for (;;)
    {
        if(ulTaskNotifyTake(pdTRUE, portMAX_DELAY))
        {
            if (processTag(tag))
            {
                xQueueSend(tagQueue_, tag, portMAX_DELAY);
            }
        }
    }
}

bool HighFrequency::processTag(Tag * tag)
{
    if (mfrc522_.PICC_IsNewCardPresent())
    {
        if (mfrc522_.PICC_ReadCardSerial())
        {
            if (mfrc522_.uid.size > 0)
            {
                tag = new Tag(Tag::TagType::HighFrequency, mfrc522_.uid.size);
                for (int i = 0; i < mfrc522_.uid.size; i++)
                {
                    tag->getData()->push_back(mfrc522_.uid.uidByte[i]);
                };
                return true;
            }
            else
            {
                log_e("card uid length is 0");
            }
        }
        else
        {
            log_e("unable to read card serial");
        }
    }
    else
    {
        log_i("no card preent");
    }

    mfrc522_.PICC_HaltA();
    return false;
}

void IRAM_ATTR isr(void *p)
{
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    TaskHandle_t *t = (TaskHandle_t *)p;
    vTaskNotifyGiveFromISR(*t, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}