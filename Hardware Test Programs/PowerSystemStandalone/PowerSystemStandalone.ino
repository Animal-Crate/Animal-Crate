#include <avr/sleep.h>
#include <avr/power.h>

const int mosfetPin = 15; // GPIO pin for MOSFET control
const int analogPin = 36; // ADC1_CH0 on ESP32 (GPIO 36)
const int greenLEDPin = 12; // GPIO pin for green LED
const int yellowLEDPin = 13; // GPIO pin for yellow LED
const int redLEDPin = 14; // GPIO pin for red LED

const float R1 = 160000.0; // 160 kΩ
const float R2 = 82000.0;  // 82 kΩ
const float referenceVoltage = 3.3; // ESP32 reference voltage
const float minOperationalVoltage = 4.0; // Minimum voltage threshold
const unsigned long inactivityThreshold = 600000; // 10 minutes in milliseconds

unsigned long lastInteractionTime = 0;

void setup() {
  setupPins();
  Serial.begin(115200); // Initialize serial communication
  lastInteractionTime = millis(); // Initialize the last interaction time
}

void loop() {
  float batteryVoltage = readBatteryVoltage();
  displayVoltage(batteryVoltage);
  indicateSoC(batteryVoltage);

  // Check for serial input
  if (Serial.available() > 0) {
    String command = Serial.readString();
    lastInteractionTime = millis(); // Reset the timer on user interaction

    if (command == "MOSFET_ON") {
      controlMOSFET(true); // Turn on the MOSFET
    } else if (command == "MOSFET_OFF") {
      controlMOSFET(false); // Turn off the MOSFET
    }
  }

  // Check if the battery voltage is above the minimum operational threshold
  if (batteryVoltage >= minOperationalVoltage) {
    controlMOSFET(true); // Turn on the MOSFET to power the load
  } else {
    controlMOSFET(false); // Turn off the MOSFET to cut power to the load
    enterSleep(); // Enter sleep mode
  }

  // Check for inactivity
  if (millis() - lastInteractionTime >= inactivityThreshold) {
    controlMOSFET(false); // Turn off the MOSFET to cut power to the load
    enterSleep(); // Enter sleep mode
  }

  // Wait for a short period before the next loop iteration
  delay(1000); // 1 second delay for loop
}

void setupPins() {
  pinMode(mosfetPin, OUTPUT); // Initialize the MOSFET pin as an output
  pinMode(greenLEDPin, OUTPUT); // Initialize the green LED pin as an output
  pinMode(yellowLEDPin, OUTPUT); // Initialize the yellow LED pin as an output
  pinMode(redLEDPin, OUTPUT); // Initialize the red LED pin as an output
  
  digitalWrite(mosfetPin, LOW); // Turn off the MOSFET initially
  digitalWrite(greenLEDPin, LOW); // Turn off the green LED initially
  digitalWrite(yellowLEDPin, LOW); // Turn off the yellow LED initially
  digitalWrite(redLEDPin, LOW); // Turn off the red LED initially
}

void controlMOSFET(bool state) {
  digitalWrite(mosfetPin, state ? HIGH : LOW);
}

float readBatteryVoltage() {
  int sensorValue = analogRead(analogPin);
  float voltage = sensorValue * (referenceVoltage / 4095.0); // ESP32 has 12-bit ADC
  return voltage * ((R1 + R2) / R2);
}

void displayVoltage(float voltage) {
  Serial.print("Battery Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");
}

void indicateSoC(float batteryVoltage) {
  if (batteryVoltage >= 5.6) {  // Full charge is when total voltage is close to 6V
    digitalWrite(greenLEDPin, HIGH);
    digitalWrite(yellowLEDPin, LOW);
    digitalWrite(redLEDPin, LOW);
  } else if (batteryVoltage >= 4.8 && batteryVoltage < 5.6) {  // Medium charge threshold
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(yellowLEDPin, HIGH);
    digitalWrite(redLEDPin, LOW);
  } else {  // Low charge threshold
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(yellowLEDPin, LOW);
    digitalWrite(redLEDPin, HIGH);
  }
}

void enterSleep() {
  Serial.println("Entering Sleep Mode...");
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode to power down
  sleep_enable(); // Enable sleep mode

  // Disable ADC to save power
  ADCSRA &= ~(1 << ADEN);

  // Sleep
  sleep_mode();

  // Wake up
  sleep_disable(); // Disable sleep mode
  power_all_enable(); // Enable all power
  Serial.println("Waking up from Sleep Mode...");
}
