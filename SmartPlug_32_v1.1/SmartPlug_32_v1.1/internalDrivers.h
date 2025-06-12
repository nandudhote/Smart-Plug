#ifndef INTERNALDRIVERS_H
#define INTERNALDRIVERS_H

#include "Arduino.h"
#include <EEPROM.h>
#include <Wire.h>

//DateTime startTime, endTime;
typedef struct VolCurPow {
  double voltage, current, power, voltagePrev, currentPrev, powerPrev;
} V_I_P;

typedef enum {
  ON = true,
  OFF = false
} loadStatus;

struct SplitData {
  String indexOneData;
  String indexTwoData;
  String indexThreeData;
};

class internalDrivers {
public:
  internalDrivers();
  void gpioInit();
  void relayControl(bool state);
  void readDataFromEEPROM();
  void writeOneByteInEEPROM(int Add, byte data);
  void earasWiFiCredentialsFromEEPROM();
  void storeStringInEEPROM(String data, byte Addr);
  String loadStringFromEEPROM(byte Addr, byte Length);
  SplitData splitStringByColon(const String& data);
  void motionDetectionUsingPIR();
};

#endif
