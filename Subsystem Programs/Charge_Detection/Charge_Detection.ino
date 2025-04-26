const int ADC_PIN = 35;  // GPIO35 for voltage measurement
const int LED_PIN = 25;  // GPIO25 for DAC output
const float SCALE_FACTOR = 2.95*5.08/4.7;  // Adjustment for voltage divider

void setup() {
    Serial.begin(9600);
    Serial.println("Monitoring voltage and adjusting LED...");
}

void loop() {
    int adcValue = analogRead(ADC_PIN);  // Read ADC value (0-4095)
    float voltage = (adcValue / 4095.0) * 3.3 * SCALE_FACTOR;  // Adjusted voltage

    // Set LED brightness based on voltage conditions
    if (voltage >= 5.5) {
        dacWrite(LED_PIN, 200);
    } else if (voltage > 4.5 && voltage <= 5.5) {
        dacWrite(LED_PIN, 180);
    } else {
        dacWrite(LED_PIN, 160);
    }

    Serial.println("ADC Value: ");
    Serial.print(adcValue);
    Serial.println(" | Adjusted Voltage: ");
    Serial.print(voltage);
  

    delay(5000);  // Delay for stability
}
