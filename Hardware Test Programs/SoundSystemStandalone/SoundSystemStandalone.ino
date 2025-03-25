#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Use pins 26 and 27 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 26; // Connects to module's RX
static const uint8_t PIN_MP3_RX = 27; // Connects to module's TX
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

// Button pins
const int buttonVolumeUp = 4;   // Pin for "Volume Up" button
const int buttonVolumeDown = 5; // Pin for "Volume Down" button
const int buttonPlay = 21;       // Pin for "Play Sound" button
bool isPlaying = false;  // Flag to indicate if sound is playing
// Create the Player object
DFRobotDFPlayerMini player;

int volume = 15;         // Initial volume (0 to 30)
int currentFile = 3;     // Start with the first MP3 file

void setup() {
  // Initialize USB serial port for debugging
  Serial.begin(9600);
  // Initialize serial port for DFPlayer Mini
  softwareSerial.begin(9600);

  // Configure button pins
  pinMode(buttonVolumeUp, INPUT_PULLUP);   // "Volume Up" button
  pinMode(buttonVolumeDown, INPUT_PULLUP); // "Volume Down" button
  pinMode(buttonPlay, INPUT_PULLUP);       // "Play Sound" button

  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial)) {
    Serial.println("DFPlayer Mini initialized successfully");

    // Set initial volume
    player.volume(volume);
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
}

void loop() {
  // Check if "Volume Up" button is pressed
  if (digitalRead(buttonVolumeUp) == HIGH) { // Button pressed (LOW due to pull-up)
    if (volume < 30) { // Max volume is 30
      volume = volume + 3;
      player.volume(volume);
      Serial.print("Volume increased to: ");
      Serial.println(volume);
      delay(200); // Debounce delay
    }
  }

  // Check if "Volume Down" button is pressed
  if (digitalRead(buttonVolumeDown) == HIGH) { // Button pressed (LOW due to pull-up)
    if (volume > 0) { // Min volume is 0
      volume = volume - 3;
      player.volume(volume);
      Serial.print("Volume decreased to: ");
      Serial.println(volume);
      delay(200); // Debounce delay
    }
  }

 // Check if "Play Sound" button is pressed
  if (digitalRead(buttonPlay) == HIGH && !isPlaying) { // Button pressed (LOW due to pull-up) and not currently playing
    Serial.println("Playing sound");
    player.play(currentFile);
    isPlaying = true;
    delay(200); // Debounce delay
  }

  // Check if the player has finished playing the current file
  if (player.available()) {
    int type = player.readType();
    if (type == DFPlayerPlayFinished) {
      Serial.println("Track finished!");
      isPlaying = false;
    }
  }

  // Small delay to avoid excessive polling
  delay(300);
}
