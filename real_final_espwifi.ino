#include <WiFi.h>
#include <WiFiUdp.h>

// Wi-Fi Credentials
const char *ssid = "DESKTOP-92HKQRB 0448";     
const char *password = "40A31v2=";    

// UDP Configuration
WiFiUDP udp;
IPAddress broadcastIp(255, 255, 255, 255);  
const unsigned int udpPort = 12345;  

void setup() {
  Serial.begin(9600);  // Default Serial for Arduino-ESP32 communication

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void loop() {
  static String receivedData = "";  // Buffer to store received data

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {  // End of message
      receivedData.trim();  // Clean up spaces

      // Extract and validate latitude & longitude
      if (isValidCoordinates(receivedData)) {
        Serial.println(receivedData);  // ✅ Print only lat, lon

        // ✅ Send only lat, lon via UDP
        udp.beginPacket(broadcastIp, udpPort);
        udp.print(receivedData);
        udp.endPacket();
      }
      receivedData = "";  // Clear buffer after sending
    } else {
      receivedData += c;  // Append character to buffer
    }
  }

  delay(10);  
}

// Function to check if received data is in valid coordinate format
bool isValidCoordinates(String data) {
  int commaIndex = data.indexOf(',');
  if (commaIndex == -1) return false; // No comma found

  String lat = data.substring(0, commaIndex);
  String lon = data.substring(commaIndex + 1);

  return isNumber(lat) && isNumber(lon);
}

// Function to check if a string is a valid number
bool isNumber(String str) {
  if (str.length() == 0) return false;
  
  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isDigit(str[i]) && str[i] != '.' && str[i] != '-') return false;
  }
  return true;
}

