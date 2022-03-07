#pragma once
#include <Arduino.h>
#include "MFRC522.h"
#include <optional>
#include "tag/tag.h"

namespace team17 {

    class HighFrequency {

    public:
        HighFrequency(QueueHandle_t tagQueue, MFRC522_SPI &mfrc522Spi, uint8_t intPin);
        void start();
        QueueHandle_t getReadQueue();

    private:
        QueueHandle_t tagQueue_;
        QueueHandle_t readQueue_;
        MFRC522_SPI &mfrc522Spi_;
        MFRC522 mfrc522_;
        bool gotRead_;
        uint8_t intPin_;
        bool processTag(Tag * tag);
    };
}