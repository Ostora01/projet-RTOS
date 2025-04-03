#define RECEIVER_RXD_PIN 26  // Connect to sender's TX
#define RECEIVER_TXD_PIN 27  // Connect to sender's RX

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Initialize UART for communication with other ESP
  Serial2.begin(115200, SERIAL_8N1, RECEIVER_RXD_PIN, RECEIVER_TXD_PIN);
  
  Serial.println("ESP Arduino Receiver started");
}

void loop() {
  // Check if data is available from the other ESP
  if (Serial2.available()) {
    String receivedData = Serial2.readStringUntil('\n');
    
    // Parse the data if it's in the expected format
    if (receivedData.startsWith("SENSOR:")) {
      String valueStr = receivedData.substring(7);  // Extract the value part
      float sensorValue = valueStr.toFloat();
      
      // Print the received data
      Serial.print("Received sensor value: ");
      Serial.println(sensorValue);
      
      // Process the sensor value as needed
      // For example, if you want to trigger something when the value exceeds a threshold
      if (sensorValue > 50.0) {
        Serial.println("Sensor value exceeds threshold!");
      }
    }
  }
  
  // Small delay to prevent CPU hogging
  delay(10);
}