#include "myWiFi.h"       // Include custom WiFi management header
#include "Config.h"       // Include configuration constants
#include "accessPoint.h"  // Include Access Point handling class
#include "internalDrivers.h"
#include <EEPROM.h>  // Include EEPROM library to read/write non-volatile memory

// Certificates and key
const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC6TCCAdGgAwIBAgIUL+Qw3ZBWWW2KuZGeTmlHHYncy0EwDQYJKoZIhvcNAQEL
BQAwFjEUMBIGA1UEAwwLSU9ULVJPT1QtQ0EwHhcNMjQxMTI5MTM0NDQxWhcNMzQx
MTI3MTM0NDQxWjAWMRQwEgYDVQQDDAtJT1QtUk9PVC1DQTCCASIwDQYJKoZIhvcN
AQEBBQADggEPADCCAQoCggEBAI+8Bk4EgZDUOGFa056SbjQ/SFWdSlglVWUSC6sk
xCjO0NBuciVsMwOOFo+M9F2tIEo74DMdJwp4Ggs8w2Zw0NOYioI4VyKry55buK2Y
h4149wuRt3QayIBqy4tTWw+iFx9v+otdmudfRNWsCmrUAG+maXB3mlpaJauUav4J
DcRtX2ex/vWT6woVxf6GnwE9mZRgVrKHMNHVVe0mDAdDyFyRhFou13lPUSBBDzdK
H/3tszh/euHHQEcjGOjbv6S1uLC9WbTJqKqEO4z5EzNhw6Co44xOAjZo25L+QYtw
0Y5UJk1QHlsY2VjgiuoUf3VMK+RXVsWdB5kEwu3ivB+K5j0CAwEAAaMvMC0wDAYD
VR0TBAUwAwEB/zAdBgNVHQ4EFgQUA/CYVEtdNDD3z96n74ByYLb2dDQwDQYJKoZI
hvcNAQELBQADggEBAElI37iWwKUXHyVbKN9Te50Ixz7WLgDnRSDQie9g4vQvbsXt
p+a9uQde8FAIn4+fGnXi073CaVqFIS4x70ZlW96IvVLUmik0OltTjyRQnC7FqnSV
vppToWfOmVutn/dDXlaxKIpsiHTkmQ+IOJjlazflEhsWOcocxidYfuHik8MlAHbc
q4TK+AYK+ESsP60CScsqDxlfeBHJTU2E1BN599pupe4BJUVt88SomfHlqyX4xkR8
EaNnB+OJTGHhReLTcfRWsINrQPNdmr+3NvARHqQoAmWCYf/F+iBCIBFTHfSR4o+3
K129Kynhmg28ua05Nd63Siqx1eFoYfC080Vu9dc=
-----END CERTIFICATE-----
)EOF";

const char* client_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIICtzCCAZ8CFAMWKtWNMeiwoOcdNsrDbeyVuOnpMA0GCSqGSIb3DQEBCwUAMBYx
FDASBgNVBAMMC0lPVC1ST09ULUNBMB4XDTI0MTIwNDEzMTIzOVoXDTI2MDQxODEz
MTIzOVowGjEYMBYGA1UEAwwPMjM3NzQ3MzM4NjE2MDczMIIBIjANBgkqhkiG9w0B
AQEFAAOCAQ8AMIIBCgKCAQEAqCtoLCycHufNh5/RLGWpB0ikRjY4bu03VrsoTeOR
HDq8Q28zDfK/4//43vkWkVIT3zkvNSVgsM0luWi0zi26mZ/qiIHf/mXALJjogqxU
7hLtGyH3VidXUGYDGluQbzUEOFBqmxrgmr0tkBCdpQr0tiAs0RFo2R3C6a8lwEBW
0sIC5K4EmuWpuxApQ+L96tXFYvLcGyW8niMFBMXnGVg5QfZjjWQ2KhjTu4DdAFX1
KXz4jxJrnd6hqbisioGweiWvKK4Qv+ZiEn5DQn3cPGrWW/K1EvMxPQg41djEgb8x
oaFL/ppyuxhKBYzarvbTd118zW/TU1+D++A3FUMri5y/lwIDAQABMA0GCSqGSIb3
DQEBCwUAA4IBAQCNIVVzO//qZAbcwimyyib+PL23+0hkJy1ECfiJPzMX0V58R2f+
ZnLXrd9BUfBGlvbMHWfRx/KWjvqd3FhXf6Rxx5OfICTnRAR3xKKEqhfHbBoLj1vh
igwu7IrQhQqLc6mhUTgr58XZbXnHY+rSlmGWxcS+D2uBZy9jYSq6Vv++RLErKPCp
RU5R/zWArSZngZe3hT7yfvzAvI8+O2CiJnuH9aaOdP9OmhKopIr/JGZAW8utyvmq
Zr7bWN4jDMvAxDwMA8PL5k0lDXrTGGB4wqebpYsurYCm7wtjH+QWbPZgUmu8DrRf
CPMjwlPux/wFKX3P0A4/U8AdeRWKU9dkcS3V
-----END CERTIFICATE-----
)EOF";

const char* client_key = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAqCtoLCycHufNh5/RLGWpB0ikRjY4bu03VrsoTeORHDq8Q28z
DfK/4//43vkWkVIT3zkvNSVgsM0luWi0zi26mZ/qiIHf/mXALJjogqxU7hLtGyH3
VidXUGYDGluQbzUEOFBqmxrgmr0tkBCdpQr0tiAs0RFo2R3C6a8lwEBW0sIC5K4E
muWpuxApQ+L96tXFYvLcGyW8niMFBMXnGVg5QfZjjWQ2KhjTu4DdAFX1KXz4jxJr
nd6hqbisioGweiWvKK4Qv+ZiEn5DQn3cPGrWW/K1EvMxPQg41djEgb8xoaFL/ppy
uxhKBYzarvbTd118zW/TU1+D++A3FUMri5y/lwIDAQABAoIBAQChDIDE3SveBQu1
eH3vUKQhD5YSj1ZYFODMNIwSm36aVD17LwDruK+//6Esl8re/IV/XtowwUzpxGz+
NKICWZS8DoOimw4SKQtzKjRw/+yN2p4rP8oIPcDGhAswsMOPXGxgKdMOGx1B0y3R
K7htGCLmT8XbqhqI4pGJDWzPwk3jos7IiR92RelTzojAFP+sUTj14fYvcqM1j0nx
1hlbqneDTcFHdkjVlY2B+TLXVmtCLMPUzQfVt+nYs5T6S4dhuWAkiXgIo5vdjeXU
PT1FPD/5oBuC7Cx8bfZqzOTnKPfTVrjlHl02YdrdxR4UNjtoESfZZGrgA7gnVlNH
EUPmE4LRAoGBANzLnkgrsvA3w/+GWylsQL42RfBLRPL8vhk/xS+/esroGzsqg0Yw
TbYM9cwy70bjpNaJHnCsJ6dLjWFXe33lQE/DRvHfjcsz0AoG+gO4/pcsSxfz/xGD
vfsHu25GKdPTIt3LXd8teWc0EKtprOavb15RgHSLMX3H0TA5Sd7yrf3bAoGBAML7
t95O2uIRA/KqHbTJsg5yXznYBCYltB5zCoqfbWD7NlXLt2N1SN9GzC2KQ/FBDjgX
hX0GhKd/lfdrT5xoXhvRNbLEylsiOFAuCLKT4QuYwnCsSfAxqBPBt7EcRjtShHuM
BWEcR6p1I4rm3nz8OCjd5JF28SRRUZu3mJF1yHf1AoGBAMzv+kSGuxoYyMW+UZNL
NXTYL2ZwltjrGw+Yj1BYbIM1mUD7nslhQGA/+03D4TBtT60JrMVsJYo0qE+3wZoD
VgQy/wC2PYhk2S7Vdu5JoZVzuWfMQ+5f19DPRnt+iIhiqzQu9TyXmt3eB7Ly7VdZ
cweaop6KLC/RbitSoWxc2H6NAoGAeKqq+Ys3blw1Y0HgLHf+F260e+mxd6Ixu/5Z
8bwERtZwo9v03DpDJNp9lVlR/sXR9Oipldwo0DfnKT1iAVudkf2BC+c5i/i7fPNg
MpPN3A1nl68n+tvgNonWEmd7F09T6dmXoqeLunPP0TBbVGIKp0/bGiLPzTf+Fy4h
3zNTItkCgYEApNQkrCV/wqJ93BRmmwLxHS3lZmH/frnd3x7SBsTqJLtuD5CHDIl3
RUInBJ70DdjQF8g+TcqBIHt0laxjEx34reLIJRzwMhA0IGnkfXPKGJp3H6Ds4A33
aga5sjxqE91Y8gs1yy3QpbPt1HBxO1umLJ4HcnbfWroWRsqbuxHMnO0=
-----END RSA PRIVATE KEY-----
)EOF";

// Create global instances
ACCESSPOINT ap;  // Access Point object
myWIFI iWIFI;    // myWIFI object instance
internalDrivers iiDrivers;
myWIFI wifiObj;  // Another myWIFI object instance for general usage
// WiFiClient WLS_Client;  // Create a WiFi client object (for MQTT)
BearSSL::WiFiClientSecure secureClient;
PubSubClient client(secureClient);
// PubSubClient client(WLS_Client);  // Create MQTT client using WiFi client

// Constructor for myWIFI class (does nothing special here)
myWIFI::myWIFI() {}

// Function to setup WiFi connection
bool myWIFI::wiFiSetup(String ssid, String pass) {
  delay(10);             // Small delay for stability
  byte wiFiCounter = 0;  // Counter to retry WiFi connection attempts
  // Serial.println();
  // Serial.print("Connecting to ");
  // Serial.println(SSID);  // Print the SSID we're trying to connect to

  WiFi.begin(ssid.c_str(), pass.c_str());  // Start connecting to the WiFi network
  while (WiFi.status() != WL_CONNECTED) {  // Loop until connected
    // digitalWrite(_wifiStatusLED, HIGH);    // Turn LED ON
    // delay(250);
    // digitalWrite(_wifiStatusLED, LOW);  // Turn LED OFF
    // delay(250);
    // Serial.print(".");  // Print progress
    wiFiCounter++;  // Increment retry counter

    if (wiFiCounter >= 15) {  // If 15 tries failed, break out
      break;
    }
  }

  if (WiFi.status() != WL_CONNECTED) {  // Still not connected after retries
    isWifiConnected = false;            // Set flag
    return false;                       // Return failure
  }

  // If connected successfully:
  // Serial.println();
  // Serial.println("WiFi connected");
  // digitalWrite(_wifiStatusLED, LOW);  // Keep LED ON
  // delay(100);
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());  // Show assigned IP address
  ap.stopApServer();       // Stop the access point server
  ap.stopApWiFi();         // Stop access point WiFi
  isWifiConnected = true;  // Set flag
  return true;             // Return success
}

// Setup MQTT server and callback For TCP Connection
// void myWIFI::mqttSetup(const char* MqttSever, int MqttPort) {
//   client.setServer(MqttSever, MqttPort);  // Set MQTT broker server and port
//   client.setCallback(MQTT_Pull);          // Set function to call when a message arrives
// }

// Setup MQTT server and callback For SSL Connection
void myWIFI::mqttSetup(const char* MqttSever, int MqttPort) {
  // Configure secure connection
  secureClient.setInsecure();
  // secureClient.setTrustAnchors(new BearSSL::X509List(ca_cert));
  secureClient.setClientRSACert(
    new BearSSL::X509List(client_cert),
    new BearSSL::PrivateKey(client_key));
  client.setServer(MqttSever, MqttPort);
  client.setCallback(MQTT_Pull);
}

// Check if WiFi is connected
bool myWIFI::wiFiLinkCheck() {
  return WiFi.status() == WL_CONNECTED;
}

// Reconnect to MQTT server, with Last Will Testament
void myWIFI::reconnectToMqtt(String pubTopic, String subTopic, String globalTopic) {
  String willMessage = "{" + deviceId + ":offline}";  // LWT (Last Will) message if device disconnects
  if (!client.connected()) {                          // If not connected to MQTT broker
    // Serial.print("Attempting MQTT connection...");
    // if (client.connect(deviceId.c_str(), mqttUserName, mqttUserPassword, pubTopic.c_str(), 1, true, willMessage.c_str())) {
    if (client.connect(deviceId.c_str(), pubTopic.c_str(), 1, true, willMessage.c_str())) {
      // Connected successfully
      // Serial.println("connected");
      subscribeTopics(subTopic);
      subscribeTopics(globalTopic);
      String onlineMessage = "{" + deviceId + ":online}";             // Publish online status
      client.publish(pubTopic.c_str(), onlineMessage.c_str(), true);  // Publish to MQTT
    } else {
      mqttCounter++;                                                       // Increment failed MQTT connection counter
      if (mqttCounter >= 15) {                                             // After 15 failed tries
        ap.readSsidAndPasswordFromAP(apSSID.c_str(), apPassword.c_str());  // Start AP for new credentials
        isApStarted = true;                                                // Set AP started flag
        mqttCounter = 0;                                                   // Reset counter
      }
      // delay(100);  // Small delay before retry
    }
  }
}

// MQTT callback when a new message arrives
void myWIFI::MQTT_Pull(char* topic, byte* payload, unsigned int length) {
  String MqttRecdMsg = "";
  for (int i = 0; i < length; i++) {  // Convert payload to a clean String
    if (((char)payload[i] != ' ') && ((char)payload[i] != '\n')) {
      MqttRecdMsg += (char)payload[i];
    }
  }
  // Serial.println(MqttRecdMsg);
  // Serial.println();

  SplitData mqttDecodedMsg = wifiObj.splitStringByColon(MqttRecdMsg);  // Split message by ':'

  // Handle based on the message type
  if (mqttDecodedMsg.indexOneData == "wiFiCredentials") {
    SSID = mqttDecodedMsg.indexTwoData;        // New SSID received
    PASSWORD = mqttDecodedMsg.indexThreeData;  // New password received
    globallySSIDAndPasswordChange = true;      // Set flag to update credentials
  } else if (mqttDecodedMsg.indexOneData == "earasWiFiCredentialsFromEEPROM") {
    enterInAPMode = true;  // Enter AP mode to reset
  } else if (mqttDecodedMsg.indexOneData == "200") {
    responseOn200Request = true;  // Some external HTTP 200 OK response trigger
  } else if (mqttDecodedMsg.indexOneData == "0") {
    iiDrivers.relayControl(0);
    alertMsg = "Load OFF ";
  } else if (mqttDecodedMsg.indexOneData == "100") {
    iiDrivers.relayControl(1);
    alertMsg = "Load ON ";
  } else if (mqttDecodedMsg.indexOneData == "publishInterval") {
    interval = mqttDecodedMsg.indexTwoData.toInt();                    // Update publishing interval
    wifiObj.writeOneByteInEEPROM(publishIntervalEEPROMAdd, interval);  // Save new interval
    alertMsg = "publishInterval is updated : " + String(interval);     // Store alert message
  } else if (mqttDecodedMsg.indexOneData == "autoMotionDetection") {
    if (mqttDecodedMsg.indexTwoData == "100") {
      autoMotionDetect_Flag = true;
      wifiObj.writeOneByteInEEPROM(autoMotionFlagEEPROMAddr, autoMotionDetect_Flag);
      alertMsg = "autoMotionActivate";
    } else if (mqttDecodedMsg.indexTwoData == "0") {
      autoMotionDetect_Flag = false;
      wifiObj.writeOneByteInEEPROM(autoMotionFlagEEPROMAddr, autoMotionDetect_Flag);
      alertMsg = "autoMotionDeactivate";
    }
  } else if (mqttDecodedMsg.indexOneData == "motionCounter") {
    motionCounter = (mqttDecodedMsg.indexTwoData).toInt();
    wifiObj.writeOneByteInEEPROM(motionCounterMsgEEPROM, motionCounter);
    alertMsg = "motionCounterUpdated: " + String(motionCounter);
  } else if (mqttDecodedMsg.indexOneData == "motionDetectThreshold") {
    motionDetectThreshold = (mqttDecodedMsg.indexTwoData).toInt();
    wifiObj.writeOneByteInEEPROM(motionDetectThresholdMsgEEPROM, motionDetectThreshold);
    alertMsg = "motionCounterUpdated: " + String(motionDetectThreshold);
  } 
  // else if (mqttDecodedMsg.indexOneData == "PIRThreshold") {
  //   PIR_THRESH = (mqttDecodedMsg.indexTwoData).toInt();
  //   float tempIntPIRThresholdValue = PIR_THRESH / 20.0;
  //   int tempFloatPIRThresholdValue = (tempIntPIRThresholdValue - int(tempIntPIRThresholdValue)) * 100;
  //   wifiObj.writeOneByteInEEPROM(pirThresholdIntEEPROMAdd, int(tempIntPIRThresholdValue));
  //   wifiObj.writeOneByteInEEPROM(pirThresholdFloatEEPROMAdd, tempFloatPIRThresholdValue);
  //   alertMsg = "PIRThresholdUpdated: " + String(PIR_THRESH);
  // } 
  else if (mqttDecodedMsg.indexOneData == "updateMqttTopic") {
    if (mqttDecodedMsg.indexThreeData.length() < 20) {
      if (mqttDecodedMsg.indexTwoData == "globalTopic") {
        wifiObj.unSubscribe(globalTopic + subTopic);
        globalTopic = mqttDecodedMsg.indexThreeData;
        wifiObj.subscribeTopics(globalTopic + subTopic);
        wifiObj.writeOneByteInEEPROM(globalTopicLenghtEEPROMAdd, globalTopic.length());
        wifiObj.storeStringInEEPROM(globalTopic, globalTopicEEPROMAdd);
        alertMsg = "GlobalTopicUpdated: " + globalTopic;
      }
    }
  } else if (mqttDecodedMsg.indexOneData == "storeStatus") {
    // alertMsg = String(SSID) + ":" + String(PASSWORD) + ":" + String(globalTopic) + ":" + String(interval) + ":" + String(motionCounter) + ":" + String(PIR_THRESH) + ":" + String(duration);
       alertMsg = String(SSID) + ":" + String(PASSWORD) + ":" + String(globalTopic) + ":" + String(interval) + ":" + String(motionCounter);
  } else {
    delay(1);  // Small delay for unknown message
  }
}

// Check if MQTT client is connected
bool myWIFI::CheckMQTTConnection() {
  return client.connected();
}

// Must be called regularly to maintain MQTT connection
void myWIFI::clientLoop() {
  client.loop();
}

// Publish a normal MQTT message
void myWIFI::publishMqttMsg(String pubTopic, String devID, float voltage, float current, double power, bool loadStatus, bool motionStatus) {
  String Final = "{device_id:" + devID + ":" + String(voltage) + ":" + String(current) + ":" + String(power) + ":" + String(loadStatus) + ":" + String(motionStatus) + "}";
  client.publish(pubTopic.c_str(), Final.c_str());
}

// Publish an alert MQTT message
void myWIFI::publishMqttMsg_Alert(String pubTopic, String devID, String Alert) {
  String Final = "{device_id:" + devID + ":" + Alert + "}";  // Format alert message
  client.publish(pubTopic.c_str(), Final.c_str());
}

// Create topic strings for publish/subscribe
subPubTopics myWIFI::createSubPubTopics(String devID, String SubTopic, String PubTopic, String globTopic) {
  subPubTopics Topics;
  Topics.Subscribe = devID + SubTopic;     // Device-specific subscription topic
  Topics.Publish = devID + PubTopic;       // Device-specific publish topic
  Topics.Global = globalTopic + SubTopic;  // Global subscription topic
  return Topics;
}

// Create a device ID from MAC address
String myWIFI::prepareDevID(byte mac[], String devPref) {
  char devID[30];
  snprintf(devID, sizeof(devID), "%s%02X%02X%02X", devPref.c_str(), mac[3], mac[4], mac[5]);
  return String(devID);  // Return device ID as string
}

// Clear stored WiFi credentials in EEPROM
void myWIFI::earasWiFiCredentialsFromEEPROM() {
  // Serial.println("Clearing EEPROM");
  for (int i = 0; i < 42; i++) {
    EEPROM.write(i, 0);  // Clear first 42 bytes
    // delay(10);
  }
  EEPROM.commit();  // Save changes
}

// Write a single byte to EEPROM
void myWIFI::writeOneByteInEEPROM(int Add, byte data) {
  EEPROM.write(Add, data);  // Write at specific address
  EEPROM.commit();          // Save changes
}

// Store a full String in EEPROM
void myWIFI::storeStringInEEPROM(String data, byte Addr) {
  for (int i = 0; i < data.length(); i++) {
    EEPROM.write(Addr + i, data.charAt(i));  // Write each character
    // delay(10);
  }
  EEPROM.commit();  // Save changes
}

// Load a string from EEPROM
String myWIFI::loadStringFromEEPROM(byte Addr, byte Length) {
  String readData = "";
  for (int i = Addr; i < (Addr + Length); i++) {
    readData += char(EEPROM.read(i));  // Read each character
    // delay(10);
  }
  return readData;
}

void myWIFI::subscribeTopics(String subsTopic) {
  client.subscribe(subsTopic.c_str());
}

void myWIFI::unSubscribe(String subPubTopic) {
  client.unsubscribe(subPubTopic.c_str());
}

// Split a string by ':' delimiter into parts
SplitData myWIFI::splitStringByColon(const String& data) {
  SplitData mqttMsg;
  int firstIndex = data.indexOf(':');
  if (firstIndex != -1) {  // Found first delimiter
    mqttMsg.indexOneData = data.substring(0, firstIndex);
    int secondIndex = data.indexOf(':', firstIndex + 1);
    if (secondIndex != -1) {  // Found second delimiter
      mqttMsg.indexTwoData = data.substring(firstIndex + 1, secondIndex);
      mqttMsg.indexThreeData = data.substring(secondIndex + 1);
      if (mqttMsg.indexThreeData.length() > 0) {
        // third index exists
      }
    } else {
      mqttMsg.indexTwoData = data.substring(firstIndex + 1);  // Only two parts
    }
  } else {
    mqttMsg.indexOneData = data.substring(firstIndex + 1);  // No delimiter found, strange fallback
  }
  return mqttMsg;
}
