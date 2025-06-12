#include "internalDrivers.h"
#include "Config.h"

internalDrivers iiiiDrivers;

internalDrivers::internalDrivers() {
}

void internalDrivers::gpioInit() {
  pinMode(_relayPin, OUTPUT);
  pinMode(_PIRSensorPin, INPUT);
  pinMode(_statusLED, OUTPUT);
}

void internalDrivers::relayControl(bool state) {
  digitalWrite(_relayPin, state);
  relayLoadStaus = state;
  delay(10);
  writeOneByteInEEPROM(loadStateEEPROMAddr, byte(state));
}

void internalDrivers::readDataFromEEPROM() {
  EEPROM.begin(512);
  // espRestartFlag = EEPROM.read(espRestartFlagEEPROMAddr);
  relayControl(bool(EEPROM.read(loadStateEEPROMAddr)));
  ssidLength = EEPROM.read(ssidLenghtEEPROMAdd);
  Serial.print("ssidLength : ");
  Serial.println(ssidLength);
  passwordLength = EEPROM.read(passwordLenghtEEPROMAdd);
  Serial.print("passwordLength : ");
  Serial.println(passwordLength);
  SSID = loadStringFromEEPROM(ssidEEPROMAdd, ssidLength);
  PASSWORD = loadStringFromEEPROM(passEEPROMAdd, passwordLength);
  autoMotionDetect_Flag = EEPROM.read(autoMotionFlagEEPROMAddr);
}

void internalDrivers::writeOneByteInEEPROM(int Add, byte data) {
  EEPROM.write(Add, data);
  EEPROM.commit();
}

void internalDrivers::earasWiFiCredentialsFromEEPROM() {
  Serial.println("Clearing EEORM");
  for (int i = 0; i < 50; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void internalDrivers::storeStringInEEPROM(String data, byte Addr) {
  // int ssdInitEepromAddr = 0;  // Start address in EEPROM
  for (int i = 0; i < data.length(); i++) {
    EEPROM.write(Addr + i, data.charAt(i));
  }
  EEPROM.commit();
}

String internalDrivers::loadStringFromEEPROM(byte Addr, byte Length) {
  String readData = "";
  for (int i = Addr; i < (Addr + Length); i++) {
    readData += char(EEPROM.read(i));
  }
  return readData;
}

void internalDrivers::motionDetectionUsingPIR() {
  // Serial.print("AutomotionFlag=");
  // Serial.println(autoMotionDetect_Flag);
  if (autoMotionDetect_Flag) {
    int duration = pulseIn(_PIRSensorPin, HIGH);
    // Serial.print("Duration : ");
    // Serial.println(duration);
    motionConfirmedCounter += 1;
    // This is to omit the counter range overflow error
    if (motionConfirmedCounter >= 65500) {
      motionConfirmedCounter = motionCounter + 1;
    }
    if ((duration > PIR_THRESH) || (motionConfirmedCounter <= motionCounter)) {
      if ((duration > PIR_THRESH))
        motionConfirmedCounter = 0;
      // Serial.println("Motion Detected");
      //      digitalWrite(_relayPin, HIGH);
      relayControl(HIGH);
      delay(100);
    } else {
      // Serial.println("Motion Not Detected");
      //      digitalWrite(_relayPin, LOW);
      relayControl(LOW);
    }
  }
}

SplitData internalDrivers::splitStringByColon(const String& data) {
  SplitData mqttMsg;
  int firstIndex = data.indexOf(':');
  if (firstIndex != -1) {
    mqttMsg.indexOneData = data.substring(0, firstIndex);
    int secondIndex = data.indexOf(':', firstIndex + 1);
    if (secondIndex != -1) {
      mqttMsg.indexTwoData = data.substring(firstIndex + 1, secondIndex);
      mqttMsg.indexThreeData = data.substring(secondIndex + 1);
      if (mqttMsg.indexThreeData.length() > 0) {
      }
    } else {
      mqttMsg.indexTwoData = data.substring(firstIndex + 1);
    }
  } else {
    mqttMsg.indexOneData = data.substring(firstIndex + 1);
  }
  return mqttMsg;
}
