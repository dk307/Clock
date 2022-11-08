#pragma once
#include "changeCallBack.h"

#include <ArduinoJson.h>

enum class TimeZoneSupported
{
    USEastern = 0,
    USCentral = 1,
    USMountainTime = 2,
    USArizona = 3,
    USPacific = 4
};

struct configData
{
    String hostName;
    String webUserName;
    String webPassword;
        
    String ntpServer1;
    String ntpServer2;
    uint32_t ntpServerRefreshInterval;
    TimeZoneSupported timezone;
    
    configData()
    {
        setDefaults();
    }

    void setDefaults()
    {
        const auto defaultUserIDPassword = F("admin");
        hostName.clear();
        webUserName = defaultUserIDPassword;
        webPassword = defaultUserIDPassword;
        ntpServerRefreshInterval = 60 * 1000;
        timezone = TimeZoneSupported::USPacific;
        ntpServer1.clear();
        ntpServer2.clear();
    }
};

class config : public changeCallBack
{
public:
    configData data;
    bool begin();
    void save();
    void reset();
    void loop();

    static void erase();
    static config instance;

    String getAllConfigAsJson();

    // does not restore to memory, needs reboot
    bool restoreAllConfigAsJson(const std::vector<uint8_t> &json, const String &md5);

private:
    config() {}
    static String readFile(const String &fileName);

    template <class... T>
    static String md5Hash(T&&... data);

    template <class... T>
    static size_t writeToFile(const String &fileName, T&&... contents);

    template<class T>
    bool deserializeToJson(const T &data, DynamicJsonDocument &jsonDocument);

    bool requestSave{false};
};
 