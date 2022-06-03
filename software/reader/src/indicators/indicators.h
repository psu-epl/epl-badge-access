#pragma once
#include <unistd.h>

class Indicators {

public:
    enum Color
    {
        RED,
        YELLOW,
        ORANGE,
        GREEN,
        BLUE
    };

    Indicators();

    void powerLEDOn(Color color);
    void powerLEDOff();

    void statusLEDOn(Color color);
    void statusLEDOff();

    void powerLEDBlink(Color color, useconds_t delay);
    void statusLEDBlink(Color color, useconds_t delay);

private:
};