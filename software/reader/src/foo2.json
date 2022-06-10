#include "device.h"
#include "device_state_impl.h"
#include <client.h>
#include <low_freq.h>
#include <Arduino.h>

#include <WiFi.h>

String ssid = "7yv89tndqr_2g";
String password = "exoticcarrot227";
String baseURL = "http://192.168.1.63";
String stationID = "72a72777-e7da-4837-a870-2f9afdbcffe8";
String jwtToken = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjIyODQyMDM1MTgsImZ1bGxOYW1lIjoic29sZGVyaW5nLWlyb24iLCJpZCI6IjcyYTcyNzc3LWU3ZGEtNDgzNy1hODcwLTJmOWFmZGJjZmZlOCIsInJvbGUiOiJzdGF0aW9uIn0.J5OE_OtbA2wjvY3bggsnP8iFVux212m5YhAGRu5hiKI";
esp_event_loop_handle_t loop_handle;

Device *device;
LabpassClient *client;

void setup()
{
  esp_event_loop_args_t loop_args = {
      .queue_size = 5,
      .task_name = "labpass_loop_task",
      .task_priority = uxTaskPriorityGet(NULL),
      .task_stack_size = 3072,
      .task_core_id = tskNO_AFFINITY};

  esp_event_loop_create(&loop_args, &loop_handle);

  client = new LabpassClient(baseURL, stationID, jwtToken, loop_handle);
  device = new Device(ssid, password, loop_handle, client);

  Serial.begin(115200);

  device->start();

  delay(10000);

  String * badge = new String("1390094");

  log_i("posting badge to event loop: %s", badge->c_str());

  esp_event_post_to(loop_handle, LabpassFakeReaderEvent, labpassFakeReaderEventType::shortBadge, badge, sizeof(*badge), portMAX_DELAY);

  delay(10000);

  badge = new String("1390094");

  esp_event_post_to(loop_handle, LabpassFakeReaderEvent, labpassFakeReaderEventType::shortBadge, &badge, sizeof(&badge), portMAX_DELAY);
}

void loop()
{
  delay(1000);
}