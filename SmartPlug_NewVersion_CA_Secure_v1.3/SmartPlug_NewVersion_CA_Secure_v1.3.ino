/*
   Name Of Project: Smart Plug Over WiFi
   MCU used: ESP8266 SINGLE CORE
   Functions: Load ON/OFF over MQTT and Power measurement
   Programmer: Nandakishor Dhote
   Author: EVOLUZN INDIA PRIVATE LIMITED
   Firmware Status: 1) Power data reading from CSE IC
                    2) Publishing the data over MQTT on one mints frequency
                    3) reading wifi creadentials from access point
                    4) MQTT secure with CA Certifications 
*/

#include "Config.h"
#include "internalDrivers.h"
#include "myWIFI.h"
#include "CSE7759.h"
#include "accessPoint.h"

ACCESSPOINT AP;
CSE7759 cse7759;
myWIFI wiFI;
internalDrivers iDrivers;
subPubTopics Topic;
PowerData powerData;

void setup() {
  Serial.begin(4800);  // for debugging
  iDrivers.gpioInit();
  iDrivers.readDataFromEEPROM();
  wiFI.wiFiSetup(SSID, PASSWORD);
  apSSID = wiFI.prepareDevID(MAC, apSSID);
  deviceId = wiFI.prepareDevID(MAC, devNamePrefix);
  AP.accessPointSetup();
  wiFI.mqttSetup(ServerMQTT, MqttPort);
  Topic = wiFI.createSubPubTopics(deviceId, subTopic, pubTopic, globalTopic);  // establish the connections with Mqtt
  wiFI.reconnectToMqtt(Topic.Publish, Topic.Subscribe, Topic.Global);
  cse7759.initialize_power_data(&powerData);
}

void loop() {
  wiFI.clientLoop();

  iDrivers.motionDetectionUsingPIR();
  powerData = cse7759.validateTheCSEChipData(cse7759.read_power_data());

  AP.updateTheSSIDAndPASSFromMqttIfAvailable();
  if (enterInAPMode) {
    mqttCounter = 0;
    wiFI.earasWiFiCredentialsFromEEPROM();
    SSID = "";
    PASSWORD = "";
    AP.accessPointSetup();
    enterInAPMode = false;
  }
  /* One Mints Millis loop */
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= (interval * 1000) || responseOn200Request || !isWifiConnected || alertMsg != "") {
    if (wiFI.wiFiLinkCheck()) {
      isWifiConnected = true;
      // digitalWrite(_wifiStatusLED, LOW);
      if (isApStarted) {
        wiFiRetryCounter = 0;
        isApStarted = false;
        AP.stopApServer();
        AP.stopApWiFi();
      }
      if (wiFI.CheckMQTTConnection()) {
        if (alertMsg != "") {
          wiFI.publishMqttMsg_Alert(Topic.Publish, deviceId, alertMsg);
          alertMsg = "";
        }
        if ((currentTime - previousTime >= (interval * 1000)) || responseOn200Request) {
          if (responseOn200Request) {
            wiFI.publishMqttMsg(Topic.Publish, "300", powerData.voltage, powerData.current, powerData.power, relayLoadStaus, autoMotionDetect_Flag);
            responseOn200Request = false;
          } else {
            wiFI.publishMqttMsg(Topic.Publish, deviceId, powerData.voltage, powerData.current, powerData.power, relayLoadStaus, autoMotionDetect_Flag);
            previousTime = currentTime;
          }
        }
      } else {
        if (!wiFI.CheckMQTTConnection()) {
          wiFI.reconnectToMqtt(Topic.Publish, Topic.Subscribe, Topic.Global);
        } else {
          delay(100);
        }
      }
    } else {
      wiFI.wiFiSetup(SSID, PASSWORD);
      wiFiRetryCounter += 1;
      if (wiFiRetryCounter >= 2 && (!isApStarted)) {
        AP.readSsidAndPasswordFromAP(apSSID.c_str(), apPassword.c_str());
        isApStarted = true;
      }
    }
  }
  delay(1000);
}
