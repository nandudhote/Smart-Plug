#ifndef CSE7759_H
#define CSE7759_H

#include "Arduino.h"
#include <string.h>
#include <math.h>

typedef struct {
  float voltage;
  float current;
  float power;
  char hex[30];
  unsigned int readings[24];
  unsigned int checksum;
  int test_flag;
  bool ifDataIsOk;
  float totalVoltge;
  float totalCurrent;
  float totalPower;
} PowerData;

class CSE7759 {
public:
  CSE7759();
  void initialize_power_data(PowerData* data);
  int hex_to_dec(const char* hex);
  void prepare_hex(unsigned int byte1, unsigned int byte2, unsigned int byte3, char* hex);
  void calculate_power(PowerData* data);
  PowerData read_power_data();
  PowerData validateTheCSEChipData(PowerData powerData, bool loadStatus);
};

#endif
