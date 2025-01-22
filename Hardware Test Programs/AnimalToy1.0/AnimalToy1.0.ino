#include <Arduino_LSM6DS3.h>

const int buzzerPin = 3; // Pin where the buzzer is connected

void setup() {
  Serial.begin(9600);
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.println("IMU initialized!");

  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  float x, y, z;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    
    // Calculate the magnitude of acceleration
    float magnitude = sqrt(x * x + y * y + z * z);

    // Check if the magnitude exceeds a threshold (indicating a shake)
    if (magnitude > 2.0) { // You can adjust the threshold value
      tone(buzzerPin, 1000); // Produce a sound at 1000 Hz
      delay(500); // Sound duration
      noTone(buzzerPin);
    }
  }
  delay(100); // Adjust this delay as needed
}
