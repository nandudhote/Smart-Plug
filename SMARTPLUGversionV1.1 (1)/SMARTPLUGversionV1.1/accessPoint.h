#ifndef ACCESSPOINT_H_
#define ACCESSPOINT_H_

#include "Arduino.h"
#include <ESPAsyncTCP.h>           // Use ESPAsyncTCP for ESP8266
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>

class ACCESSPOINT {
  public:
    ACCESSPOINT();
    void accessPointSetup();
    void updateTheSSIDAndPASSFromMqttIfAvailable();
    void readSsidAndPasswordFromAP(const char* ssid, const char* pass);
    void splitSSID_AND_PASS(String ssipass);
    void stopApServer();
    void stopApWiFi();
};

#endif
