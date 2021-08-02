#ifndef NetworkManager_H // include guard
#define NetworkManager_H

#include "myStripLed.h"
//class MyStripLed;

class NetworkManager
{
public:
    NetworkManager();
    NetworkManager(MyStripLed *p_myStripLed);

    void setWifi();
    void setSPIFFS();
    void setServer();
    StaticJsonDocument<1000> requestJsonApi();

private:
    const char *ssid = "clock";
    const char *password = "harrah123456";

    const char *ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 3600;
    const int daylightOffset_sec = 3600; //0;

    AsyncWebServer *server; // (80);
    WiFiManager wm;

    MyStripLed *myStripLed;
};

#endif /* NetworkManager_H */
