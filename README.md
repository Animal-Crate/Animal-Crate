<p align="center"> <img alt="Animal Crate" width="900" height="300" src="https://github.com/Animal-Crate/Animal-Crate/blob/master/Team-Logo.png" /></p>

This project is designed to create an animal noise toy that helps children to learn their animals and their sounds. It is specifically a toy that is meant to better cater to children and individuals with disabilities so that they can learn in a fun and constructive manner with a toy that is appealing and able to meet their needs.

This is a capstone project for the University of Kentucky ECE 490/491 that we are working on as a team. This is the public, open-source repository for this project and all code within this project falls under the provided license in this repository, AGPL-3.0 License. The provided schematics, CAD designs, and other resources are suggestions for recreating this project from home.

---

## Our Goal
The goal of this project is to design a child's toy that makes animal noises that is accessible to a wide-range of audiences, from children with disabilities or impairments to elderly with arthritis and compromised movement.

The provided code will be organized, commented, and user-friendly to allow for the ultimate configuration based on needs and systems. This is our goal as the members working on this project, to ensure that it is robust and well-rounded, allowing those without prior coding knowledge to make changes to suit their needs.

## Prerequisites
Before compiling the programming in this repository, some libraries are necessary for the Arduino IDE. These libraries are not all contained here due to licensing.

The following libraries are required for compiling and uploading to an ESP-32.
- The Animal Toy System utilizes the following libraries:
  - [Arduino LSM6DS3 Library](https://github.com/arduino-libraries/Arduino_LSM6DS3)
  - [DFPlayerMiniFast](https://github.com/PowerBroker2/DFPlayerMini_Fast)
- The Scanner System utilizes the following libraries:
  - [Adafruit ST7735 Library](https://github.com/adafruit/Adafruit-ST7735-Library) (For Testing)
  - [MFRC522](https://github.com/Animal-Crate/Animal-Crate/tree/master/MFRC522)
- The Sound System utilizes the following libraries:
  - [DFRobotDFPlayerMini](https://github.com/DFRobot/DFRobotDFPlayerMini)
  - [ESPSoftwareSerial](https://github.com/plerup/espsoftwareserial/)
- The Video/Algorithm System utilizes the following libraries:
  - [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
  - [Adafruit ILI9341 Library](https://github.com/adafruit/Adafruit_ILI9341)
  - [Adafruit ImageReader Library](https://github.com/adafruit/Adafruit_ImageReader)
  - [Adafruit SPIFlash](https://github.com/adafruit/Adafruit_SPIFlash)
  - [SdFat](https://github.com/greiman/SdFat)

## Acknowledgements
Please note that the files shown in [MFRC522](https://github.com/Animal-Crate/Animal-Crate/tree/master/_MFRC522) are taken from another repository. These are not licensed under this repository's license, and you can find more information in the README section in that directory.
