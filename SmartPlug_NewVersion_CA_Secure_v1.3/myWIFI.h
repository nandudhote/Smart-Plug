#ifndef MY_WIFI_H
#define MY_WIFI_H

// #include <WiFi.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

struct subPubTopics {
  String Subscribe;
  String Publish;
  String Global;
};

struct SplitData {
  String indexOneData;
  String indexTwoData;
  String indexThreeData;
};

class myWIFI {
public:
  myWIFI();
  bool wiFiSetup(String ssid, String pass);
  void mqttSetup(const char* MqttSever, int MqttPort);
  bool wiFiLinkCheck();
  void reconnectToMqtt(String pubTopic, String subTopic, String globalTopic);
  static void MQTT_Pull(char* topic, byte* payload, unsigned int length);
  bool CheckMQTTConnection();
  void clientLoop();
  void publishMqttMsg(String pubTopic, String devID, float voltage, float current, double power, bool loadStatus, bool motionStatus);
  void publishMqttMsg_Alert(String pubTopic, String devID, String Alert);
  subPubTopics createSubPubTopics(String devID, String SubTopic, String PubTopic, String globTopic);
  String prepareDevID(byte mac[], String devPref);
  void earasWiFiCredentialsFromEEPROM();
  void writeOneByteInEEPROM(int Add, byte data);
  void storeStringInEEPROM(String data, byte Addr);
  String loadStringFromEEPROM(byte Addr, byte Length);
  void subscribeTopics(String subsTopic);
  void unSubscribe(String subPubTopic);
  SplitData splitStringByColon(const String& data);
};

#endif