/* 
MOAT - Model 2O Awesome Teensy
mk. V.0
General code to oversee all functions of the Teensy

*/

#include <Arduino.h>

//Libraries
#include <SPI.h>
#include <RH_RF95.h>
#include <SoftwareSerial.h>

//Classes
#include <Actuator.h>
//#include <Battery.h>
//#include <Cooler.h>
//#include <Radio.h>


//GENERAL SETTINGS
  #define DEBUG_MODE 0 //Starts with Serial output, waits for connection
  #define req_battery 1 //Halts program if battery allocation fails
  #define req_radio 1 //Halts program if radio allocation fails
  #define req_actuator 1 //Halts program if actuator allocation fails
  #define req_cooler 1 //Halts program if cooler allocation fails



//ACTUATOR SETTINGS
  //Add like req ports, 

  //PINS
  #define eg_tooth 1
  #define gb_tooth 1
  #define inbound_hall 1
  #define outbound_hall 1
  //CONSTANTS
  #define odrive_serial Serial1
  //CREATE OBJECT
  Actuator actuator(odrive_serial, eg_tooth, gb_tooth, inbound_hall, outbound_hall);



//BATTERY SETTINGS
  //Add like req ports, 

  //PINS
  #define battery_pin_1 3
  //CONSTANTS

  //CREATE OBJECT
  //Battery battery(battery_pin_1);



//COOLER SETTINGS
  //Add like req ports, 

  //PINS
  #define THERMO_PIN 8

  //CONSTANTS

  //CREATE OBJECT
  //Cooler cooler(THERMO_PIN);



//RADIO SETTINGS
  //Pin numbers
  #define RADIO_CS 4
  #define RADIO_RST 2
  #define RADIO_INT 3

  //CONSTANTS
  #define RADIO_FREQ 915.0

  //CREATE OBJECT
  //Radio radio(RADIO_CS, RADIO_RST, RADIO_INT, RADIO_FREQ);



//ERROR TRACKER {ACTUATOR, BATTERY, COOLER, RADIO} (alphabetical)
  int error_tracker[4] = {0,0,0,0};



void setup() {
/*---------------------------[Overall Init]---------------------------*/
  // if (DEBUG_MODE) {
  //   Serial.begin(9600);
  //   while (!Serial) {
  //     ; //Wait for serial to be ready
  //   }
  //   Serial.println("[DEBUG MODE]");
  // }

  // error_tracker[0] = actuator.init();
  // if (error_tracker[0] != 0 && req_actuator){
  //   debugMessage("[ERROR] Actuator failed to initialize");
  //   while(1);
  // }
  // error_tracker[2] = cooler.init();
  // if (error_tracker[2] != 0 && req_cooler){
  //   debugMessage("[ERROR] Cooler failed to initialize");
  //   while(1);
  // }
  // error_tracker[3] = radio.init();
  // if (error_tracker[3] != 0 && req_radio){
  //   debugMessage("[ERROR] Radio failed to initialize");
  //   while(1);
  // }

  // debugMessage("All systems initialized successfully");


  actuator.initialize();
}

void loop() {
  actuator.control_function();
}

void debugMessage(const char* message) {
  if (DEBUG_MODE) {
    Serial.println(message);
  }
}