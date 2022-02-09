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
#include <string>

//Classes
#include <Actuator.h>
#include <ODrive.h>
#include <Radio.h>

//GENERAL SETTINGS
  #define DEBUG_MODE 0 //Starts with Serial output(like to the computer), waits for connection
  #define PRINTTOSERIAL 1 //Set to 1 if connected to the serial moniter 0 if not

  #define RADIO_DEBUG_MESSAGES 0 //Sends debugMessages over radio as well as Serial (no confirmation that signal is recieved)
  //NOTE: This makes no guarantees that the messages are actually sent or recieved

  #define WAIT_FOR_RADIO 0 //Waits for a radio connection before continuing
  //Useful for debugging issues with startup report
  
  #define HOME_ON_STARTUP 0 //Homes actuator immediately after homing
  
  //Enabled systems will initialize and run, while disbaled systems will not
  #define enable_actuator 1
  #define enable_radio 1


  //Halts program if the initializtion of the system fails
  #define req_radio 1
  #define req_actuator 1


//<--><--><--><-->< Base Systems ><--><--><--><--><-->

//BATTERY SETTINGS
  //Add like req ports, 

  //PINS
  #define battery_pin_1 3
  //CONSTANTS
  #define enc_PPR 2048
  #define enc_index 22
  #define motor_number 1
  #define homing_timeout 30000 //NOTE: In ms
  #define cycle_period 5000 //If u wanna use freq instead : 1/x=T
  
  //CREATE OBJECT
  //Battery battery(battery_pin_1);

//ODRIVE SETTINGS
  #define starting_timeout 1000 //NOTE: In ms

  //PINS

  //CREATE OBJECT
  ODrive odrive(Serial1, DEBUG_MODE);

//RADIO SETTINGS
  //Pin numbers
  #define RADIO_CS 4
  #define RADIO_RST 2
  #define RADIO_INT 3

  //CONSTANTS
  #define RADIO_FREQ 915.0

  //CREATE OBJECT
  Radio radio(RADIO_CS, RADIO_RST, RADIO_INT, RADIO_FREQ);



//<--><--><--><-->< Sub-Systems ><--><--><--><--><-->
//ACTUATOR SETTINGS
  //PINS TEST BED
  // #define enc_A 20
  // #define enc_B 21
  // #define hall_inbound 12
  // #define hall_outbound 11
  // #define gearTooth_engine 15 //rn just attached to encoder B haha

  //PINS CAR
  #define enc_A 2
  #define enc_B 3
  #define hall_inbound 23
  #define hall_outbound 33
  #define gearTooth_engine 41
  #define gearTooth_gearbox 40


  //CREATE OBJECT
  static void external_interrupt_handler();
  static void external_count_egTooth();
//RM external int handler
  Actuator actuator(&odrive, enc_A, enc_B, gearTooth_engine, 0, hall_inbound, hall_outbound, &external_count_egTooth, PRINTTOSERIAL);

  static void external_count_egTooth(){
    actuator.count_egTooth();
  }
  

//COOLER SETTINGS
  //Add like req ports, 

  //PINS
  #define THERMO_PIN 8

  //CONSTANTS

  //CREATE OBJECT
  //Cooler cooler(THERMO_PIN);


//FREE FUNCTIONS

void debugMessage(String message) {
  if (DEBUG_MODE) {
    Serial.println(message);
  }
  if (RADIO_DEBUG_MESSAGES) {
    int result = radio.send(message, sizeof(message));
  }
}

Encoder encoder(2, 3);

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
      if (req_radio) {
        while (1) ;  //Halt if radio connection is required
      }
    }

    if (WAIT_FOR_RADIO) { //Wait for radio connection and reciprocation
      debugMessage("Waiting for radio connection");
      while (!radio.checkConnection()) ;
      debugMessage("Radio connection success");
    }
  }

/*---------------------------[Actuator Init]---------------------------*/
  

//   if (enable_actuator) {
//     return_code = actuator.init();
//     if (return_code != 0) {
//       debugMessage("[ERROR] Actuator init failed with error code");
//       debugMessage("0" + return_code);
//       if (req_actuator) {
//         while (1) ;
//       }
//     }
//     if (HOME_ON_STARTUP) {
//       if (return_code = actuator.homing_sequence() != 0) {
//         debugMessage("[ERROR] Actuator init failed with error code");
//         debugMessage("0" + return_code);
//       }
//     }
//   }

//   debugMessage("All systems initialized successfully");
}

void loop() {
//"When you join the MechE club thinking you could escape the annoying CS stuff like pointers and interrupts"
//                               __...------------._
//                          ,-'                   `-.
//                       ,-'                         `.
//                     ,'                            ,-`.
//                    ;                              `-' `.
//                   ;                                 .-. \
//                  ;                           .-.    `-'  \
//                 ;                            `-'          \
//                ;                                          `.
//                ;                                           :
//               ;                                            |
//              ;                                             ;
//             ;                            ___              ;
//            ;                        ,-;-','.`.__          |
//        _..;                      ,-' ;`,'.`,'.--`.        |
//       ///;           ,-'   `. ,-'   ;` ;`,','_.--=:      /
//      |'':          ,'        :     ;` ;,;,,-'_.-._`.   ,'
//      '  :         ;_.-.      `.    :' ;;;'.ee.    \|  /
//       \.'    _..-'/8o. `.     :    :! ' ':8888)   || /
//        ||`-''    \\88o\ :     :    :! :  :`""'    ;;/
//        ||         \"88o\;     `.    \ `. `.      ;,'
//        /)   ___    `."'/(--.._ `.    `.`.  `-..-' ;--.
//        \(.="""""==.. `'-'     `.|      `-`-..__.-' `. `.
//         |          `"==.__      )                    )  ;
//         |   ||           `"=== '                   .'  .'
//         /\,,||||  | |           \                .'   .'
//         | |||'|' |'|'           \|             .'   _.' \
//         | |\' |  |           || ||           .'    .'    \
//         ' | \ ' |'  .   ``-- `| ||         .'    .'       \
//           '  |  ' |  .    ``-.._ |  ;    .'    .'          `.
//        _.--,;`.       .  --  ...._,'   .'    .'              `.__
//      ,'  ,';   `.     .   --..__..--'.'    .'                __/_\
//    ,'   ; ;     |    .   --..__.._.'     .'                ,'     `.
//   /    ; :     ;     .    -.. _.'     _.'                 /         `
//  /     :  `-._ |    .    _.--'     _.'                   |
// /       `.    `--....--''       _.'                      |
//           `._              _..-'                         |
//              `-..____...-''                              |
//                                                          |
//                                mGk                       |
}

