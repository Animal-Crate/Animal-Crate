#include "SPI.h"
#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ILI9341.h>     // Hardware-specific library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions

const int freq = 5000; // PWM frequency        
const int channel = 1;  // PWM channel      
const int resolution = 8;  // PWM channel    
int brightnessLevels[] = {32, 64, 96, 128, 160, 192, 232, 255};  
int brightness = 128;  // Initial brightness (50%)        
int levelIndex = 0;  // Index to track brightness level   

const char* animalNames[] = {"Pig", "Chicken", "Dog", "Horse", "Cow", "Sheep"};
const char* imageFiles[] = {"3pig3.bmp", "3chicken4.bmp", "3dog2.bmp", "3horse1.bmp", "3cow3.bmp", "3sheep1.bmp"};

// Global interrupt flags for buttons
volatile bool backPressed = false;
volatile bool nextPressed = false;
volatile bool navigatePressed = false;
volatile bool brightnessPressed = false;

// TFT display and SD card
//#define TFT_DC 2  
//#define TFT_CS 15  
#define SD_CS 5  

#define TFT_DC 2
#define TFT_CS 15
#define TFT_MOSI 23
#define TFT_CLK 18
#define TFT_RST 4
#define TFT_MISO 19

// PWM Backlight Control
#define BCK_LIGHT 13  

// Button Pins
#define BTN_PIN1 22  // Back
#define BTN_PIN2 14  // Next
#define BTN_PIN3 12  // Navigate
#define BTN_PIN4 33  // Brightness control

// Button States (for edge detection)
bool lastButtonState1 = HIGH;
bool lastButtonState2 = HIGH;
bool lastButtonState3 = HIGH;
bool lastButtonState4 = HIGH;

SdFat SD;
Adafruit_ImageReader reader(SD);
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

int selectedOption = 0;
const int totalOptions = 2;
enum Mode { MENU, LEARNING, GAME };
Mode currentMode = MENU;

// Interrupt Service Routines for buttons
void IRAM_ATTR backISR() { backPressed = true; }
void IRAM_ATTR nextISR() { nextPressed = true; }
void IRAM_ATTR navigateISR() { navigatePressed = true; }
void IRAM_ATTR brightnessISR() { brightnessPressed = true; }

void setup() {
  Serial.begin(115200);

  SPI.begin(TFT_CLK, TFT_MISO, TFT_MOSI, SD_CS);

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  // pinMode(TFT_CLK, OUTPUT);
  // pinMode(TFT_MOSI, OUTPUT);
  // pinMode(TFT_MISO, OUTPUT);

  tft.begin();
  tft.setRotation(1); //Landscape mode

  // tft.fillScreen(ILI9341_BLUE);
  // delay(3000);
  
  if (!SD.begin(SD_CS, SD_SCK_MHZ(10))) {
      Serial.println(F("SD initialization failed!"));
      for (;;);
  }

  // Set up buttons as input with pull-up and attach interrupts
  pinMode(BTN_PIN1, INPUT_PULLUP);
  pinMode(BTN_PIN2, INPUT_PULLUP);
  pinMode(BTN_PIN3, INPUT_PULLUP);
  pinMode(BTN_PIN4, INPUT_PULLUP);

  
  
  attachInterrupt(BTN_PIN1, backISR, FALLING);
  attachInterrupt(BTN_PIN2, nextISR, FALLING);
  attachInterrupt(BTN_PIN3, navigateISR, FALLING);
  attachInterrupt(BTN_PIN4, brightnessISR, FALLING);

  ledcAttachChannel(BCK_LIGHT, freq, resolution, channel);
  ledcWrite(BCK_LIGHT, brightness); // set initial brightness to half 50%

  randomSeed(analogRead(34)); // set it to un used pin
  drawMenu();
}

void loop() {
  if (currentMode == MENU || currentMode == GAME) {
      if (backPressed) {
          backPressed = false;
          drawMenu();
      }
      if (nextPressed) {
          nextPressed = false;
          selectOption();
      }
      if (navigatePressed) {
          navigatePressed = false;
          moveSelector();
      }
      if (brightnessPressed) {
          brightnessPressed = false;
          brightnessControl();
      }
  }
  if (currentMode == LEARNING) {
      learningMode();
  }
}


void drawMenu() {
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

void moveSelector() {
  selectedOption = (selectedOption + 1) % totalOptions;
  drawMenu();
}

void brightnessControl() {
  levelIndex = (levelIndex + 1) % 8; // Cycle through brightness levels
  brightness = brightnessLevels[levelIndex]; // Update PWM value
  ledcWrite(BCK_LIGHT, brightness);
}

void selectOption() {
  tft.fillScreen(ILI9341_BLACK);
  if (selectedOption == 0) {
      currentMode = LEARNING;
      learningMode();
  } else {
      currentMode = GAME;
      gameMode();
  }
}

// Learning mode
void learningMode() {
  currentMode = LEARNING;
  backPressed = false;  

  while (!backPressed) {
      if (brightnessPressed) {
          brightnessControl();
          brightnessPressed = false;
      }
      
      tft.fillScreen(ILI9341_BLACK);
      tft.setCursor(0, 0);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_WHITE);
      tft.println("Learning Mode");
      tft.setCursor(0, 20);
      tft.println("Place animal");

      int detectedAnimal = random(6); // sensor data input here

      const char* animalNames[] = {"Pig", "Chicken", "Dog", "Horse", "Cow", "Sheep"};
      const char* imageFiles[] = {"3pig3.bmp", "3chicken4.bmp", "3dog2.bmp", "3horse1.bmp", "3cow3.bmp", "3sheep1.bmp"};

      tft.setCursor(50, 40);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_WHITE);
      tft.println(animalNames[detectedAnimal]);
      reader.drawBMP(imageFiles[detectedAnimal], tft, 10, 60);
      // play sound here
      Serial.println(animalNames[detectedAnimal]);

      delay(3000);
  }
  drawMenu();
}

void gameMode() {
  int score = 0;
  int availableAnimals[6] = {0, 1, 2, 3, 4, 5};  // List of available animals
  int remaining = 6;  // Total animals left in the game

  backPressed = false;  // Reset back button flag before game starts

  while (remaining > 0) {
      int randIndex = random(remaining);
      int targetAnimal = availableAnimals[randIndex];  // Select a random animal

      tft.fillScreen(ILI9341_BLACK);
      tft.setCursor(0, 0);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_WHITE);
      tft.println("Game Mode");

      // **Play animal sound here** (Add actual sound function)
      delay(1000);

      tft.setCursor(0, 20);
      tft.println("Place the correct animal");

      int detectedAnimal = -1;

      while (true) {
          // **Simulated sensor reading** (Replace with actual sensor input)
          detectedAnimal = random(6);

          // **Allow user to exit game mode**
          if (backPressed) {
              backPressed = false;  // Reset flag
              drawMenu();  
              return;
          }

          if (detectedAnimal == targetAnimal) {
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
              tft.println(animalNames[detectedAnimal]);
              reader.drawBMP(imageFiles[detectedAnimal], tft, 10, 60);
              // play sound here
              delay(1000);
              break;  // Move to next round
          } else {
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
