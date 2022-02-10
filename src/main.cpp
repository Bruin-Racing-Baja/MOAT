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
#include <ArduinoLog.h>

//Classes
#include <Actuator.h>
#include <ODrive.h>
#include <Radio.h>
#include <SD_Reader.h>

//GENERAL SETTINGS
  #define LOG_LEVEL LOG_LEVEL_VERBOSE
  
  #define DEBUG_MODE 0 //Starts with Serial output(like to the computer), waits for connection
  #define PRINTTOSERIAL 1 //Set to 1 if connected to the serial moniter 0 if not



  #define RADIO_DEBUG_MESSAGES 0 //Sends debugMessages over radio as well as Serial (no confirmation that signal is recieved)
  //NOTE: This makes no guarantees that the messages are actually sent or recieved

//<--><--><--><-->< Base Systems ><--><--><--><--><-->

//ODRIVE SETTINGS
  #define starting_timeout 1000 //NOTE: In ms

  //PINS

  //CREATE OBJECT
  ODrive odrive(Serial1, true);

//LOGGING AND SD SETTINGS
  //Create SD object (contains the file stream)
  Sd sd;
  //Create logging object


//<--><--><--><-->< Sub-Systems ><--><--><--><--><-->
//ACTUATOR SETTINGS
  //PINS TEST BED
  #define PRINTTOSERIAL false
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

  static void external_count_egTooth();

  Actuator actuator(&odrive, enc_A, enc_B, gearTooth_engine, 0, hall_inbound, hall_outbound, &external_count_egTooth, PRINTTOSERIAL);

  static void external_count_egTooth(){
    actuator.count_egTooth();
  }
  
//FREE FUNCTIONS


void setup() {
  actuator.init();
}

void loop() {
/*
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
*/
}

