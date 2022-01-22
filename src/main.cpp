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
//#include <Report.h>


//GENERAL SETTINGS
  #define DEBUG_MODE 0 //Starts with Serial output(like to the computer), waits for connection
  #define PRINTTOSERIAL 1 //Set to 1 if connected to the serial moniter 0 if not

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
  #define gearTooth_engine 15 //rn just attached to encoder B haha
  //CREATE OBJECT
  static void external_interrupt_handler();
  static void external_count_egTooth();

  Actuator actuator(Serial1, enc_A, enc_B, gearTooth_engine, 0, hall_inbound, hall_outbound,  &external_interrupt_handler, &external_count_egTooth, PRINTTOSERIAL);

  //CREATE GODFRSAKEN FUNCTION (NO QUESTIONS)
  static void external_interrupt_handler() {
    actuator.control_function();
  }

  static void external_count_egTooth(){
    actuator.count_egTooth();
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
  //Radio radio(RADIO_CS, RADIO_RST, RADIO_INT, RADIO_FREQ);



void setup() {
/*---------------------------[Overall Init]---------------------------*/
  // if (DEBUG_MODE) {
  //   Serial.begin(9600);
  //   while (!Serial) {
  //     ; //Wait for serial to be ready
  //   }
  //   Serial.println("[DEBUG MODE]");
  // }
  bool goodboy = true;
  
  Serial.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor to open
  Serial.println("Getty is cool");
  actuator.init();
  //Serial.print(actuator.communication_speed());
  
  // if (actuator.init() != 0 && req_actuator){
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
    Serial.print(" encoder position: ");
    Serial.println(actuator.get_encoder_pos());
    delay(100);
/*---------------------------[Overall Init]---------------------------*/
  // Report report; //Generates a new report object

  // report.battery(battery.measureVoltage()); //Measures battery voltage and adds to the report

  // report.cooler_temp()

  // report.a_odometry(actuator.odometry());

  // report.a_action(actuator.action());

  // actuator.action();

  // char* generated_report = report.getReport();

  // radio.send(generated_report, sizeof(generated_report)); //Sends the report to the radio
}

void debugMessage(const char* message) {
  if (DEBUG_MODE) {
    Serial.println(message);
  }
}