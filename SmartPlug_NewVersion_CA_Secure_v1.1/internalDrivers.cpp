#include "internalDrivers.h"
#include "Config.h"
#include "myWiFi.h"

myWIFI iwiFI;

internalDrivers::internalDrivers() {
}

void internalDrivers::gpioInit() {
  pinMode(_wifiStatusLED, OUTPUT);
  pinMode(_relayPin, OUTPUT);
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
  relayControl(bool(EEPROM.read(loadStateEEPROMAddr)));
}
