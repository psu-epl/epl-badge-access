#pragma once
#include <SPI.h>
#include <Arduino.h>
#include <MFRC522.h>
#include <optional>
#include <tag.h>

namespace team17 {

    class HighFrequency {

    public:
        HighFrequency(QueueHandle_t tagQueue, MFRC522_SPI &mfrc522Spi);
        void start();

    private:
        QueueHandle_t tagQueue_;
        MFRC522_SPI &mfrc522Spi_;
        MFRC522 mfrc522_;
        Tag * processTag();
        static void wakeupTagTask(void *p);
    };
}