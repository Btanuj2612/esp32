#include <WiFi.h>
#include <WiFiUdp.h>

// Wi-Fi credentials
const char *ssid = "NARZO 70x 5G";        // Replace with your Wi-Fi SSID
const char *password = "abcdefghij"; // Replace with your Wi-Fi password

// UART communication with Arduino
HardwareSerial mySerial(1); // Use UART1 for communication with Arduino

WiFiUDP udp;
IPAddress broadcastIp(255, 255, 255, 255);  // Broadcast address (all devices in the network)
const unsigned int udpPort = 12345;  // UDP port to send data

void setup() {
  Serial.begin(9600);  // Debugging via Serial Monitor
  mySerial.begin(9600, SERIAL_8N1, 16, 17);  // UART setup (RX:16, TX:17 for ESP32)

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi!");

  // Start the UDP connection
  udp.begin(udpPort);  // UDP port to send data
  Serial.println("UDP initialized, sending data...");
}

void loop() {
  // Check if data is available on the Serial Monitor (input from user)
  if (Serial.available()) {
    String inputData = Serial.readStringUntil('\n');  // Read data from Serial Monitor
    if (inputData.length() > 0) {
      // Send received data over UDP
      udp.beginPacket(broadcastIp, udpPort);
      udp.print(inputData);  // Send data from Serial Monitor
      udp.endPacket();
      Serial.println("Data sent: " + inputData);  // Print to Serial Monitor for debugging
    }
  }

  delay(10);  // Small delay for stability
}
