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
#include <Battery.h>
#include <Cooler.h>
#include <Radio.h>
#include <Report.h>


//GENERAL SETTINGS
  #define DEBUG_MODE 0 //Starts with Serial output(like to the computer), waits for connection

  #define WAIT_FOR_RADIO 0 //Waits for a radio connection before continuing
  //Useful for debugging issues with startup report

  #define req_battery 1 //Halts program if battery allocation fails
  #define req_radio 1 //Halts program if radio allocation fails
  #define req_actuator 1 //Halts program if actuator allocation fails
  #define req_cooler 1 //Halts program if cooler allocation fails



//ACTUATOR SETTINGS
  //PINS
  #define enc_A 20
  #define enc_B 21
  #define hall_inbound 10
  #define hall_outbound 11

  //CONSTANTS
  #define enc_PPR 2048
  #define enc_index 22
  #define motor_number 0
  #define homing_timeout 30000 //NOTE: In ms
  #define cycle_period 5000 //If u wanna use freq instead : 1/x=T

  //CREATE OBJECT
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
  Battery battery(battery_pin_1);



//COOLER SETTINGS
  //Add like req ports, 

  //PINS
  #define THERMO_PIN 8

  //CONSTANTS

  //CREATE OBJECT
  Cooler cooler(THERMO_PIN);



//RADIO SETTINGS
  //Pin numbers
  #define RADIO_CS 4
  #define RADIO_RST 2
  #define RADIO_INT 3

  //CONSTANTS
  #define RADIO_FREQ 915.0

  //CREATE OBJECT
  Radio radio(RADIO_CS, RADIO_RST, RADIO_INT, RADIO_FREQ);



void setup() {
/*---------------------------[Debug Init]---------------------------*/
  if (DEBUG_MODE) {
    Serial.begin(9600);
    while (!Serial) {
      ; //Wait for serial to be ready
    }
    Serial.println("[DEBUG MODE]");
  }

/*---------------------------[Actuator Init]---------------------------*/
  if (actuator.init() != 0 && req_actuator){
    debugMessage("[ERROR] Actuator failed to initialize");
    while(1);
  }

/*---------------------------[Radio Init]---------------------------*/
  if (radio.init() != 0 && req_radio){
    debugMessage("[ERROR] Radio failed to initialize");
    while(1);
  }

  debugMessage("All systems initialized successfully");

/*---------------------------[Wait for radio]---------------------------*/
  if (WAIT_FOR_RADIO) {
    while (true) {
      int response = radio.checkConnection();
      if (response) {
        break;
      }
      delay(200);
    }
  }
}

void loop() {
/*---------------------------[Overall Init]---------------------------*/
  Report report; //Generates a new report object

  // report.battery(battery.measureVoltage()); //Measures battery voltage and adds to the report

  // report.cooler_temp()

  // report.a_odometry(actuator.odometry());

  // report.a_action(actuator.action());

  // actuator.action();

  char* generated_report = report.getReport();

  radio.send(generated_report, sizeof(generated_report)); //Sends the report to the radio
}

void debugMessage(const char* message) {
  if (DEBUG_MODE) {
    Serial.println(message);
  }
}