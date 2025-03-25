#include "SPI.h"
#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ILI9341.h>     // Hardware-specific library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions

const int freq = 5000;  // PWM frequency
const int channel = 1;  // PWM channel
const int resolution = 8;  // 8-bit resolution (0-255)
int brightnessLevels[] = {32, 64, 96, 128, 160, 192, 232, 255};  
int brightness = 128; // Initial brightness (50%)
int levelIndex = 0; // Index to track brightness level
int randomNum;

// TFT display and SD card
#define TFT_DC 2  
#define TFT_CS 5  
#define SD_CS 15  

// PWM Backlight Control
#define BCK_LIGHT 13  

// Button Pins
#define BTN_PIN1 22  // Back
#define BTN_PIN2 27  // Next
#define BTN_PIN3 17  // Navigate
#define BTN_PIN4 33  // Brightness control

// Button States (for edge detection)
bool lastButtonState1 = HIGH;
bool lastButtonState2 = HIGH;
bool lastButtonState3 = HIGH;
bool lastButtonState4 = HIGH;

SdFat SD;
Adafruit_ImageReader reader(SD);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

int selectedOption = 0;
const int totalOptions = 2;

void setup() {
  Serial.begin(115200);
  tft.begin(); // Initialize display
  tft.setRotation(1); // Landscape mode

  if (!SD.begin(SD_CS, SD_SCK_MHZ(25))) { // ESP32 requires 25 MHz limit
    Serial.println(F("SD initialization failed!"));
    for (;;);
  }

  // Initialize buttons
  pinMode(BTN_PIN1, INPUT_PULLUP);
  pinMode(BTN_PIN2, INPUT_PULLUP);
  pinMode(BTN_PIN3, INPUT_PULLUP);
  pinMode(BTN_PIN4, INPUT_PULLUP);

  ledcAttachChannel(BCK_LIGHT, freq, resolution, channel);
  ledcWrite(BCK_LIGHT, brightness); // set initial brightness to half 50%

  drawMenu();
}

void loop() {
  // Check for button presses
  checkButtonPress(BTN_PIN1, lastButtonState1, drawMenu);
  checkButtonPress(BTN_PIN2, lastButtonState2, selectOption);
  checkButtonPress(BTN_PIN3, lastButtonState3, moveSelector);
  checkButtonPress(BTN_PIN4, lastButtonState4, brightnessControl);
}

void checkButtonPress(int pin, bool &lastState, void (*callback)()) {
  bool currentState = digitalRead(pin);

  // Detect press event when the state goes from HIGH to LOW
  if (currentState == LOW && lastState == HIGH) {  
    callback();  // Execute function
    delay(300);  // Increased debounce delay
  }
  lastState = currentState; // Update last state
}

void drawMenu() {
  tft.fillScreen(ILI9341_BLACK); // Clear screen
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
  tft.println("Press Up/Down to navigate");
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
  tft.fillScreen(ILI9341_BLACK); // Clear screen
  tft.setCursor(20, 30);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);

  if (selectedOption == 0) {
    randomNum = random(6); // Simulate sensor data John
    learningMode(randomNum);
  } else {
    tft.println("Game Mode");
    reader.drawBMP("pig1.bmp", tft, 0, 10);
  }
}

void learningMode(int animalType) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(2); 
  tft.println("Learning Mode");

  switch (animalType) {
    case 0:
      tft.setCursor(0, 20);
      tft.println("The Animal is Pig");
      reader.drawBMP("3pig3.bmp", tft, 10, 40);
      break;
    case 1:
      tft.setCursor(0, 20);
      tft.println("The Animal is Chicken");
      reader.drawBMP("2chicken2.bmp", tft, 10, 40);
      break;
    case 2:
      tft.setCursor(0, 20);
      tft.println("The Animal is Dog");
      reader.drawBMP("2dog1.bmp", tft, 10, 40);
      break;
    case 3:
      tft.setCursor(0, 20);
      tft.println("The Animal is Horse");
      reader.drawBMP("2horse1.bmp", tft, 10, 40);
      break;
    case 4:
      tft.setCursor(0, 20);
      tft.println("The Animal is Cow");
      reader.drawBMP("cow1.bmp", tft, 10, 40);
      break;
    case 5:
      tft.setCursor(0, 20);
      tft.println("The Animal is Sheep");
      reader.drawBMP("sheep1.bmp", tft, 10, 40);
      break;
  }
}
