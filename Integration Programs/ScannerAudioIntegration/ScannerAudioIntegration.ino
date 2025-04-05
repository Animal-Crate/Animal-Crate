/* -------------------------------------------------- */
/* NAME:    CARD-READ & AUDIO TEST                    */
/* AUTHOR:  John Willis, Shunsuke Morosa              */
/*                                                    */
/*                                                    */
/* PURPOSE: To read MiFare Ultralight Cards           */
/* and display the information on the ST7735          */
/* LED display from Adafruit. Playing Audio for       */
/* specific cards.                                    */
/* -------------------------------------------------- */

/* Library Includes */
// - Standard Libraries
#include <stdint.h>                 // Library for data types.
#include <SPI.h>                    // Arduino library for SPI controls.
#include <MFRC522.h>                // Library for RC522 scanning module.
#include "SoftwareSerial.h"         // Library for Software Communication between devices.
#include "DFRobotDFPlayerMini.h"    // Library for DFPlayer Mini module.

/* Definitions for Pins */
// - Scanner and Display
#define RC522_SS_PIN      5   // SS/SDA Pin Location for RC522
#define RC522_RST_PIN     0   // RST Pin Location for RC522
// - Audio System
#define VOLUME_UP         33  // Pin Location for Button Input
#define VOLUME_DOWN       32  // Pin Location for Button Input
#define PIN_MP3_TX        26  // Connect to Module TX Data
#define PIN_MP3_RX        27  // Connect to Module RX Data
// - Animal Definitions
#define ANIMAL_1          1   // Cow is on the 001.mp3 file.
#define ANIMAL_2          2   // Dog is on the 002.mp3 file.
#define ANIMAL_3          3   // Sheep is on the 003.mp3 file.
#define ANIMAL_4          4   // Horse is on the 004.mp3 file.
#define ANIMAL_5          5   // Chicken is on the 005.mp3 file.
#define ANIMAL_6          6   // Pig is on the 006.mp3 file.

/* Enumerators */
enum animal_ID : uint8_t    // File selection integer enumerator. This acts to display the best course of understanding to users unaffiliated with programming.
{
  COW = ANIMAL_1,
  DOG = ANIMAL_2,
  SHEEP = ANIMAL_3,
  HORSE = ANIMAL_4,
  CHICKEN = ANIMAL_5,
  PIG = ANIMAL_6
};
animal_ID currentFile = COW;  // DEFINES THE FILES SELECTION!

/* Card Definitions */
char card_1[] = {"4330DFE1"};     // THESE ARE TESTING CARDS! THESE ARE NOT THE SAME!
char card_2[] = {"E31FCE13"};     // THESE ARE TESTING CARDS! THESE ARE NOT THE SAME!

/* Class Creations & Associations */
MFRC522 rfid(RC522_SS_PIN, RC522_RST_PIN);                                            // Creating class for RC522 module
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);                                // Creating class for Serial Connection for RX/TX Pins
DFRobotDFPlayerMini player;                                                           // Creating class for DFPlayer Mini module

/* Global Variables */
volatile bool isPlaying = 0;                                                          // Create a boolean for whether the audio player is playing audio.
volatile bool upPressed = 0;                                                          // Create a boolean for labeling when the up button has been pressed, to prevent debounce.
volatile bool downPressed = 0;                                                        // Create a boolean for labeling when the down button has been pressed, to prevent debounce.
volatile uint8_t volume = 21;                                                         // Volume Control Integer. Initial Volume is 15, volume ranges from 0-30.
const char noTrigger[] = {"Volume is currently: "};                                   // Create a string for outputting to serial when nothing is pressed. [Interrupts Use]
const char upTrigger[] = {"Volume was Increased to: "};                               // Create a string for outputting to serial when up is pressed. [Interrupts Use]
const char downTrigger[] = {"Volume was Decreased to: "};                             // Create a string for outputting to serial when down is pressed. [Interrupts Use]
const char bothTrigger[] = {"Volume was Changed to: "};                               // Create a string for outputting to serial when both are pressed. [Interrupts Use]
char uid[32];                                                                         // Create a 32 byte array for UID (well over what it needs).

/* Interrupt Service Routines for button presses. */
/// <summary>
///   Increases the volume of the DFPlayer Mini for the Speakers.
///   **  Does not change if volume up button was previously pressed.
/// </summary>
void IRAM_ATTR ISR_volumeUp()
{
  if (volume < 30 && !upPressed) {
    volume = volume + 3;  // Maximum volume is 30, check underneath.
    volume = volume > 30 ? 30 : volume; // Ensures volume is within the dedicated range. Redundancy.
    upPressed = true;
  }
}

/// <summary>
///   Decreases the volume of the DFPlayer Mini for the Speakers.
///   **  Does not change if volume down button was previously pressed.
/// </summary>
void IRAM_ATTR ISR_volumeDown()
{
  if (volume > 0 && !downPressed) {
      volume = volume - 3;  // Ninimum volume is 0, check underneath.
      volume = volume < 0 ? 0 : volume; // Ensures volume is within the dedicated range. Redundancy.
      downPressed = true;
    }
}

void setup()
{
  /* Initializations */
  Serial.begin(9600);                               // Initialize the serial communication with the computer.
  SPI.begin();                                      // Initialization for the SPI bus for slave devices.
  rfid.PCD_Init();                                  // Initialization for the RC522 module.
  softwareSerial.begin(9600);                       // Initialization for the DFPlayer Mini module.

  /* Line Clears */
  Serial.println();                                       // Clear the line for beginning of serial output.
  Serial.println();                                       // Add an additional cleared line for cleanliness.
  Serial.println(F("<RC522 Successfully Setup>"));        // Display initialization success for RC522 to serial window.

  /* Assign Button Inputs */
  pinMode(VOLUME_UP, INPUT_PULLUP);                       // "Volume Up" button
  attachInterrupt(VOLUME_UP, ISR_volumeUp, RISING);       // Attach interrupt to Volume Up button.
  pinMode(VOLUME_DOWN, INPUT_PULLUP);                     // "Volume Down" button
  attachInterrupt(VOLUME_DOWN, ISR_volumeDown, RISING);   // Attach interrupt to Volume Down button.
  Serial.println(F("<Buttons Configured>"));              // Print that the button is configured.

  /* Start Communication with DFPlayer Mini */
  if (player.begin(softwareSerial))
  {
    Serial.println("<DFPlayer Mini Successfully Setup>");

    // Set initial volume
    player.volume(volume);
  }
  else
    Serial.println("<DFPlayer Mini Failed Setup>");

  /* Confirmations for data, display in terminal. */
  Serial.println(F("<Program Successfully Setup>"));                                  // State success of program setting up.
  Serial.println(F("This program is intended to scan Mifare Ultralight Cards."));     // Specify acceptable card types in serial window.
  Serial.println(F("Scan card to see data as follows: UID and PICC Type."));          // Give directions in Serial Window for what to scan.
  Serial.println();                                                                   // Add another clear line for cleanliness.

  for (uint8_t i = 0; i < 10; i++)  // Set default values to 0.
    uid[i] = 0;
}

void loop()
{
  checkVolume();
  // Check if the player has finished playing the current file
  if (player.available()) {
    int type = player.readType();
    if (type == DFPlayerPlayFinished) {
      Serial.println("Track finished!");
      isPlaying = false;
    }
  }

  /* Test if card is still there. */
  if (!rfid.PICC_IsNewCardPresent())     // Check if the card is present. If this fails, return to start of loop.
  {
    Serial.println(F("Take card away from sensor and replace. (Delaying 300 ms)"));
    delay(300);
    return;
  }
  
  if (!rfid.PICC_ReadCardSerial())       // Verify a successful read. If this fails, return to start of loop.
  {
    Serial.println(F("Card Read Failure, please try again. (Delaying 300 ms)"));
    delay(300);
    return;
  }

  convertByte(rfid.uid.uidByte, rfid.uid.size);
  playAudio();
  Serial.print(F("Card UID (String): "));
  Serial.println(uid);

  /* Print Card UID in HEX */
  Serial.print(F("Card UID (Hex):"));
  printHex(rfid.uid.uidByte, rfid.uid.size);  // Print in HEX
  Serial.println();

  /* Print Card UID in DEC */
  Serial.print(F("Card UID (Dec):"));
  printDec(rfid.uid.uidByte, rfid.uid.size);  // Print in DEC
  Serial.println();

  /* Read Card PICC Type and print and display. */
  Serial.print(F("PICC Type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  delay(2000); // Delay 2 seconds after reading information to prevent reads again.
}

/* Functions for playing Audio and setting Enumerators. */
/// <summary>
///   Plays the audio based on scanner reading successfully.
///   **  Prints out "Playing Sound" in Serial Connection.
/// </summary>
void playAudio()
{
  if (!strcmp(card_1, uid)) currentFile = COW;
  else if (!strcmp(card_2, uid)) currentFile = DOG;
  else currentFile = PIG;

  if (!isPlaying) // Only works if the audio player is not currently playing an audio, to prevent errors.
  {
    player.volume(volume);

    Serial.println("Playing sound");
    player.play(currentFile);
    isPlaying = true;
    delay(200); // Debounce delay
  }
}

/// <summary>
///   Checks the volume condition and resets booleans for the button presses.
///   **  Prints out volume in Serial connection.
/// </summary>
void checkVolume()
{
  if (upPressed || downPressed)
  {
    if (!downPressed) Serial.print(F(upTrigger));
    else if (!upPressed) Serial.print(F(downTrigger));
    else Serial.print(F(bothTrigger));

    upPressed = false;
    downPressed = false;
    Serial.println(volume);
  }
}

/* Functions for dumping information from byte arrays, specifically for RC522 module. */
/// <summary>
///   Converts a byte array to characters for use in other functions.
///   **  Modifies global variable for "uid".
/// </summary>
void convertByte(byte *buffer, byte bufferSize)
{
  for (uint8_t i = 0; i < bufferSize; i++)
    sprintf(&uid[i*2], "%02X", rfid.uid.uidByte[i]);
}

/// <summary>
///   Print to Serial window the bytes from a buffer with spaces for each byte in
///   HEXadecimal format.
///   **  Prints out data into Serial Connection.
/// </summary>
void printHex(byte *buffer, byte bufferSize)            // Print information in Hex.
{
  for (uint8_t i = 0; i < bufferSize; i++) 
    if (buffer[i] != NULL)
    {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
    }
}

/// <summary>
///   Print to Serial window the bytes from a buffer with spaces for each byte in
///   DECimal format.
///   **  Prints out data into Serial Connection.
/// </summary>
void printDec(byte *buffer, byte bufferSize)            // Print information in Dec.
{
  for (uint8_t i = 0; i < bufferSize; i++)
    if (buffer[i] != NULL)
    {
      Serial.print(' ');
      Serial.print(buffer[i], DEC);
    }
}