#include "BluetoothSerial.h"

// Bluetooth setup
BluetoothSerial SerialBT;
String device_name = "ESP32-BT-Slave";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it.
#endif

void setup() {
  Serial.begin(9600); // UART communication with Arduino
  SerialBT.begin(device_name); // Initialize Bluetooth
  Serial.printf("Bluetooth device \"%s\" is ready. Pair with your device.\n", device_name.c_str());
}

void loop() {
  // Receive data from Arduino and forward to Bluetooth
  if (Serial.available()) {
    String receivedData = Serial.readStringUntil('\n');
    if (!receivedData.isEmpty()) {
      Serial.println("Received from Arduino:");
      Serial.println(receivedData);
      SerialBT.println(receivedData); // Forward to Bluetooth
    }
  }

  // Optional: Receive data from Bluetooth and forward to Arduino
  if (SerialBT.available()) {
    String btData = SerialBT.readStringUntil('\n');
    if (!btData.isEmpty()) {
      Serial.println("Received from Bluetooth:");
      Serial.println(btData);
      Serial.println(btData); // Forward to Arduino
    }
  }

  delay(10); // Small delay for stability
}
