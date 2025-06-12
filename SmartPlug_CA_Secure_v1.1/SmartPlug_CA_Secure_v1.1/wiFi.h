#ifndef WIFI_H_
#define WIFI_H_

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

typedef struct {
  String Subscribe;
  String Publish;
  String Global;
} subPubTopics;

class WIFI {
public:
  WIFI();
  bool wiFiSetup(String ssid, String pass);
  bool wiFiLinkCheck();
  void mqttSetup(const char* MqttSever, int MqttPort);
  void reconnectToMqtt(String subTopic, String globalTopic);
  static void MQTT_Pull(char* topic, byte* payload, unsigned int length);
  //  void decodeAndUpdateThresholdFromMqtt(String mqttMsg);
  bool CheckMQTTConnection();
  void clientLoop();
  void publishMqttMsg(String pubTopic, String devID, float voltage, float current, double power, bool loadStatus, bool autoMotionFlag);
  void publishSingleMqttMsg(String pubTopic, String Msg);
  subPubTopics createSubPubTopics(String devID, String SubTopic, String PubTopic, String globTopic);
  void subscribeTopics(String subsTopic);
  String prepareDevID(byte mac[], String devPref);
};

#endif
