/*
   Name Of Project: Smart Plug Over WiFi
   MCU used: ESP32 SINGLE CORE
   Functions: Load ON/OFF over MQTT and Power measurement
   Programmer: Nilesh Patle
   Author: EVOLUZN INDIA PRIVATE LIMITED
   Firmware Status: 1) Power data reading from CSE IC
                    2) Publishing the data over MQTT on one mints frequency
                    3) reading wifi creadentials from access point
*/

// Schedule updated:
//schedule:2024-11-08 15:35:00/2024-11-08 15:40:00
// Start: 2024-11-08T14:55:00
// End: 2024-11-08T15:00:00
//autoMotionDetection:1 For on the automotion
//autoMotionDetection:0 for off the automotion

#include "Config.h"
#include "internalDrivers.h"
#include "wiFi.h"
#include "CSE7759.h"
#include "accessPoint.h"
#include <Wire.h>

ACCESSPOINT AP;
CSE7759 cse7759;
WIFI wiFI;
internalDrivers iDrivers;
subPubTopics Topic;
PowerData powerData;

void setup() { 
  Serial.begin(4800);  // for debugging
  iDrivers.gpioInit();
  iDrivers.readDataFromEEPROM();
  // wiFI.wiFiSetup(SSID, PASSWORD);
  apSSID = wiFI.prepareDevID(MAC, apSSID);
  deviceId = wiFI.prepareDevID(MAC, devNamePrefix);
  AP.accessPointSetup();
  wiFI.mqttSetup(ServerMQTT, MqttPort);
  cse7759.initialize_power_data(&powerData);
  Topic = wiFI.createSubPubTopics(deviceId, subTopic, pubTopic, globalTopic);  // establish the connections with Mqtt
  wiFI.reconnectToMqtt(Topic.Publish,Topic.Subscribe, Topic.Global);
  Serial.println(Topic.Subscribe);
}
void loop() {
  wiFI.clientLoop();
  iDrivers.motionDetectionUsingPIR();
  powerData = cse7759.validateTheCSEChipData(cse7759.read_power_data(), relayLoadStaus);

  powerData.totalVoltge += powerData.voltage;
  powerData.totalCurrent += powerData.current;
  powerData.totalPower += powerData.power;
  sampleCount += 1;
 
  AP.updateTheSSIDAndPASSFromMqttIfAvailable();
  if (enterInAPMode) {
    iDrivers.earasWiFiCredentialsFromEEPROM();
    SSID = "";
    PASSWORD = "";
    AP.accessPointSetup();
    enterInAPMode = false;
  }
  /* One Mints Millis loop */

  unsigned long currentTime = millis();
  if (currentTime - previousTime >= 60000 || responseOn200Request) {
    if (wiFI.wiFiLinkCheck()) {
      if (isApStarted) {
        wiFiRetryCounter = 0;
        isApStarted = false;
        AP.stopApServer();
        AP.stopApWiFi();
      }
      if (wiFI.CheckMQTTConnection()) {   
        powerData.totalVoltge = powerData.totalVoltge / sampleCount;
        powerData.totalCurrent = powerData.totalCurrent / sampleCount;
        powerData.totalPower = powerData.totalPower / sampleCount;
        if (responseOn200Request) {
          wiFI.publishMqttMsg(Topic.Publish, "300", powerData.totalVoltge, powerData.totalCurrent, powerData.totalPower, relayLoadStaus, autoMotionDetect_Flag);
          responseOn200Request = false;
        } else {
          wiFI.publishMqttMsg(Topic.Publish, deviceId, powerData.totalVoltge, powerData.totalCurrent, powerData.totalPower, relayLoadStaus, autoMotionDetect_Flag);
          previousTime = currentTime;
        }
        powerData.voltage = powerData.current = powerData.power = 0.0;
        sampleCount = 0;
      } else {
        if (!wiFI.CheckMQTTConnection()) {
          wiFI.reconnectToMqtt(Topic.Publish,Topic.Subscribe, Topic.Global);
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
