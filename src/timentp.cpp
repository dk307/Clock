#include "timentp.h"
#include "logging.h"
#include "configManager.h"
#include "WiFiManager.h"
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <Timezone.h> 
#include <coredecls.h> 


timentp timentp::instance;

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  // Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   // Eastern Standard Time = UTC - 5 hours
Timezone usET(usEDT, usEST);

// US Central Time Zone (Chicago, Houston)
TimeChangeRule usCDT = {"CDT", Second, Sun, Mar, 2, -300};
TimeChangeRule usCST = {"CST", First, Sun, Nov, 2, -360};
Timezone usCT(usCDT, usCST);

// US Mountain Time Zone (Denver, Salt Lake City)
TimeChangeRule usMDT = {"MDT", Second, Sun, Mar, 2, -360};
TimeChangeRule usMST = {"MST", First, Sun, Nov, 2, -420};
Timezone usMT(usMDT, usMST);

// Arizona is US Mountain Time Zone but does not use DST
Timezone usAZ(usMST);

// US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
Timezone usPT(usPDT, usPST);

void timentp::begin()
{
    WiFi.onStationModeGotIP(std::bind(&timentp::onGotIP, this, std::placeholders::_1));
    WiFi.onStationModeDisconnected(std::bind(&timentp::onWifiDisconnect, this, std::placeholders::_1));
    LOG_INFO(F("Wifi Status:") << WiFi.status());
    forceReconnect = true;

    const auto ftn = [this]
    {
        LOG_DEBUG(F("Config reset, get timentp data again"));     
        forceReconnect = true;
    };

    config::instance.addConfigSaveCallback(ftn); 
    settimeofday_cb(timeIsSet);   
}

uint32_t sntp_update_delay_MS_rfc_not_less_than_15000 ()
{
   return std::max<uint32_t>(15000, config::instance.data.ntpServerRefreshInterval);
}   

void timentp::timeIsSet(bool from_sntp)
{
    if (!timentp::instance.timeSet) 
    {
        LOG_INFO(F("Time is set. From SNTP:") << from_sntp);
        timentp::instance.timeSet = true;
    } 
    else 
    {
        LOG_INFO(F("Time is updated. From SNTP:") << from_sntp);
    } 
}

void timentp::loop()
{
    if (forceReconnect) {
        LOG_INFO(F("Doing NTP Setup"));
        forceReconnect = false;
        ntpServer1 = config::instance.data.ntpServer1;
        ntpServer2 = config::instance.data.ntpServer2;

        configTime(0, 0, ntpServer1.c_str(), ntpServer2.c_str());
    }
}

void timentp::onGotIP(const WiFiEventStationModeGotIP& event)
{
    LOG_INFO(F("Got Wifi IP: ") << event.ip);
    forceReconnect = true;
}

void timentp::onWifiDisconnect(const WiFiEventStationModeDisconnected& event)
{
    LOG_INFO(F("Got Wifi Disconnect"));
    forceReconnect = false;
}

std::optional<timentp::DisplayTime> timentp::getDisplayTime() const
{
    if (timeSet) 
    {
        Timezone*  tz;
        switch (config::instance.data.timezone)
        {
            default:
            case TimeZoneSupported::USEastern: 
            tz = &usET;
            break;
            case TimeZoneSupported::USCentral:
            tz = &usCT;
            break; 
            case TimeZoneSupported::USMountainTime:
            tz = &usMT;
            break; 
            case TimeZoneSupported::USArizona:
            tz = &usAZ;
            break; 
            case TimeZoneSupported::USPacific:
            tz = &usPT;
            break; 
        }

        const auto utc = time(NULL);
        const time_t t = tz->toLocal(utc);
        return DisplayTime{hour(t), minute(t)};
    }
    return std::nullopt;
}


