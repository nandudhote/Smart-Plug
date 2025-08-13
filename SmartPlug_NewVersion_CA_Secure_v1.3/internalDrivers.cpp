#include "internalDrivers.h"
#include "Config.h"
#include "myWiFi.h"

myWIFI iwiFI;

internalDrivers::internalDrivers() {
}

void internalDrivers::gpioInit() {
  // pinMode(_wifiStatusLED, OUTPUT);
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
  EEPROM.begin(512);
  if (((SSID == "") && (PASSWORD == "")) || ((ssidLength == 255) || (passwordLength == 255))) {
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
  byte motionDetectThresholdTemp = EEPROM.read(motionDetectThresholdMsgEEPROM);
  if(motionDetectThresholdTemp != 0) {
    motionDetectThreshold = motionDetectThresholdTemp;
  }
  // int tempIntPIRThresholdValue = EEPROM.read(pirThresholdIntEEPROMAdd);
  // int tempFloatPIRThresholdValue = EEPROM.read(pirThresholdFloatEEPROMAdd);
  // int PIR_THRESHtemp = int((tempIntPIRThresholdValue + float(tempFloatPIRThresholdValue / 100.0)) * 20.0);
  // if (PIR_THRESHtemp != 0) {
  //   PIR_THRESH = PIR_THRESHtemp;
  // }
  autoMotionDetect_Flag = EEPROM.read(autoMotionFlagEEPROMAddr);
  if (!autoMotionDetect_Flag) {
    relayControl(bool(EEPROM.read(loadStateEEPROMAddr)));
  }
}

// =====================================
// Function to handle motion detection
// =====================================
void internalDrivers::motionDetectionUsingPIR() {
  if(autoMotionDetect_Flag) {
  bool pirState = digitalRead(_PIRSensorPin); // Read current state from PIR sensor (HIGH = motion detected, LOW = no motion)
  // Serial.print("PIR State: "); Serial.println(pirState);    // Print PIR reading for debugging
  // If motion is detected
  if (pirState == HIGH) {
    motionDetectCount++; // Increment consecutive motion detection counter
    // Serial.print("Motion Detect Count: "); Serial.println(motionDetectCount);
  } else {
    motionDetectCount = 0; // Reset counter if no motion is detected
  }
  // Case 1: No motion has been detected yet
  if (!motionDetected) {
    // Check if motion has been consistently detected enough times to be valid
    if (motionDetectCount >= motionDetectThreshold) {
      motionDetected = true;  // Mark motion as detected
      onTimer = 0;            // Reset ON timer
      relayControl(1);        // Turn relay ON
      // Serial.println("Motion Detected - Relay ON");
      motionDetectCount = 0;  // Reset detection counter
    } else {
      relayControl(0); // Keep relay OFF until motion is confirmed
      // Serial.println("Waiting for motion...");
    }
  // Case 2: Motion is already active
  } else {
    // If motion is detected again while relay is ON
    if (motionDetectCount >= motionDetectThreshold) {
      onTimer = 0;            // Reset ON timer (extend ON time)
      motionDetectCount = 0;  // Reset motion count
      // Serial.println("Motion Retriggered - Timer Reset");
    }
    onTimer++; // Increase ON timer counter
    // Serial.print("ON Timer: "); Serial.println(onTimer);
    // If ON time exceeds set limit
    if (onTimer >= motionCounter) {
      relayControl(0);        // Turn relay OFF
      motionDetected = false; // Reset motion detection flag
      motionDetectCount = 0;  // Reset motion count
      // Serial.println("Timeout - Relay OFF");
    } else {
      relayControl(1); // Keep relay ON if timer hasn't expired
    }
  }
 }
}

// void internalDrivers::motionDetectionUsingPIR() {
//   if (autoMotionDetect_Flag) {
//     duration = pulseIn(_PIRSensorPin, HIGH);
//     motionConfirmedCounter += 1;
//     // This is to omit the counter range overflow error
//     if (motionConfirmedCounter >= 65500) {
//       motionConfirmedCounter = motionCounter + 1;
//     }
//     if ((duration > PIR_THRESH) || (motionConfirmedCounter <= motionCounter)) {
//       if ((duration > PIR_THRESH))
//         motionConfirmedCounter = 0;
//       relayControl(1);
//       delay(100);
//     } else {
//       relayControl(0);
//     }
//   }
// }