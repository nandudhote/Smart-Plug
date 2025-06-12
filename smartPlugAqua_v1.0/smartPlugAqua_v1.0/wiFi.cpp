#include "wiFi.h"
#include "Config.h"
#include "String.h"
#include "internalDrivers.h"
#include "accessPoint.h"

ACCESSPOINT ap;
WIFI iWIFI;
internalDrivers iiDrivers;
//EthernetServer server(80);
WiFiClient WLS_Client;
PubSubClient client(WLS_Client);

WIFI::WIFI() {
}

bool WIFI::wiFiSetup(String ssid, String pass) {
  delay(10);
  byte wiFiCounter = 0;
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(ssid.c_str(), pass.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(_statusLED, HIGH);
    delay(250);
    digitalWrite(_statusLED, LOW);
    delay(250);
    Serial.print(".");
    wiFiCounter++;
    if (wiFiCounter >= 15) {
      return false;
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  digitalWrite(_statusLED, LOW);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  ap.stopApServer();
  ap.stopApWiFi();
  return true;
}

void WIFI::mqttSetup(const char* MqttSever, int MqttPort) {
  client.setServer(MqttSever, MqttPort);
  client.setCallback(MQTT_Pull);
}

bool WIFI::wiFiLinkCheck() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  return true;
}

void WIFI::reconnectToMqtt(String pubTopic, String subTopic, String globalTopic) {
  String willMessage = "{" + deviceId + ":offline}";  // Last Will and Testament message

  if (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    //    if (client.connect(prepareDevID(MAC, devNamePrefix).c_str(), mqttUserName, mqttUserPassword)) {
    // if (client.connect(prepareDevID(MAC, devNamePrefix).c_str()))
    if (client.connect(deviceId.c_str(), pubTopic.c_str(), 1, true, willMessage.c_str())) {
      //Serial.println("connected");
      client.subscribe(subTopic.c_str());
      client.subscribe(globalTopic.c_str());

      String onlineMessage = "{" + deviceId + ":online}";
      client.publish(pubTopic.c_str(), onlineMessage.c_str(), true);
    } else {
      mqttCounter++;
      if (mqttCounter >= 15) {
        enterInAPMode = true;
      }
      delay(100);
    }
  }
}

void WIFI::MQTT_Pull(char* topic, byte* payload, unsigned int length) {
  String MqttRecdMsg = "";
  for (int i = 0; i < length; i++) {
    MqttRecdMsg += (char)payload[i];
  }
  Serial.println(MqttRecdMsg);
  Serial.println();

  SplitData mqttDecodedMsg = iiDrivers.splitStringByColon(MqttRecdMsg);

  if (MqttRecdMsg == "200") {
    responseOn200Request = true;
  } else if (MqttRecdMsg == "0") {
    iiDrivers.relayControl(OFF);
  } else if (MqttRecdMsg == "100") {
    iiDrivers.relayControl(ON);
  } else if (strstr(MqttRecdMsg.c_str(), "wiFiCredentials") != NULL) {
    int delimiterIndex = MqttRecdMsg.indexOf(':');
    if (delimiterIndex >= 0) {
      String thresData = MqttRecdMsg.substring(delimiterIndex + 1);  // Extract the value after ":" autoBrightness:1:500
      int delimiterIndex1 = thresData.indexOf(':');
      int delimiterIndex2 = thresData.indexOf(':', delimiterIndex1 + 1);
      SSID = thresData.substring(0, delimiterIndex1);
      PASSWORD = thresData.substring(delimiterIndex1 + 1, delimiterIndex2);
      Serial.print("SSID : ");
      Serial.println(SSID);
      Serial.print("PASSWORD : ");
      Serial.println(PASSWORD);
      globallySSIDAndPasswordChange = true;
    }
  } else if (MqttRecdMsg == "earasWiFiCredentialsFromEEPROM") {
    enterInAPMode = true;
  } else if (mqttDecodedMsg.indexOneData == "autoMotionDetection") {
    // String autoMotionStatus = mqttDecodedMsg.indexTwoData;
    autoMotionDetect_Flag = mqttDecodedMsg.indexTwoData.toInt();
    iiDrivers.writeOneByteInEEPROM(autoMotionFlagEEPROMAddr, autoMotionDetect_Flag);
  } else {
  }
}

bool WIFI::CheckMQTTConnection() {
  if (client.connected()) {
    return true;
  }
  return false;
}

void WIFI::clientLoop() {
  client.loop();
}

void WIFI::publishMqttMsg(String pubTopic, String devID, float voltage, float current, float power, bool loadStatus, bool autoMotionFlag) {
  String Final;
  Final.concat("{device_id:" + devID + ":");
  Final.concat(voltage);
  Final.concat(":");
  Final.concat(current);
  Final.concat(":");
  Final.concat(power);
  Final.concat(":");
  Final.concat(loadStatus);
  Final.concat(":");
  Final.concat(autoMotionFlag);
  Final.concat("}");
  client.publish(pubTopic.c_str(), Final.c_str());
}

void WIFI::publishSingleMqttMsg(String pubTopic, String Msg) {
  client.publish(pubTopic.c_str(), Msg.c_str());
}

subPubTopics WIFI::createSubPubTopics(String devID, String SubTopic, String PubTopic, String globTopic) {
  subPubTopics Topics;
  Topics.Subscribe = devID + SubTopic;
  Topics.Publish = devID + PubTopic;
  Topics.Global = globalTopic + SubTopic;
  return Topics;
}

String WIFI::prepareDevID(byte mac[], String devPref) {
  char devID[30];
  snprintf(devID, sizeof(devID), "%s%02X%02X%02X", devPref.c_str(), mac[3], mac[4], mac[5]);
  return String(devID);
}
