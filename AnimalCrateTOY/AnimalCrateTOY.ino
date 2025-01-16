int xPin = A0;
int yPin = A1;
int zPin = A2;
int speakerPin = D3;

void setup() {
  Serial.begin(9600);
  pinMode(speakerPin, OUTPUT);
}

void loop() {
  int x = analogRead(xPin);
  int y = analogRead(yPin);
  int z = analogRead(zPin);

  // Convert analog readings to G values
  float xG = (x - 512) * 0.0033;
  float yG = (y - 512) * 0.0033;
  float zG = (z - 512) * 0.0033;

  // Calculate overall acceleration
  float acceleration = sqrt(xG * xG + yG * yG + zG * zG);
 
  // Detect shake based on acceleration threshold
  if (acceleration > 1.5) {
    playAnimalSound(random(1, 5));
  }

  delay(100); // Delay for stability
}

void playAnimalSound(int sound) {
  switch (sound) {
    case 1:
      Serial.println("Playing cow sound...");
      tone(speakerPin, 500, 500); // Adjust the frequency and duration as needed
      break;
    case 2:
      Serial.println("Playing dog sound...");
      tone(speakerPin, 700, 500); // Adjust the frequency and duration as needed
      break;
    case 3:
      Serial.println("Playing cat sound...");
      tone(speakerPin, 900, 500); // Adjust the frequency and duration as needed
      break;
    case 4:
      Serial.println("Playing bird sound...");
      tone(speakerPin, 1100, 500); // Adjust the frequency and duration as needed
      break;
  }
}
