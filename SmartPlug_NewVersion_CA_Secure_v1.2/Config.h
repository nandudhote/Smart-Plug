#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"

extern String SSID;
extern String PASSWORD;

extern byte MAC[6];
extern const char* ServerMQTT;
extern int MqttPort;
extern const char* mqttUserName;
extern const char* mqttUserPassword;

extern String devNamePrefix;
extern String subTopic;  // devName + 6 digits of MAC + /Control
extern String pubTopic;
extern String globalTopic;
extern String apSSID;
extern String apPassword;
extern String deviceId;

extern const byte ssidEEPROMAdd;
extern const byte passEEPROMAdd;
extern const byte ssidLenghtEEPROMAdd;
extern const byte passwordLenghtEEPROMAdd;
extern const byte publishIntervalEEPROMAdd;
extern const byte loadStateEEPROMAddr;
extern const byte autoMotionFlagEEPROMAddr;
extern const byte motionCounterMsgEEPROM;
extern const byte pirThresholdIntEEPROMAdd;
extern const byte pirThresholdFloatEEPROMAdd;
extern const byte globalTopicLenghtEEPROMAdd;
extern const byte globalTopicEEPROMAdd;

extern const char _wifiStatusLED;
extern const char _relayPin;
extern const char _PIRSensorPin;

extern bool responseOn200Request;
extern bool relayLoadStaus;
extern bool ledState;
extern bool enterInAPMode;
extern bool globallySSIDAndPasswordChange;
extern bool espRestartFlag;
extern bool isApStarted;
extern bool isWifiConnected;

extern unsigned int previousTime;

extern byte ssidLength;
extern byte passwordLength;
extern byte wiFiRetryCounter;
extern byte mqttCounter;
extern byte responseLength;
extern byte interval;
extern byte globalTopicLength;

extern String publishData;
extern String alertMsg;

extern float voltagePre;
extern float currentPre;
extern float powerPre;

extern int duration;
extern bool autoMotionDetect_Flag;
extern unsigned int motionCounter;
extern unsigned int motionConfirmedCounter;
extern int PIR_THRESH;

#endif