#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <string.h>


// For the Adafruit shield, these are the default.
#define TFT_DC 2
#define TFT_CS 15
#define TFT_MOSI 23
#define TFT_CLK 19
#define TFT_RST 18
#define TFT_MISO 4
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {
  Serial.begin(9600);
  Serial.println("Setup started");

  // Initialize display
  tft.begin();
  Serial.println("Display initialized");

  tft.setRotation(1); // Portrait mode
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(20, 30);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("Test screen");
  tft.setTextSize(5);
  tft.println("Groop");
  delay(1000);
  drawMenu();
  Serial.println("Setup complete");
}

void loop() {
  // Add any required logic here
}


void drawMenu() {
  tft.fillScreen(ILI9341_BLACK); // Clear screen

  tft.setCursor(50, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(4);
  tft.println("Main Menu");

  // Draw Option 1
  tft.setCursor(20, 50);
 // if (selectedOption == 0) {
 //   tft.setTextColor(ILI9341_YELLOW); // Highlight selected option
  //} 
 // else {
  //  tft.setTextColor(ILI9341_WHITE);
 // }
  tft.setTextSize(3);
  tft.println("> Learning Mode");

  // Draw Option 2
  tft.setCursor(20, 90);
 // if (selectedOption == 1) {
 //   tft.setTextColor(ILI9341_YELLOW); // Highlight selected option
 // } 
  //else {
  //  tft.setTextColor(ILI9341_WHITE);
 // }
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
