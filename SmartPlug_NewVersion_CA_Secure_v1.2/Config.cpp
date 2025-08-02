#include <sys/_intsup.h>
#include "Config.h"

String SSID = "";
String PASSWORD = "";

byte MAC[6] = { 0x8C, 0x4C, 0xAD, 0xF0, 0xBF, 0x4C };
const char* ServerMQTT = "mqtt.evoluzn.in";
// const char* ServerMQTT = "evoluzn.org";
//const char* ServerMQTT = "a6kvi1np2cmrt-ats.iot.ap-south-1.amazonaws.com";
//int MqttPort = 8883;
int MqttPort = 18889;
const char* mqttUserName = "evzin_led";
const char* mqttUserPassword = "63I9YhMaXpa49Eb";

String devNamePrefix = "plug";
String subTopic = "/control";  // devName + 6 digits of MAC + /Control
String pubTopic = "/status";   // // devName + 6 digits of MAC + /Status
String globalTopic = "smartPlugGlobal";
String apSSID = "SmartPlugAp";
String apPassword = "123456789";
String deviceId = "";

const byte ssidEEPROMAdd = 00;
const byte passEEPROMAdd = 21;
const byte ssidLenghtEEPROMAdd = 41;
const byte passwordLenghtEEPROMAdd = 42;
const byte publishIntervalEEPROMAdd = 43;
const byte loadStateEEPROMAddr = 44;
const byte autoMotionFlagEEPROMAddr = 45;
const byte motionCounterMsgEEPROM = 46;
const byte pirThresholdIntEEPROMAdd = 47;
const byte pirThresholdFloatEEPROMAdd = 48;
const byte globalTopicLenghtEEPROMAdd = 49;
const byte globalTopicEEPROMAdd = 50;  // Global topic max length should of 22 bits

const char _wifiStatusLED = D4;
const char _relayPin = 13;
const char _PIRSensorPin = 14;

bool responseOn200Request = false;
bool relayLoadStaus = false;
bool ledState = false;
bool enterInAPMode = false;
bool globallySSIDAndPasswordChange = false;
bool espRestartFlag = false;
bool isApStarted = false;
bool isWifiConnected = false;

unsigned int previousTime = 0;

byte ssidLength = 0;
byte passwordLength = 0;
byte wiFiRetryCounter = 0;
byte mqttCounter = 0;
byte responseLength = 2;
byte interval = 60;
byte globalTopicLength = 0;

String publishData = "";
String alertMsg = "";

float voltagePre = 0.0;
float currentPre = 0.0;
float powerPre = 0.0;

int duration = 0;
bool autoMotionDetect_Flag = false;
unsigned int motionCounter = 25;
unsigned int motionConfirmedCounter = motionCounter;
int PIR_THRESH = 200;
