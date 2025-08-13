#ifndef INTERNALDRIVERS_H
#define INTERNALDRIVERS_H

#include "Arduino.h"
#include <EEPROM.h>

typedef enum {
  ON = true,
  OFF = false
} loadStatus;

class internalDrivers {
public:
  internalDrivers();
  void gpioInit();
  void relayControl(bool state);
  void readDataFromEEPROM();
  void motionDetectionUsingPIR();
};

#endif
