#include <Arduino.h>
#include "driver/ledc.h"
#include "low_freq/low_freq.h"
#include "driver/mcpwm.h"

using namespace team17;
#define RX 16
#define TX 17

#define PWM_CHANNEL 0
#define PWM_FREQUENCY 125000
#define PWM_DUTYCYCLE 0
#define PWM_RESOLUTION 1

etl::bitset<TAG_BIT_SIZE> *lfTag;
QueueHandle_t tagQueue = xQueueCreate(1, sizeof(etl::bitset<TAG_BIT_SIZE> *));
LowFrequency lowFreq(tagQueue, RX);

void lowFreqRXTask(void * parameter);

void setup()
{
  Serial.begin(115200);

  xTaskCreate(
      lowFreqRXTask,
      "Low Frequency RX loop",
      4096,
      NULL,
      10,
      NULL
  );
  log_i("started lf rx loop");
}

void loop() 
{
  xQueueReceive(tagQueue, (void *)&lfTag, portMAX_DELAY);
  Serial.printf("got tag: %s\n", lfTag->to_string().c_str());
  free(lfTag);
}

void lowFreqRXTask(void * parameter) {
  lowFreq.start();
}