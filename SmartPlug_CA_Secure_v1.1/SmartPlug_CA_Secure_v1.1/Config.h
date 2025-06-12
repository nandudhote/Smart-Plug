#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"

extern String SSID;
extern String PASSWORD ;

extern byte MAC[6];
extern const char* ServerMQTT;
extern int MqttPort;

extern String devNamePrefix;
extern String subTopic;  // devName + 6 digits of MAC + /Control
extern String pubTopic;
extern String globalTopic;

extern const byte ssidEEPROMAdd;
extern const byte passEEPROMAdd;

extern const byte ssidLenghtEEPROMAdd;
extern const byte passwordLenghtEEPROMAdd;
extern const byte loadStateEEPROMAddr;
extern const byte espRestartFlagEEPROMAddr;
extern const byte autoMotionFlagEEPROMAddr;


extern const char _relayPin;
extern const char _statusLED;
extern const char _PIRSensorPin;

extern bool ethernetLinkFlag;
extern bool responseOn200Request;
extern unsigned int previousTime;
extern byte ethernetResetCounter;
extern bool relayLoadStaus;

extern float voltagePre;
extern float currentPre;
extern float powerPre;

extern String apSSID;
extern String apPassword;

extern String deviceId;

extern byte ssidLength;
extern byte passwordLength;
extern bool enterInAPMode;
extern bool globallySSIDAndPasswordChange;
extern bool espRestartFlag;
extern byte wiFiRetryCounter;
extern bool isApStarted;

extern int duration;
extern int PIR_THRESH; 

extern bool autoMotionDetect_Flag;
extern unsigned int motionCounter;
extern unsigned int motionConfirmedCounter;

extern bool operationActive;
extern unsigned int sampleCount;

extern float totalVoltage;
extern float totalCurrent;
extern double totalPower;

extern byte mqttCounter;

#endif
