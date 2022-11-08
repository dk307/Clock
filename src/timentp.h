#pragma once

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "changeCallback.h"

class timentp
{
public:
    typedef std::tuple<uint8_t,uint8_t> DisplayTime;
    void begin();
    void loop();

    std::optional<DisplayTime> getDisplayTime() const;
 
    static timentp instance;
private:   
    bool forceReconnect{false};
    String ntpServer1;
    String ntpServer2;
    bool timeSet{false};

    void onGotIP(const WiFiEventStationModeGotIP& event);
    void onWifiDisconnect(const WiFiEventStationModeDisconnected& event);
    static void timeIsSet(bool from_sntp);
};