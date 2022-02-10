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
#include <fstream>
#include <iostream>
#include <SD.h>

//Classes
#include <Actuator.h>
#include <ODrive.h>
#include <Radio.h>
#include <SD_Reader.h>

using namespace std;

//General Settings
  //Log
  #define LOG_LEVEL LOG_LEVEL_VERBOSE
  //Note: By default the log requires and outputs to the SD card, and can be changed in setup

//<--><--><--><-->< Base Systems ><--><--><--><--><-->

//ODrive Settings
  #define starting_timeout 1000 //NOTE: In ms

  //PINS

  //CREATE OBJECT
  ODrive odrive(Serial1, true);

//LOGGING AND SD SETTINGS
  //Create SD object (contains the file stream)
  Sd sd;
  //Cannot create logging object until init



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

  Actuator actuator(&odrive, &Log, enc_A, enc_B, gearTooth_engine, 0, hall_inbound, hall_outbound, &external_count_egTooth, PRINTTOSERIAL);

  static void external_count_egTooth(){
    actuator.count_egTooth();
  }
  
//FREE FUNCTIONS

void setup() {
  // ofstream ofile;
  // ofile.open("huh.dat");
  sd.init();
  File coolGuy = SD.open("coolio.txt", FILE_WRITE);
  //Create log object given the SD card object
  // File logFile;
  Log.begin(LOG_LEVEL, &coolGuy);
  // int actuator_init = actuator.init();
  // if (!actuator_init) {
  //   Log.fatal("Failed to initialize Actuator, error code: %d", actuator_init);
  // }
  Log.notice("testing 1 2 3 4 5 6");
  //sd.write("testing 1 2 3");
  // sd.save();
  coolGuy.close();
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

