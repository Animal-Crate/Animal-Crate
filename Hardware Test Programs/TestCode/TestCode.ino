// This code will flash the onboard LED three times when the reset button is pressed

const int buttonPin1 = 2; // D2 
const int buttonPin2 = 4; // D4 
const int tonePin1 = 9; // D9
const int tonePin2 = 10; // D10

const int song1[][2] = {
  {440, 500}, // A4, half a second (500 ms) 
  {494, 500}, // B4, half a second (500 ms) 
  {523, 500} // C5, half a second (500 ms) 
  };

  const int song2[][2] = { 
    {349, 500}, // F4, half a second (500 ms) 
  {392, 500}, // G4, half a second (500 ms) 
  {440, 500} // A4, half a second (500 ms) 
  };

  const int song1Length = sizeof(song1) / sizeof(song1[0]); 
  const int song2Length = sizeof(song2) / sizeof(song2[0]);

  void setup(){
  // Initialize the digital pins as input and output
  pinMode(buttonPin1, INPUT); 
  pinMode(buttonPin2, INPUT);
  pinMode(tonePin1, OUTPUT); 
  pinMode(tonePin2, OUTPUT); 
  }

  void loop() {
    // Read the state of the buttons
    int buttonState1 = digitalRead(buttonPin1);
    int buttonState2 = digitalRead(buttonPin2);

    if (buttonState1 == HIGH) {
      // Play the first song through tonePin1
      for (int idx = 0; idx < song1Length; idx++) {
       tone(tonePin1, song1[idx][0], song1[idx][1]);
       delay(song1[idx][1]);
       } 
       noTone(tonePin1); // Stop the tone after playing 
       }
       else if (buttonState2 == HIGH) {
        for (int idx = 0; idx < song2Length; idx++) {
          tone(tonePin2, song2[idx][0], song2[idx][1]);
          delay(song2[idx][1]);
          } noTone(tonePin2); // Stop the tone after playing 
          }
           }
