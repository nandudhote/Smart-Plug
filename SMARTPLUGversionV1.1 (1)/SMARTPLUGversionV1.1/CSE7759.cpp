#include "CSE7759.h"
#include "Config.h"

CSE7759::CSE7759() {
}

void CSE7759::initialize_power_data(PowerData* data) {
  memset(data, 0, sizeof(PowerData));
  data->test_flag = 0;
  data->checksum = 255;
  data->ifDataIsOk = false;
}

int CSE7759::hex_to_dec(const char* hex) {
  long long decimal = 0;
  int len = strlen(hex) - 1;

  for (int i = 0; hex[i] != '\0'; i++) {
    int val;
    if (hex[i] >= '0' && hex[i] <= '9') {
      val = hex[i] - '0';
    } else if (hex[i] >= 'a' && hex[i] <= 'f') {
      val = hex[i] - 'a' + 10;
    } else if (hex[i] >= 'A' && hex[i] <= 'F') {
      val = hex[i] - 'A' + 10;
    } else {
      return -1; // Invalid hex character
    }
    decimal += val * pow(16, len);
    len--;
  }
  return decimal;
}

void CSE7759::prepare_hex(unsigned int byte1, unsigned int byte2, unsigned int byte3, char* hex) {
  if (byte1 < 16 && byte2 < 16 && byte3 < 16) {
    sprintf(hex, "0%x0%x0%x", byte1, byte2, byte3);
  } else if (byte1 < 16) {
    sprintf(hex, "0%x%x%x", byte1, byte2, byte3);
  } else if (byte2 < 16) {
    sprintf(hex, "%x0%x%x", byte1, byte2, byte3);
  } else if (byte3 < 16) {
    sprintf(hex, "%x%x0%x", byte1, byte2, byte3);
  } else {
    sprintf(hex, "%x%x%x", byte1, byte2, byte3);
  }
}

void CSE7759::calculate_power(PowerData* data) {
  char temp_hex[10];
  float temp1, temp2, temp3;

  prepare_hex(data->readings[2], data->readings[3], data->readings[4], temp_hex);
  temp1 = hex_to_dec(temp_hex);
  prepare_hex(data->readings[5], data->readings[6], data->readings[7], temp_hex);
  temp2 = hex_to_dec(temp_hex);

  data->voltage = data->test_flag ? 0 : temp1 / temp2;

  prepare_hex(data->readings[8], data->readings[9], data->readings[10], temp_hex);
  temp1 = hex_to_dec(temp_hex);
  prepare_hex(data->readings[11], data->readings[12], data->readings[13], temp_hex);
  temp2 = hex_to_dec(temp_hex);

  data->current = data->test_flag ? 0 : temp1 / temp2;

  prepare_hex(data->readings[14], data->readings[15], data->readings[16], temp_hex);
  temp1 = hex_to_dec(temp_hex);
  prepare_hex(data->readings[17], data->readings[18], data->readings[19], temp_hex);
  temp2 = hex_to_dec(temp_hex);

  data->power = data->test_flag ? 0 : temp1 / temp2;
  data->test_flag = 0;
}

PowerData CSE7759::read_power_data() {
  PowerData data;
  initialize_power_data(&data);
  unsigned char byte;
  unsigned int Checksum = 0;

  Serial.println("Reading Chip...");

  for (int i = 0; i <= 100; i++) {
    byte = Serial.read();
    if (byte == 85) {
      data.readings[0] = byte;
      byte = Serial.read();
      if (byte == 90) {
        data.readings[1] = byte;
        for (int j = 2; j <= 23; j++) {
          data.readings[j] = Serial.read();
        }
        for (int x = 2; x <= 22; x++) {
          Checksum += data.readings[x];
        }
        if ((Checksum & data.checksum) == data.readings[23]) {
          calculate_power(&data);
          return data;  // Return the data object with the calculated values
          //          break;
        }
      }
    }
  }
  return data;  // Return the data object even if no valid data was read
}

/*
  INPUT: VOLTAGE, CURRENT, POWER STRUCTURE AND RELAY STATUS FLAG
  OUTPUT: VERIFIED VOLTAGE, CURRENT, POWER STRUCTURE
  OPERATION: CHECKING IF RECEIEVED DATA IS GARBAGE. IF YES, THEN RE-REQUEST FOR DATA. TRYING IT FOR 5 TIMES, IF STILL GETTING GARBAGE DATA THEN RETURNING THE PREVIOUS CORRECT DATA.
*/
PowerData CSE7759::validateTheCSEChipData(PowerData powerData, bool loadStatus) {
  byte powerValidationCounter = 0;
  float expectedPower = powerData.voltage * powerData.current;
  float expectedCurrent = powerData.power / powerData.voltage;
  if ((powerData.power > (expectedPower * 2)) || (powerData.current > (expectedCurrent * 10)) || (powerData.voltage < 20)) {
    Serial.print("###Garbage Data are -> Voltage: ");
    Serial.print(powerData.voltage, 2);
    Serial.print(" V, Current: ");
    Serial.print(powerData.current, 2);
    Serial.print(" A, Power: ");
    Serial.print(powerData.power, 2);
    Serial.println(" W");
    powerData.ifDataIsOk = false;
    powerData.voltage = voltagePre;
    powerData.current = currentPre;
    powerData.power = powerPre;
  } else {
    powerData.ifDataIsOk = true;
    voltagePre = powerData.voltage;
    currentPre = powerData.current;
    powerPre = powerData.power;
  }
  Serial.print("***Received Parameters are -> Voltage: ");
  Serial.print(powerData.voltage, 2);
  Serial.print(" V, Current: ");
  Serial.print(powerData.current, 2);
  Serial.print(" A, Power: ");
  Serial.print(powerData.power, 2);
  Serial.println(" W");
  return powerData;
}
