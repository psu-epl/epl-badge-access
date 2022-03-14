#include <Arduino.h>
#include "driver/ledc.h"
#include "low_freq/low_freq.h"
#include "high_freq/high_freq.h"
#include "tag/tag.h"
#include "driver/mcpwm.h"
#include "MFRC522.h"

using namespace team17;

#define HF_NSS 21
#define HF_RST 27
#define HF_INT 33

QueueHandle_t tagQueue = xQueueCreate(1, sizeof(Tag *));
MFRC522_SPI mfrc522Spi = MFRC522_SPI(HF_NSS, HF_RST);

LowFrequency lowFreq(tagQueue, RX);
HighFrequency highFreq(tagQueue, mfrc522Spi);

void lowFreqRXTask(void *parameter);

Tag *tag;

void setup()
{
  Serial.begin(115200);

  error_t startErr = lowFreq.start();
  if (startErr)
  {
    printf("start error: %d", startErr);
  }

  highFreq.start();
}

void loop() 
{
    xQueueReceive(tagQueue, &tag, portMAX_DELAY);

    printf("Got %s tag: ", tag->getType().c_str());
    for (auto b : *(tag->getData()))
    {
      printf("%02X", b);
    }
    printf("\n");
}
