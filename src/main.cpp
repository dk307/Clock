#include <ESP8266WiFi.h>

#include "webServer.h"
#include "configManager.h"
#include "WiFiManager.h"
#include "operations.h"
#include "hardware.h"
#include "timentp.h"
#include "logging.h"

void setup(void)
{
#ifdef SERIAL_PRINT	
	Serial.begin(115200);
	delay(1000);
	LOG_INFO(F("Begin"));
#endif
	operations::instance.begin();
	hardware::instance.preBegin();
	config::instance.begin();
	WifiManager::instance.begin();
	WebServer::instance.begin();
	timentp::instance.begin();
	hardware::instance.begin();
	
	LOG_INFO(F("Finish setup. Free heap: ") << ESP.getFreeHeap() / 1024 << F(" KB"));
}

void loop(void)
{
	WifiManager::instance.loop();
	config::instance.loop();
	hardware::instance.loop();
	timentp::instance.loop();
	operations::instance.loop();
	delay(2);
}
