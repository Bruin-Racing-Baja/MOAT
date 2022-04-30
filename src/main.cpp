/*
MOAT - Model 2O Awesome Teensy
mk. V.0
General code to oversee all functions of the Teensy
*/

#include <Arduino.h>

// Libraries
#include <SPI.h> // MUST BE INCLUDED BEFORE ArduinoLog.h
#include <ArduinoLog.h> 
#include <HardwareSerial.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <string>
#include <OneButton.h>

// Classes
#include <Actuator.h>
#include <Cooling.h>
#include <Radio.h>

// #define MODE 4

// Startup
#define WAIT_SERIAL_STARTUP 0 // Set headless mode or not
#define HOME_ON_STARTUP 1
//#define RUN_DIAGNOSTIC_STARTUP 0

// Logging
// NOTE: By default the log requires and outputs to the SD card (can be changed in setup)
#define LOG_LEVEL LOG_LEVEL_NOTICE
#define SAVE_THRESHOLD 1000 // Sets how often the log object will save to SD when in operating mode

// Actuator

// Diagnostic Mode
#define DIAGNOSTIC_MODE_SHOTS 100 // Number of times diagnostic mode is run

//<--><--><--><-->< Base Systems ><--><--><--><--><-->
// ODrive Settings
#define ODRIVE_STARTING_TIMEOUT 1000 // [ms]

// Pins

// Create objects
// ODrive odrive(Serial1);

// Logging and SD settings
File log_file;

//<--><--><--><-->< Subsystems ><--><--><--><--><-->
Cooling cooler_o;

// Acuator settings
#define PRINT_TO_SERIAL false

// Pins test bed
//#define ENC_A_PIN 20
//#define ENC_B_PIN 21
//#define HALL_INBOUND_PIN 12
//#define HALL_OUTBOUND_PIN 11
//#define GEARTOOTH_ENGINE_PIN 15

// PINS CAR
#define ENC_A_PIN 2
#define ENC_B_PIN 3
#define HALL_INBOUND_PIN 22
#define HALL_OUTBOUND_PIN 23
#define GEARTOOTH_ENGINE_PIN 41
#define GEARTOOTH_GEARBOX_PIN 40

//DIAGNOSTIC PINS
#define LED_1 28
#define LED_2 29
#define LED_3 30
#define LED_4 31

#define BTN_TOP 8
#define BTN_LEFT 9
#define BTN_CENTER 10
#define BTN_RIGHT 11
#define BTN_BOTTOM 12

Actuator actuator(Serial1, ENC_A_PIN, ENC_B_PIN, GEARTOOTH_ENGINE_PIN, HALL_INBOUND_PIN, HALL_OUTBOUND_PIN, PRINT_TO_SERIAL);

OneButton btn_top = OneButton(
  BTN_TOP,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);

OneButton btn_left = OneButton(
  BTN_LEFT,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);

OneButton btn_center = OneButton(
  BTN_CENTER,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);

OneButton btn_right = OneButton(
  BTN_RIGHT,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);

OneButton btn_bottom = OneButton(
  BTN_BOTTOM,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);

// Handler function for a single click:
static void handleClick() {
  Serial.println("Clicked!");
}

void setup()
{
  //-------------Wait for serial-----------------//
  if (WAIT_SERIAL_STARTUP)
  {
    while (!Serial) // wait for serial port to connect. Needed for native USB port only
    {
    }
  }

  Serial.begin(9600); //for testing my code

  // pinMode(BTN_LEFT, INPUT_PULLUP); //active low
  // pinMode(BTN_RIGHT, INPUT_PULLUP);
  // pinMode(BTN_TOP, INPUT_PULLUP);
  // pinMode(BTN_BOTTOM, INPUT_PULLUP);
  // pinMode(BTN_CENTER, INPUT_PULLUP);

// Single Click event attachment
  btn_top.attachClick(handleClick);
  btn_left.attachClick(handleClick);
  btn_center.attachClick(handleClick);
  btn_right.attachClick(handleClick);
  btn_bottom.attachClick(handleClick);

  pinMode(LED_1, OUTPUT); //active low
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
}

void loop() {
  btn_top.tick();
  btn_left.tick();
  btn_center.tick();
  btn_right.tick();
  btn_bottom.tick();

  //Serial.println("BUTTONS: " + String(digitalRead(BTN_LEFT)) + String(digitalRead(BTN_RIGHT)) + String(digitalRead(BTN_TOP)) + String(digitalRead(BTN_BOTTOM)) + String(digitalRead(BTN_CENTER)));

  /* T — doesn’t trigger
  L — good
  C — good
  R — triggers twice
  B — good */
  
  // Serial.println("BUTTONS: " + String(digitalRead(BTN_LEFT)) + String(digitalRead(BTN_RIGHT)) + String(digitalRead(BTN_TOP)) + String(digitalRead(BTN_BOTTOM)) + String(digitalRead(BTN_CENTER)));
  // actuator.run_test_sequence(LED_1, LED_2, LED_3, LED_4, BTN_LEFT, BTN_RIGHT, BTN_TOP, BTN_BOTTOM, BTN_CENTER); //TODO: actuator does not name a type??
  // digitalWrite(LED_2, HIGH);
  // digitalWrite(LED_3, LOW);
  // digitalWrite(LED_4, HIGH);

}