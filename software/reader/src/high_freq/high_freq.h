#pragma once
#include <Arduino.h>
#include <esp_event.h>
#include <SPI.h>
#include <MFRC522.h>
#include <tag.h>
#include "labpass_event/labpass_event.h"

ESP_EVENT_DECLARE_BASE(LabpassReaderEvent);

class HighFrequency {

public:
    HighFrequency(esp_event_loop_handle_t eventLoop, MFRC522_SPI *mfrc522Spi);
    static void timerExpire(void *);
    void start();
    Tag getLastTag();
    void setLastTag(Tag tag);
    inline esp_event_loop_handle_t getEventLoop() { return eventLoop_; }

private:
    esp_event_loop_handle_t eventLoop_;
    MFRC522_SPI *mfrc522Spi_;
    MFRC522 mfrc522_;
    void processTag();
    static void wakeupTagTask(void *p);
    uint32_t badgeDuration_;
    esp_timer_handle_t timerHandle_;
    Tag lastTag_;
    SemaphoreHandle_t lastTagMutex_;
};