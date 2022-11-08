#pragma once

#include <Wire.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <BH1750.h>
#include "changeCallback.h"
#include "timentp.h"

class hardware
{
public:
    void preBegin();
    void begin();
    void loop();

    float getLux() const
    {
        return lastLux;
    }

    static hardware instance;
    changeCallBack temperatureChangeCallback;
private:
    const int SDAWire = 4;
    const int SCLWire = 5;
    BH1750 lightMeter;
    float lastLux = NAN;

    MD_Parola matrix{ MD_MAX72XX::FC16_HW, 13, 14, 12, 4 };
    uint8_t currentIntensity = 1;
    String currentDisplayText;
    bool currentScroll{true};

    bool refreshDisplay{false};

    // Time
    uint64_t lastDisplayUpdate{0};
    timentp::DisplayTime lastDisplayTime{};

    void updateDisplay();
    void displayLine(const String& line, bool scroll);
    void displayLines(const String& line1, const String& line2);   
    static uint8_t luxToIntensity(uint32_t intensity);
};