#include <Arduino_LSM6DS3.h>
#include <DFPlayerMini_Fast.h>

DFPlayerMini_Fast myDFPlayer;
const int transistorPin = 3; // Transistor control pin

float ax, ay, az;
float shakeThreshold = 2.0; // Adjust this value to set the shake sensitivity

void setup() {
  Serial.begin(9600);
  
  // Set the transistor control pin as output
  pinMode(transistorPin, OUTPUT);
  digitalWrite(transistorPin, LOW); // Ensure the transistor is off initially

  // Initialize LSM6DS3 (onboard accelerometer)
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  
  // Initialize DFPlayer Mini
  Serial1.begin(9600);
  if (!myDFPlayer.begin(Serial1)) {
    Serial.println("DFPlayer Mini initialization failed");
    while (1);
  }
  myDFPlayer.volume(25); // Set volume (0 to 30)
}

void loop() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);

    // Calculate the magnitude of the acceleration vector
    float acceleration = sqrt(ax * ax + ay * ay + az * az);

    // Check if the device is shaken
    if (acceleration > shakeThreshold) {
      Serial.println("Shake detected");
      
      // Power on the speaker
      digitalWrite(transistorPin, HIGH);
      
      myDFPlayer.play(1); // Play the first sound file on the SD card

      // Adjust this delay to the length of your audio file in milliseconds
      delay(2000); // Delay to allow the audio file to play
      
      // Power off the speaker
      digitalWrite(transistorPin, LOW);
    }
  }

  delay(100);
}
