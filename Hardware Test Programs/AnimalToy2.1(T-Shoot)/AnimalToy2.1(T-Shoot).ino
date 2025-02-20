#include <Arduino_LSM6DS3.h>
#include <DFPlayerMini_Fast.h>

DFPlayerMini_Fast myDFPlayer;
const int transistorPin = 3; // Transistor control pin

float ax, ay, az;
float shakeThreshold = 2.0; // Adjust this value to set the shake sensitivity

bool playing = false; // Track if a sound is currently playing

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
  delay(1000); // Allow some time for DFPlayer Mini to initialize
  if (!myDFPlayer.begin(Serial1)) {
    Serial.println("DFPlayer Mini initialization failed");
    while (1);
  }
  myDFPlayer.volume(25); // Set volume (0 to 30)
}

void resetDFPlayer() {
  myDFPlayer.reset();
  delay(1000); // Wait for reset to complete
  myDFPlayer.volume(25); // Restore volume
  Serial.println("DFPlayer Mini reset completed");
}

void loop() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);

    // Calculate the magnitude of the acceleration vector
    float acceleration = sqrt(ax * ax + ay * ay + az * az);

    // Check if the device is shaken and not currently playing
    if (acceleration > shakeThreshold && !playing) {
      Serial.println("Shake detected");

      // Power on the speaker
      digitalWrite(transistorPin, HIGH);

      myDFPlayer.play(1); // Play the first sound file on the SD card
      playing = true; // Set playing state to true

      // Wait for playback to complete
      while (myDFPlayer.isPlaying()) {
        // Keep the speaker powered while playing
        delay(100);
      }

      // Playback completed
      digitalWrite(transistorPin, LOW); // Power off the speaker
      playing = false; // Reset playing state
      Serial.println("Playback completed");

      // Reset DFPlayer Mini
      resetDFPlayer();
    }
  }

  delay(100);
}
