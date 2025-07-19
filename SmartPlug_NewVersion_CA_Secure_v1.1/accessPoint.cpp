#include "accessPoint.h"  // Include the header for the ACCESSPOINT class
#include "Config.h"       // Include project configuration file (for global settings, EEPROM addresses, etc.)
#include "myWiFi.h"       // Include the WiFi management class
// #include <WiFi.h>         // Include WiFi library for ESP32
#include <ESP8266WiFi.h>
// #include <AsyncTCP.h>           // Include AsyncTCP library (ESPAsyncTCP for ESP8266/ESP32)
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>  // Include asynchronous web server library

myWIFI wifi;                // Create an instance of WiFi management class
subPubTopics iTopic;        // Create an instance to hold MQTT topics
AsyncWebServer server(80);  // Create an Async Web Server object on port 80

ACCESSPOINT::ACCESSPOINT() {}  // Constructor for ACCESSPOINT class (does nothing)

void ACCESSPOINT::accessPointSetup() {
  // If SSID and PASSWORD are empty or lengths are invalid
  if (((SSID == "") && (PASSWORD == "")) || ((ssidLength == 255) || (passwordLength == 255))) {
    // Start Access Point to get SSID and Password
    readSsidAndPasswordFromAP(apSSID.c_str(), apPassword.c_str());
    // Serial.print("Waiting for SSID and Password from AP");
    // Wait in loop until valid SSID and PASSWORD are received
    while (((SSID == "") && (PASSWORD == "")) || ((ssidLength == 255) || (passwordLength == 255))) {
      digitalWrite(_wifiStatusLED, HIGH);  // Turn WiFi status LED ON
      delay(100);                          // Wait for 100ms
      digitalWrite(_wifiStatusLED, LOW);   // Turn WiFi status LED OFF
      delay(100);                          // Wait for 100ms
      // Serial.print(".");                   // Print dot to indicate waiting
    }
    // Serial.println("Received SSID and Password");
    wifi.wiFiSetup(SSID, PASSWORD);  // Setup WiFi connection using received credentials
  } else {
    wifi.wiFiSetup(SSID, PASSWORD);  // If credentials are already available, connect WiFi directly
  }
}

void ACCESSPOINT::updateTheSSIDAndPASSFromMqttIfAvailable() {
  // Check if SSID and Password change is requested via MQTT
  if (globallySSIDAndPasswordChange) {
    bool wiFiStatus = wifi.wiFiSetup(SSID, PASSWORD);  // Try connecting to new SSID and Password
    // Serial.println(SSID);                                                                    // Debug print: New SSID
    // Serial.println(PASSWORD);                                                                // Debug print: New Password
    wifi.reconnectToMqtt(iTopic.Publish, iTopic.Subscribe, iTopic.Global);                   // Reconnect MQTT
    if (wiFiStatus) {                                                                        // If WiFi connection is successful
      wifi.publishMqttMsg_Alert(iTopic.Publish, deviceId, "Wi-Fi Credentials Changed");      // Publish success message
      wifi.writeOneByteInEEPROM(ssidLenghtEEPROMAdd, SSID.length());                         // Save SSID length to EEPROM
      wifi.writeOneByteInEEPROM(passwordLenghtEEPROMAdd, PASSWORD.length());                 // Save password length to EEPROM
      wifi.storeStringInEEPROM(SSID, ssidEEPROMAdd);                                         // Save SSID to EEPROM
      wifi.storeStringInEEPROM(PASSWORD, passEEPROMAdd);                                     // Save Password to EEPROM
    } else {                                                                                 // If WiFi connection failed
      wifi.publishMqttMsg_Alert(iTopic.Publish, deviceId, "Wi-Fi Credentials not Changed");  // Publish failure message
    }
    globallySSIDAndPasswordChange = false;  // Reset flag
  }
}

void ACCESSPOINT::readSsidAndPasswordFromAP(const char* ssid, const char* pass) {
  WiFi.softAP(ssid, pass);  // Start Access Point with given SSID and Password
  // Serial.println("Access Point SSID: " + String(ssid));      // Print AP SSID
  // Serial.println("Access Point Password: " + String(pass));  // Print AP Password

  // Define HTTP GET endpoint '/wiFiCredentials' to receive SSID and password
  server.on("/wiFiCredentials", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (request->hasParam("ssidAndPass")) {                               // Check if 'ssidAndPass' parameter exists
      const AsyncWebParameter* param = request->getParam("ssidAndPass");  // Get the parameter
      if (param != nullptr) {
        String SSIDANDPASS = param->value();  // Read value of the parameter
        // Serial.print("Received Data: ");
        // Serial.println(SSIDANDPASS);                 // Print received data
        request->send(200, "text/plain", deviceId);  // Respond with deviceId

        int delimiterIndex = SSIDANDPASS.indexOf(':');         // Find index of ':' delimiter
        SSID = SSIDANDPASS.substring(0, delimiterIndex);       // Extract SSID
        PASSWORD = SSIDANDPASS.substring(delimiterIndex + 1);  // Extract Password

        // Serial.print("tempSSID: ");
        // Serial.println(SSID);
        // Serial.print("tempPASSWORD: ");
        // Serial.println(PASSWORD);

        wifi.earasWiFiCredentialsFromEEPROM();  // Erase old WiFi credentials from EEPROM
        // Serial.println("After Clearing EEPROM");
        wifi.writeOneByteInEEPROM(ssidLenghtEEPROMAdd, SSID.length());          // Save new SSID length
        wifi.writeOneByteInEEPROM(passwordLenghtEEPROMAdd, PASSWORD.length());  // Save new Password length
        // Serial.println("After Writing EEPROM");
        wifi.storeStringInEEPROM(SSID, ssidEEPROMAdd);      // Save new SSID
        wifi.storeStringInEEPROM(PASSWORD, passEEPROMAdd);  // Save new Password
        // Serial.println("After Storing EEPROM");
      } else {
        request->send(400);  // If parameter is missing, send Bad Request (400)
      }
    } else {
      request->send(400);  // If parameter is missing, send Bad Request (400)
    }
  });

  server.begin();  // Start the Async web server
}

void ACCESSPOINT::stopApServer() {
  server.end();  // Stop the Async Web Server
  // Serial.println("Server stopped.");
}

void ACCESSPOINT::stopApWiFi() {
  WiFi.softAPdisconnect(true);  // Disconnect the Access Point
  // Serial.println("Access Point mode stopped.");
}
