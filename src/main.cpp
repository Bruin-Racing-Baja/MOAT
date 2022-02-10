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
#include <SD.h>

//Classes
#include <Actuator.h>
#include <ODrive.h>
#include <Radio.h>

using namespace std;

//General Settings
  //Mode
  #define OPERATING 0

  //Startup
  #define WAIT_SERIAL_STARTUP 1
  #define RUN_DIAGNOSTIC_STARTUP 0

  #define DIAGNOSTIC_MODE 0

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
  //Create file to log to
  File logFile;
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

  Actuator actuator(&odrive, enc_A, enc_B, gearTooth_engine, 0, hall_inbound, hall_outbound, &external_count_egTooth, PRINTTOSERIAL);

  static void external_count_egTooth(){
    actuator.count_egTooth();
  }
  
//FREE FUNCTIONS

// void save_log() {
//   logFile.close();
//   logFile = SD.open("log.txt", FILE_WRITE);
// }

void setup() {
  //-------------Wait for serial-----------------
  if(WAIT_SERIAL_STARTUP) {
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  }
  //-------------Logging and SD Card-----------------
  SD.begin(BUILTIN_SDCARD);
  logFile = SD.open("log.txt", FILE_WRITE);
  Log.begin(LOG_LEVEL_VERBOSE, &logFile, false);
  Log.verbose("Logging Started");
  /*
  The log file will only actually save when the file is closed, so here
  is an interrupt to save the file every 30 seconds and then re-open it
  */
  
  //-------------Actuator-----------------
  Serial.println("Starting Actuator");
  int actuator_init = actuator.init();
  if (actuator_init != 0) {
    Log.error("Actuator init failed code: %d" CR, actuator_init);
  }
  else {
    Log.verbose("Actuator init successful" CR);
  }


  logFile.close();
  Serial.println("Running diagnostic");
}

void loop() {
  Log.notice((actuator.diagnostic(false)).c_str());
  delay(1000);
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

