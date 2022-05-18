#include "device.h"
#include "device_state_impl.h"

#include <WiFi.h>

const char *ssid = "7yv89tndqr_2g";
const char *password = "exoticcarrot227";

Device device(ssid, password);

void setup()
{
  Serial.begin(115200);
  device.init();
}

void loop()
{
  delay(1000);
}