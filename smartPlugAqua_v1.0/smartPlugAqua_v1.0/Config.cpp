#include "Config.h"
#include "internalDrivers.h"

String SSID = "";
String PASSWORD = "";

byte MAC[6] = {0x8C, 0x4C, 0xAD, 0xF0, 0xF0, 0xF0};  // 8C:4C:AD:F0:BE:83
const char* ServerMQTT = "203.109.124.70";
//char* ServerMQTT = "evoluzn.org";
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
const byte loadStateEEPROMAddr = 43;
const byte espRestartFlagEEPROMAddr = 44;
const byte autoMotionFlagEEPROMAddr = 45;

const char _relayPin = 13;
const char _statusLED = 4;
const char _PIRSensorPin = 14;

bool responseOn200Request = false;
unsigned int previousTime = 0;
byte ethernetResetCounter = 0;
bool relayLoadStaus = false;

float voltagePre = 0.0;
float currentPre = 0.0;
float powerPre = 0.0;

byte ssidLength = 0;
byte passwordLength = 0;
bool enterInAPMode = false;
bool globallySSIDAndPasswordChange = false;
bool espRestartFlag = false;
byte wiFiRetryCounter = 0;
bool isApStarted = false;
byte mqttCounter = 0;

bool autoMotionDetect_Flag = false;
unsigned int motionCounter = 15;
unsigned int motionConfirmedCounter = motionCounter;
int PIR_THRESH = 200;

bool operationActive;//For Scheduling
unsigned int sampleCount = 0;

