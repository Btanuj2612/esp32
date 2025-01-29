#include <WiFi.h>
#include <WiFiUdp.h>

// Wi-Fi Credentials
const char *ssid = "NARZO 70x 5G";     
const char *password = "abcdefghij";    

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
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi!");

  // Start UDP Communication
  udp.begin(udpPort);
  Serial.println("UDP initialized, ready to send data...");
}

void loop() {
  // Check if data is available from Arduino via default Serial
  if (Serial.available()) {
    String receivedData = Serial.readStringUntil('\n');  
    receivedData.trim();  

    if (receivedData.length() > 0) {
      Serial.println("Received from Arduino: " + receivedData);  

      // Send received data over UDP
      udp.beginPacket(broadcastIp, udpPort);
      udp.print(receivedData);
      udp.endPacket();

      Serial.println("Data sent to UDP: " + receivedData);  
    }
  }

  delay(10);  
}
