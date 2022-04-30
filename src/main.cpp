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

//clicked
bool is_top_click, is_left_click, is_center_click, is_right_click, is_bottom_click;

// Handler function for a single click:
static void handleClick_t() {
  is_top_click = true;
//  Serial.println("is_top_click");
}

static void handleClick_l() {
  is_left_click = true;
//  Serial.println("is_left_click");
}

static void handleClick_c() {
  is_center_click = true;
 // Serial.println("is_center_click");
}

static void handleClick_r() {
  is_right_click = true;
 // Serial.println("is_right_click");
}

static void handleClick_b() {
  is_bottom_click = true;
 // Serial.println("is_bottom_click");
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

// Single Click event attachment
  btn_top.attachClick(handleClick_t);
  btn_left.attachClick(handleClick_l);
  btn_center.attachClick(handleClick_c);
  btn_right.attachClick(handleClick_r);
  btn_bottom.attachClick(handleClick_b);

  pinMode(LED_1, OUTPUT); //active low
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
}

void loop() {
  btn_top.tick(); //TODO: find out if these calls slow the rest of the code down significantly
  btn_left.tick();
  btn_center.tick();
  btn_right.tick();
  btn_bottom.tick();

  actuator.run_test_sequence(LED_1, LED_2, LED_3, LED_4, is_top_click, is_left_click, is_center_click, is_right_click, is_bottom_click); //TODO: actuator does not name a type??

  //if the buttons aren't triggered, set the buttons to false 
  //TODO: find a less hack-y and more robust solution. could pass by reference to the actuator.run_test_sequence
  is_top_click = false;
  is_left_click = false;
  is_center_click = false;
  is_right_click = false;
  is_bottom_click = false;
}