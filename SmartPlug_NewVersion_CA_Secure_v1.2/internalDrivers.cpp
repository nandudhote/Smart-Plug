#include "internalDrivers.h"
#include "Config.h"
#include "myWiFi.h"

myWIFI iwiFI;

internalDrivers::internalDrivers() {
}

void internalDrivers::gpioInit() {
  pinMode(_wifiStatusLED, OUTPUT);
  pinMode(_relayPin, OUTPUT);
  pinMode(_PIRSensorPin, INPUT);
}

void internalDrivers::relayControl(bool state) {
  digitalWrite(_relayPin, state);
  relayLoadStaus = state;
  delay(10);
  iwiFI.writeOneByteInEEPROM(loadStateEEPROMAddr, byte(state));
}

void internalDrivers::readDataFromEEPROM() {
  if (((SSID == "") && (PASSWORD == "")) || ((ssidLength == 255) || (passwordLength == 255))) {
    EEPROM.begin(512);
    ssidLength = EEPROM.read(ssidLenghtEEPROMAdd);
    passwordLength = EEPROM.read(passwordLenghtEEPROMAdd);
    SSID = iwiFI.loadStringFromEEPROM(ssidEEPROMAdd, ssidLength);
    PASSWORD = iwiFI.loadStringFromEEPROM(passEEPROMAdd, passwordLength);
  }
  globalTopicLength = EEPROM.read(globalTopicLenghtEEPROMAdd);
  String globalTopicTemp = iwiFI.loadStringFromEEPROM(globalTopicEEPROMAdd, globalTopicLength);
  if (globalTopicTemp != NULL) {
    globalTopic = globalTopicTemp;
  }
  byte intervalTemp = EEPROM.read(publishIntervalEEPROMAdd);
  if (intervalTemp != 0) {
    interval = intervalTemp;
  }
  byte motionCounterTemp = EEPROM.read(motionCounterMsgEEPROM);
  if (motionCounterTemp != 0) {
    motionCounter = motionCounterTemp;
  }
  int tempIntPIRThresholdValue = EEPROM.read(pirThresholdIntEEPROMAdd);
  int tempFloatPIRThresholdValue = EEPROM.read(pirThresholdFloatEEPROMAdd);
  int PIR_THRESHtemp = int((tempIntPIRThresholdValue + float(tempFloatPIRThresholdValue / 100.0)) * 20.0);
  if (PIR_THRESHtemp != 0) {
    PIR_THRESH = PIR_THRESHtemp;
  }
  autoMotionDetect_Flag = EEPROM.read(autoMotionFlagEEPROMAddr);
  if (!autoMotionDetect_Flag) {
    relayControl(bool(EEPROM.read(loadStateEEPROMAddr)));
  }
}

void internalDrivers::motionDetectionUsingPIR() {
  if (autoMotionDetect_Flag) {
    duration = pulseIn(_PIRSensorPin, HIGH);
    motionConfirmedCounter += 1;
    // This is to omit the counter range overflow error
    if (motionConfirmedCounter >= 65500) {
      motionConfirmedCounter = motionCounter + 1;
    }
    if ((duration > PIR_THRESH) || (motionConfirmedCounter <= motionCounter)) {
      if ((duration > PIR_THRESH))
        motionConfirmedCounter = 0;
      relayControl(1);
      delay(100);
    } else {
      relayControl(0);
    }
  }
}