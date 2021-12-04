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
#include <Radio.h>
//#include <Report.h>


//GENERAL SETTINGS
  #define DEBUG_MODE 0 //Starts with Serial output(like to the computer), waits for connection

  #define RADIO_DEBUG_MESSAGES 0 //Sends debugMessages over radio as well as Serial (no confirmation that signal is recieved)
  //NOTE: This makes no guarantees that the messages are actually sent or recieved

  #define WAIT_FOR_RADIO 0 //Waits for a radio connection before continuing
  //Useful for debugging issues with startup report
  
  #define HOME_ON_STARTUP 1 //Homes actuator immediately after homing
  //Enabled systems will initialize and run, while disbaled systems will not
  #define enable_actuator 1
  #define enable_radio 1


  //Halts program if the initializtion of the system fails
  #define req_battery 1
  #define req_radio 1
  #define req_actuator 1
  #define req_cooler 1

//ACTUATOR SETTINGS
  //PINS
  #define enc_A 20
  #define enc_B 21
  #define hall_inbound 10
  #define hall_outbound 11

  //CONSTANTS
  #define enc_PPR 2048
  #define enc_index 22
  #define motor_number 1
  #define homing_timeout 30000 //NOTE: In ms
  #define cycle_period 5000 //If u wanna use freq instead : 1/x=T
  
  //CREATE OBJECT
  static void external_interrupt_handler();

  Actuator actuator(
    Serial1, enc_A, enc_B, 0, 0, hall_inbound, hall_outbound, 
    motor_number, homing_timeout, cycle_period, &external_interrupt_handler);

  //CREATE GODFRSAKEN FUNCTION (NO QUESTIONS)
  static void external_interrupt_handler() {
    actuator.control_function();
  }  

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
  Radio radio(RADIO_CS, RADIO_RST, RADIO_INT, RADIO_FREQ);

//FREE FUNCTIONS

void debugMessage(const char* message) {
  if (DEBUG_MODE) {
    Serial.println(message);
  }
  if (RADIO_DEBUG_MESSAGES) {
    int result = radio.send(message, sizeof(message));
  }
}

void setup() {
/*---------------------------[Overall Init]---------------------------*/
  int return_code;

  if (DEBUG_MODE) {
    Serial.begin(9600);
    while (!Serial) ; //Wait for serial to be ready
    Serial.println("[DEBUG MODE]");
  }

/*---------------------------[Radio Init]---------------------------*/
  if (enable_radio) {
    return_code = radio.init();
    if (return_code != 0) {
      debugMessage("[ERROR] Radio init failed with error code");
      debugMessage("0" + return_code);
    }

    if (WAIT_FOR_RADIO) {
      debugMessage("Waiting for radio connection");
      while (!radio.checkConnection()) ;
      debugMessage("Radio connection success");
    }
  }

/*---------------------------[Actuator Init]---------------------------*/
  

  if (enable_actuator) {
    return_code = actuator.init();
    if (return_code != 0) {
      debugMessage("[ERROR] Actuator init failed with error code");
      debugMessage("0" + return_code);
      if (req_actuator) {
        while (1) ;
      }
    }
    // if (HOME_ON_STARTUP) {
    //   if (return_code = actuator.homing_sequence() != 0) {
    //     debugMessage("[ERROR] Actuator init failed with error code");
    //     debugMessage("0" + return_code);
    //   }
    // }
  }

  bool goodboy = true;
  actuator.init();
  Serial.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor to open
  Serial.print(actuator.communication_speed());

  // debugMessage("All systems initialized successfully");
  // if (WAIT_FOR_RADIO) {
  //   while (true) {
  //     int response = radio.checkConnection();
  //     if (response) {
  //       break;
  //     }
  //     delay(200);
  //   }
  // }
}

void loop() {
  
}

