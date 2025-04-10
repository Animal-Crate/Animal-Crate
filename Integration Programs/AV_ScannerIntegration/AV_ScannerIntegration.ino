/* -------------------------------------------------- */
/* NAME:    CARD-READ, FEEDBACK AV                    */
/* AUTHOR:  John Willis, Shunsuke Morosa,             */
/*            Yoseph Tefera                           */
/*                                                    */
/*                                                    */
/* PURPOSE: To read MiFare Ultralight Cards           */
/* and display the information on the ST7735          */
/* LED display from Adafruit. Playing Audio for       */
/* specific cards.                                    */
/* -------------------------------------------------- */

/* Library Includes */
#include <stdint.h>                 // Library for data types.
#include <SPI.h>                    // Arduino library for SPI controls.
#include <MFRC522.h>                // Library for RC522 scanning module.
#include "SoftwareSerial.h"         // Library for Software Communication between devices.
#include "DFRobotDFPlayerMini.h"    // Library for DFPlayer Mini module.
#include <Adafruit_GFX.h>           // Adafruit Core Graphics Library for Display module.
#include <Adafruit_ILI9341.h>       // Hardware-Specific Library for Display module.
#include <Adafruit_ImageReader.h>   // Adafruit Image Reading Functions Library for Display module.
#include <Adafruit_SPIFlash.h>      // SPI/QSPI Flash Library for reading functions.
#include <SdFat.h>                  // SD Card and FAT Functions Library for reading data and organization.

/* Definitions for Pins */
// - Scanner System
#define RC522_SS_PIN      21  // SS/SDA Pin Location for RC522
#define RC522_RST_PIN     0   // RST Pin Location for RC522
// - Audio System
#define VOLUME_UP         16  // Pin Location for Button Input - Volume Up
#define VOLUME_DOWN       32  // Pin Location for Button Input - Volume Down
#define PIN_MP3_TX        26  // Connect to Module TX Data - CONNECTS TO RX ON DFPLAYER MINI
#define PIN_MP3_RX        27  // Connect to Module RX Data - CONNECTS TO TX ON DFPLAYER MINI
// - Display System
#define TFT_DC            2   // Display DC Connection
#define TFT_CS            15  // Display Chip Select Pin Connection
#define SD_CS             5   // SD Chip Select Pin Connection
#define BCK_LIGHT         13  // Display Back Light Connection
#define BACK              22  // Pin Location for Button Input - Back
#define NEXT              14  // Pin Location for Button Input - Next
#define NAVIGATE          12  // Pin Location for Button Input - Navigate
#define BRIGHTNESS        33  // Pin Location for Button Input - Brightness  

/* Definitions for Constant Values */
// - Animal Definitions
#define ANIMAL_1          1     // Cow is on the 001.mp3 file.
#define ANIMAL_2          2     // Dog is on the 002.mp3 file.
#define ANIMAL_3          3     // Sheep is on the 003.mp3 file.
#define ANIMAL_4          4     // Horse is on the 004.mp3 file.
#define ANIMAL_5          5     // Chicken is on the 005.mp3 file.
#define ANIMAL_6          6     // Pig is on the 006.mp3 file.
// - Display Definitions
#define FREQ              5000  // PWM Frequency
#define CHANNEL           1     // PWM Channel
#define RESOLUTION        8     // PWM Channel Resolution
#define TOTALOPTIONS      2     // Total Options to Choose

/* Card Definitions */
char card_1[] = {"4330DFE1"};     // THESE ARE TESTING CARDS! THESE ARE NOT THE SAME!
char card_2[] = {"E31FCE13"};     // THESE ARE TESTING CARDS! THESE ARE NOT THE SAME!

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

enum mode : uint8_t         // Mode selection integer enumerator. This selects the current mode that algorithm is in and whether to display the menu.
{
  MENU,
  LEARNING,
  GAME
};

/* Class/Enumerator Creations & Associations */
// - Classes
MFRC522 rfid(RC522_SS_PIN, RC522_RST_PIN);                // Creating class for RC522 module
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);    // Creating class for Serial Connection for RX/TX Pins
DFRobotDFPlayerMini player;                               // Creating class for DFPlayer Mini module
SdFat SD;                                                 // Creating class for SD Card module
Adafruit_ImageReader reader(SD);                          // Creating class for ImageReader module
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);  // Creating class for Display module
// - Enumerators
animal_ID currentFile = COW;  // DEFINES THE FILES SELECTION!
mode currentMode = MENU;      // DEFINES THE GAMEMODE

/* Global Variables */
// - Button Booleans    :   Creates booleans for labeling when buttons are pressed to prevent debounce.
volatile bool upPressed = 0;            // Volume Up Button
volatile bool downPressed = 0;          // Volume Down Button
volatile bool backPressed = 0;          // Navigate Back Button
volatile bool nextPressed = 0;          // Navigate Next Button
volatile bool navigatePressed = 0;      // Navigation (Menu) Button
volatile bool brightnessPressed = 0;    // Brightness Control Button
// - Constant Characters/Integers
const char noTrigger[] = {"Volume is currently: "};                                                                 // Create a string for outputting to serial when nothing is pressed. [Interrupts Use]
const char upTrigger[] = {"Volume was Increased to: "};                                                             // Create a string for outputting to serial when up is pressed. [Interrupts Use]
const char downTrigger[] = {"Volume was Decreased to: "};                                                           // Create a string for outputting to serial when down is pressed. [Interrupts Use]
const char bothTrigger[] = {"Volume was Changed to: "};                                                             // Create a string for outputting to serial when both are pressed. [Interrupts Use]
const uint8_t brightnessLevels[] = {32, 64, 96, 128, 160, 192, 232, 255};                                           // Create an array of integers for brigthness levels.
const char* animalNames[] = {"Cow", "Dog", "Sheep", "Horse", "Chicken", "Pig"};                                     // Create an array of strings for animal names for printing.
const char* imageFiles[] = {"3cow3.bmp", "3dog2.bmp", "3sheep1.bmp", "3horse1.bmp", "3chicken4.bmp", "3pig3.bmp"};  // Create an array of strings for file names for images pertaining to animals.
// - Starting Values & Globals
volatile bool isPlaying = 0;            // Create a boolean for whether the audio player is playing audio.
volatile bool scanBacked = 0;           // Create a boolean for whether the scan has been backed or not.
volatile uint8_t volume = 21;           // Volume Control Integer. Initial Volume is 15, volume ranges from 0-30.
volatile uint8_t brightness = 128;      // Brightness Control Integer. Initial brightness is 128, brightness ranges from 32-255.
volatile uint8_t levelIndex = 3;        // Brightness Array Indexer.
volatile uint8_t selectedOption = 0;    // Option selector integer.
char uid[32];                           // Create a 32 byte array for UID (well over what it needs).

/* Interrupt Service Routines for button presses. */
/// <summary>
///   Increases the volume of the DFPlayer Mini for the Speakers.
///   **  Does not change if volume up button was previously pressed.
/// </summary>
void IRAM_ATTR ISR_volumeUp()
{
  if (volume < 30 && !upPressed)
  {
    volume = volume + 3;                // Maximum volume is 30, check underneath.
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
  if (volume > 0 && !downPressed)
  {
    volume = volume - 3;              // Ninimum volume is 0, check underneath.
    volume = volume < 0 ? 0 : volume; // Ensures volume is within the dedicated range. Redundancy.
    downPressed = true;
  }
}

/// <summary>
///   These two functions, ISR_BACK and ISR_NEXT, are simple checkstatements.
///   **  These do not execute if button was previously pressed.
///   **  These are condensed.
/// </summary>
void IRAM_ATTR ISR_BACK() { if (!backPressed) backPressed = true; }
void IRAM_ATTR ISR_NEXT() { if (!nextPressed) nextPressed = true; }

/// <summary>
///   Changes the selected option for the menu screen.
///   **  Does not change if the navigate button was previously pressed.
/// </summary>
void IRAM_ATTR ISR_NAVIGATE() 
{
  if (!navigatePressed)
  {
    selectedOption = (selectedOption + 1) % TOTALOPTIONS;
    navigatePressed = true;
  }
}

/// <summary>
///   Changes the brightness of the backlight on the LED Display.
///   **  Does not change if the brightness button was previously pressed.
/// </summary>
void IRAM_ATTR ISR_BRIGHTNESS()
{
  if (!brightnessPressed)
  {
    levelIndex = (levelIndex + 1) % RESOLUTION;
    brightness = brightnessLevels[levelIndex];
    brightnessPressed = true;
  }
}

void setup()
{
  /* Initializations */
  Serial.begin(9600);           // Initialize the serial communication with the computer.
  SPI.begin();                  // Initialization for the SPI bus for slave devices.
  rfid.PCD_Init();              // Initialization for the RC522 module.
  softwareSerial.begin(9600);   // Initialization for the DFPlayer Mini module.
  tft.begin();                  // Initialization for display module.
  tft.setRotation(1);           // Set display module to landscape mode.

  /* Line Clears */
  Serial.println();                                       // Clear the line for beginning of serial output.
  Serial.println();                                       // Add an additional cleared line for cleanliness.
  Serial.println(F("<RC522 Successfully Setup>"));        // Display initialization success for RC522 to serial window.

  /* Assign Button Inputs */
  // - Button Inputs
  pinMode(VOLUME_UP, INPUT_PULLUP);     // "Volume Up" button
  pinMode(VOLUME_DOWN, INPUT_PULLUP);   // "Volume Down" button
  pinMode(BACK, INPUT_PULLUP);          // "Back" button
  pinMode(NEXT, INPUT_PULLUP);          // "Next" button
  pinMode(NAVIGATE, INPUT_PULLUP);      // "Navigate" button
  pinMode(BRIGHTNESS, INPUT_PULLUP);    // "Brightness" button
  // - Button Interrupts
  attachInterrupt(VOLUME_UP, ISR_volumeUp, FALLING);       // Attach interrupt to Volume Up button.
  attachInterrupt(VOLUME_DOWN, ISR_volumeDown, FALLING);   // Attach interrupt to Volume Down button.
  attachInterrupt(BACK, ISR_BACK, FALLING);               // Attach interrupt to Back button.
  attachInterrupt(NEXT, ISR_NEXT, FALLING);               // Attach interrupt to Next button.
  attachInterrupt(NAVIGATE, ISR_NAVIGATE, FALLING);       // Attach interrupt to Navigate button.
  attachInterrupt(BRIGHTNESS, ISR_BRIGHTNESS, FALLING);   // Attach interrupt to Brightness button.
  // - Confirmation
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

  /* Start Communication with SD Card */
  if (!SD.begin(SD_CS, SD_SCK_MHZ(10))) // Initialization for SD Card.
  {
    Serial.println(F("<SD Initialization Failed!>"));
    for (;;);
  }
  else
    Serial.println(F("<SD Card Successfully Setup>"));      // Display initialization success for SD Card to serial window.

  /* Start Display Setup */
  ledcAttachChannel(BCK_LIGHT, FREQ, RESOLUTION, CHANNEL);  // Setup backlight.
  ledcWrite(BCK_LIGHT, brightness);                           // Set initial brightness to 50%.
  Serial.println(F("<LED Screen Successfully Setup>"));     // Initialization for display.

  /* Confirmations for data, display in terminal. */
  Serial.println(F("<Program Successfully Setup>"));                                  // State success of program setting up.
  Serial.println(F("This program is intended to scan Mifare Ultralight Cards."));     // Specify acceptable card types in serial window.
  Serial.println(F("Scan card to see data as follows: UID and PICC Type."));          // Give directions in Serial Window for what to scan.
  Serial.println();                                                                   // Add another clear line for cleanliness.

  for (uint8_t i = 0; i < 10; i++)  // Set default values to 0.
    uid[i] = 0;
  
  /* Initialize Program */
  randomSeed(analogRead(34)); // set it to un used pin
  drawMenu();
}

void loop()
{
  if (currentMode == MENU || currentMode == GAME)
  {
    if (backPressed) { backPressed = false; drawMenu(); }
    if (nextPressed) { nextPressed = false; selectOption(); }
    if (navigatePressed) { navigatePressed = false; moveSelector(); }
    if (brightnessPressed) { brightnessPressed = false; brightnessControl(); }

    checkVolume();
    checkAudio();
  }
  else if (currentMode == LEARNING) { learningMode(); }

  delay(500);
}

/* ------------------------------------------------------------ */
/* Functions for Display Control and Algorithm/Gamemode Control */
/* ------------------------------------------------------------ */

/// <summary>
///   Draws the entire screen, filling with appropriate text for options.
///   **  This is a display function, and interacts with only the display.
/// </summary>
void drawMenu()
{
  currentMode = MENU;
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(50, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(4);
  tft.println("Main Menu");
  // Draw Option 1
  tft.setCursor(20, 50);
  tft.setTextColor(selectedOption == 0 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.setTextSize(3);
  tft.println("> Learning Mode");
  // Draw Option 2
  tft.setCursor(20, 90);
  tft.setTextColor(selectedOption == 1 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.setTextSize(3);
  tft.println("> Game Mode");
  // Instructions at the bottom
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(10, 140);
  tft.println("Press Navigate to move");
  tft.setCursor(10, 180);
  tft.println("Press Next to select");
  tft.setCursor(10, 220);
  tft.println("Press Back to return");
}

/// <summary>
///   Update Functions for Display and Menu.
///   Controls Brightness and Calls other Update Functions.
///   **  These functions update display.
/// </summary>
void moveSelector() { drawMenu(); }
void brightnessControl() { ledcWrite(BCK_LIGHT, brightness); }

/// <summary>
///   Updates the current mode option and calls the function
///   based on the selected option.
///   **  This function sets the algorithm selection.
/// </summary>
void selectOption()
{
  tft.fillScreen(ILI9341_BLACK);
  
  if (selectedOption == 0)
  {
    currentMode = LEARNING;
    learningMode();
  } 
  else
  {
    currentMode = GAME;
    gameMode();
  }
}

/// <summary>
///   This function runs the Learning Mode for active feedback on pieces.
///   ** This is an algorithm implemented in a separate loop from main.
/// </summary>
void learningMode()
{
  currentMode = LEARNING;
  backPressed = false;  

  while (!backPressed)
  {
    if (backPressed) { backPressed = false; drawMenu(); return;}
    if (brightnessPressed) { brightnessPressed = false; brightnessControl(); }

    checkVolume();
    checkAudio();
      
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_WHITE);
    tft.println("Learning Mode");
    tft.setCursor(0, 20);
    tft.println("Place animal");

    checkCardScan();
    if (scanBacked)
    {
      scanBacked = false;
      return;
    }

    tft.setCursor(50, 40);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_WHITE);
    tft.println(animalNames[currentFile - 1]);
    reader.drawBMP(imageFiles[currentFile - 1], tft, 10, 60);
    
    playAudio();
    
    Serial.println(animalNames[currentFile - 1]);

    delay(3000);
  }
  drawMenu();
}

/// <summary>
///   This runs the Game Mode for testing pieces and playing audio/pictures prior to scan.
///   **  This is an algorithm implemented separate from the main loop.
/// </summary>
void gameMode()
{
  uint8_t score = 0;
  uint8_t availableAnimals[6] = {0, 1, 2, 3, 4, 5};  // List of available animals
  uint8_t remaining = 6;  // Total animals left in the game

  backPressed = false;  // Reset back button flag before game starts

  while (remaining > 0)
  {
    uint8_t randIndex = random(remaining);
    uint8_t targetAnimal = availableAnimals[randIndex];  // Select a random animal

    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_WHITE);
    tft.println("Game Mode");

    switch(randIndex)
    {
      case 0: currentFile = COW; break;
      case 1: currentFile = DOG; break;
      case 2: currentFile = SHEEP; break;
      case 3: currentFile = HORSE; break;
      case 4: currentFile = CHICKEN; break;
      case 5: currentFile = PIG; break;
      default: currentFile = COW; break;
    }
    playAudio();
    
    delay(1000);

    tft.setCursor(0, 20);
    tft.println("Place the correct animal");

    while (true)
    {
      if (backPressed) { backPressed = false; drawMenu(); return;}
      if (brightnessPressed) { brightnessPressed = false; brightnessControl(); }

      checkVolume();
      checkAudio();
      checkCardScan();

      if (currentFile == targetAnimal)
      {
        // **Correct answer: update score and remove used animal**
        score++;
        availableAnimals[randIndex] = availableAnimals[remaining - 1];  // Replace with last element
        remaining--;  // Reduce available options

        //tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(40, 40);
        tft.setTextSize(2);
        tft.setTextColor(ILI9341_GREEN);
        tft.println("Correct!");
        tft.setCursor(160, 40);
        tft.println(animalNames[currentFile - 1]);
        reader.drawBMP(imageFiles[currentFile - 1], tft, 10, 60);
        // play sound here
        delay(1000);
        break;  // Move to next round
      }
      else
      {
        // **Incorrect answer: Show "Try Again" message**
        //tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(40, 40);
        tft.setTextSize(2);
        tft.setTextColor(ILI9341_RED);
        tft.println("Try Again");
        delay(1000);
        
        // **Re-display the prompt**
        tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(0, 0);
        tft.setTextSize(2);
        tft.setTextColor(ILI9341_WHITE);
        tft.println("Game Mode");
        tft.setCursor(0, 20);
        tft.println("Place the correct animal");
      }

      delay(500);  // Prevent rapid detection spam
    }
  }

  // **Game Over: Display final score and return to menu**
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.println("Game Over");
  tft.setCursor(0, 20);
  tft.print("Score: ");
  tft.println(score);
  delay(3000);
  drawMenu();
}

/* ---------------------------------------------------- */
/* Functions for playing Audio and setting Enumerators. */
/* ---------------------------------------------------- */

/// <summary>
///   Sets the enumerator based on card data. This is called after reading a card.
///   **  This sets enumerator and requires other functions.
/// </summary>
void checkAnimal()
{
  if (!strcmp(card_1, uid)) currentFile = COW;
  else if (!strcmp(card_2, uid)) currentFile = DOG;
  else currentFile = PIG;
}

/// <summary>
///   Check the audio player availability and prints track success.
///   **  Prints out "Track Finished" in Serial Connection. 
/// </summary>
void checkAudio()
{
  if (player.available())
  {
    uint8_t type = player.readType();

    if (type == DFPlayerPlayFinished)
    {
      Serial.println("Track finished!");
      isPlaying = false;
    }
  }
  else
    Serial.println("Player not available! [DEBUG]");
}

/// <summary>
///   Plays the audio based on scanner reading successfully.
///   **  Prints out "Playing Sound" in Serial Connection.
/// </summary>
void playAudio()
{
  checkAudio();

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

/* ---------------------------------------------------------------------------------- */
/* Functions for dumping information from byte arrays, specifically for RC522 module. */
/* ---------------------------------------------------------------------------------- */

/// <summary>
///   Checks if a card is there, splitting into a loop from main loop until card is read.
///   **  This splits from loop until it reads from scanner.
/// </summary>
void checkCardScan()
{
  bool isValid = false; // Checks if the card is valid.
  bool isNew = false;   // Checks if the card is new.

  while (!isValid || !isNew)
  {
    if (backPressed)
    { 
      backPressed = false;
      drawMenu();
      scanBacked = true;
      return;
    }
    if (brightnessPressed) { brightnessPressed = false; brightnessControl(); }

    checkVolume();
    checkAudio();

    if (rfid.PICC_IsNewCardPresent())     // Check if the card is present. If this fails, return to start of loop.
      isNew = true;
    else
      Serial.println(F("Take card away from sensor and replace."));
    
    if (rfid.PICC_ReadCardSerial())       // Verify a successful read. If this fails, return to start of loop.
      isValid = true;
    else
      Serial.println(F("Card Read Failure, please try again."));

    if (isValid && isNew)
      break;
    else
    {
      isValid = false;
      isNew = false;
    }

    delay(300); // Delay 300 milliseconds to prevent repeat scans of same card.
  }

  updateUID();  // Update the UID after a successful scan, no matter what.
}

/// <summary>
///   Converts information, updates UID, and prints card information to Serial.
///   **  This prints information to Serial Connection.
/// </summary>
void updateUID()
{
  convertByte(rfid.uid.uidByte, rfid.uid.size);
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

  checkAnimal();
}

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