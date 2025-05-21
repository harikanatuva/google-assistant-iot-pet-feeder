# Google Assistant Controlled IoT Pet Feeder

This project uses an ESP8266 microcontroller, servo motor, LCD display, and Adafruit MQTT to build a smart pet feeder controlled by Google Assistant.

## Features

- Voice-controlled feeding
- Scheduled dispensing
- Real-time LCD updates
- MQTT communication with Adafruit server

## Components

- ESP8266
- Servo Motor
- 16x2 LCD (with I2C)
- Internet connection
- Adafruit MQTT account
- Google Assistant integration

## Circuit Diagram
(Add circuit image here)

## Code
The `feeder.ino` file contains the Arduino code to run the pet feeder.

## How It Works

1. Connect ESP8266 to Wi-Fi
2. Use Adafruit MQTT to link with Google Assistant
3. Send commands like "Feed my pet"
4. Servo motor rotates and food is dispensed

## Future Ideas

- Add a weight sensor
- Integrate a camera
