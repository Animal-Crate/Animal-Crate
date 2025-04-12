The programs in this directory are intended for testing the individual systems within this project. This is a modular based project such that each subsystem can be tested individually to ensure they are working correctly and as intended. The programs are listed below for each part and how they work.

---
**NOTE**

These programs have **Prerequisites** for the libraries prior to compiling. It is suggested that you review these libraries on the front page to ensure that you have all of the necessary files for compiling in Arduino IDE. The links to the GitHub repositories for these are provided, but must be downloaded individually in the Arduino IDE if using it as the source for compilation to an ESP32 or other microcontroller.

---

### Algorithm-Feedback
The `Algorithm-Feedback.ino` file is used for testing the individual buttons (for the display), the display, and the learning/game modes within the program. This does not test the scanning and audio functions, but does test the display and buttons for manipulating the brightness, navigation, and functionality around the different modes in this project.

### Animal_Toy
The `Animal_Toy.ino` file is used for testing the individual toys and their functionalities, including the DFPlayer Minis that are used in each of the toys. This tests the gyro controls in the microcontrollers that were used in the toys, and is not necessary in the final project if ignoring the individual toys' functionality.

### Power-System_Debug
The `Power-System_Debug.ino` file is used for testing the functionality of a dedicated power system following the same design used in this project, which is 4 C-Type Batteries with components to step the voltage down for the required voltage for each of the parts used in this project in the other subsystems.

### Scanner_Debug & Scanner_Debug-Display
The `Scanner_Debug.ino` and `Scanner_Debug-Display.ino` files are used for testing the scanner functionality to serial connection and to display. These are intended to test the scanner and its primary functions within the project. The `Scanner_Debug.ino` tests the functionality of the Scanner itself without a display. The `Scanner_Debug-Display.ino` is intended to demonstrate the functionality of the Scanner in converting the UID of the NFC chip to a string value that can be used in the project.
- It is suggested to run both, if possible, to ensure that the NFC chips used have UIDs that can be properly displayed. The purpose of this is to ensure that the UID is within 32 characters, the maximum for the program, and that the display can show these appropriately so that it can be used within the program for comparing the animals later.

### Sound-System_Debug
The `Sound-System_Debug.ino`file is used for testing the audio subsystem by itself to ensure that the animals are played. This also tests the buttons associated with the sound system to ensure that they trigger the volume up/down and plays the audio. This is also used to ensure the correct wiring for the DFPlayer Mini. This is necessary for testing the functionality of the DFPlayer Mini due to the current draw.
- It is suggested to test the audio system by itself, since greater current draw, such as testing the scanner and display alongside the audio system, may cause issues with the DFPlayer Mini powering the speaker, depending on the speaker draw.