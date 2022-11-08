#include "hardware.h"
#include "configManager.h"
#include "WiFiManager.h"
#include "logging.h"

#include <math.h>

hardware hardware::instance;


void hardware::preBegin()
{
    Wire.begin(SDAWire, SCLWire);
 
    matrix.begin();
    matrix.setIntensity(currentIntensity);
    matrix.displayClear();
    matrix.setCharSpacing(2);

    displayLine(F("Start"), false);

    if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
        LOG_ERROR(F("Failed to Init Light Sensor"));
    }

}

void hardware::begin()
{
    const auto ftn = [this] 
    {
        LOG_DEBUG(F("Display refresh needed"));
        refreshDisplay = true;
    };

    config::instance.addConfigSaveCallback(ftn);
    WifiManager::instance.addConfigSaveCallback(ftn);

    updateDisplay();
}

void hardware::loop()
{
    updateDisplay();
    if (currentScroll)
    {
        if (matrix.displayAnimate()) 
        {
            matrix.displayReset();
        }
    }
    
    if (lightMeter.measurementReady(true)) {
        float lux = lightMeter.readLightLevel();

        if (lux != lastLux) {
            lastLux = lux;
            LOG_DEBUG(F("New Light Level: ") << lux);

            const uint8_t intensity = luxToIntensity(round(lux));

            if (intensity != currentIntensity) {
                LOG_INFO(F("New Intensity Level: ") << intensity);
                matrix.setIntensity(intensity);
                currentIntensity = intensity;
            }
        }      
    }
}

void hardware::displayLine(const String& line, bool scroll)
{
    if ((currentDisplayText != line) || (scroll != currentScroll))
    {      
        currentDisplayText = line;
        currentScroll = scroll;
        if (scroll)
        {
            matrix.displayText(currentDisplayText.c_str(), PA_LEFT, 200, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        }
        else 
        {
            matrix.setTextAlignment(PA_CENTER);
            matrix.displayText(currentDisplayText.c_str(), PA_CENTER, 75, 0, PA_PRINT, PA_DISSOLVE); 
            while (matrix.displayAnimate());
           
        }
        LOG_INFO(line);
    }
} 

void hardware::displayLines(const String& line1, const String& line2)
{
    const String data =  line1 + " " + line2;
    displayLine(data, true);
} 

void hardware::updateDisplay()
{
    const auto now = millis();
    if ((now - lastDisplayUpdate >= 500) || refreshDisplay)
    {
        if (WifiManager::instance.isCaptivePortal())
        {
            displayLines(F("AP Mode"),  WifiManager::instance.getAPForCaptiveMode());
        }
        else
        {
            const auto currentTime = timentp::instance.getDisplayTime();

            if (currentTime.has_value()) 
            {
                if (*currentTime != lastDisplayTime) 
                {
                    char str[12];
                    auto hour = std::get<0>(*currentTime);
                    hour = hour > 12 ? hour - 12 : hour;
                    hour = hour == 0 ? 12 : hour;
                    sprintf(str, "%0d:%02d", hour, std::get<1>(*currentTime));
                    displayLine(str, false);
                    LOG_INFO(F("New Display:") << str);  
                    lastDisplayTime = *currentTime;
                }
            }
            else
            {
                displayLines(F("Listening at"), WifiManager::LocalIP().toString());
            }
        }
        lastDisplayUpdate = now;
        refreshDisplay = false;
    }
}

uint8_t hardware::luxToIntensity(uint32_t intensity)
{
    // https://learn.microsoft.com/en-us/windows/win32/sensorsapi/understanding-and-interpreting-lux-values
    if (intensity <= 10) {
        return 0;
    } else if (intensity <= 10) {
        return 1;
    } else if (intensity <= 30) {
        return 2;
    } else if (intensity <= 50) {
        return 3;
    } else if (intensity <= 100) {
        return 4;
    } else if (intensity <= 200) {
        return 5;
    } else if (intensity <= 300) {
        return 6;
    } else if (intensity <= 400) {
        return 7;
    } else if (intensity <= 500) {
        return 8;
    } else if (intensity <= 600) {
        return 9;
    } else if (intensity <= 700) {
        return 10;
    } else if (intensity <= 800) {
        return 11;
    } else if (intensity <= 900) {
        return 12;
    } else if (intensity <= 1000) {
        return 13;
    } else if (intensity <= 1500) {
        return 14;
    } else {
        return 15;
    }



}
