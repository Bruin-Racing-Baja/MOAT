/* 
MOAT - Model 2O Awesome Teensy
mk. V.0
General code to oversee all functions of the Teensy
*/

#include <Arduino.h>

//Libraries
#include <SPI.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <string>
#include <ArduinoLog.h>
#include <SD.h>

//Classes
#include <Actuator.h>
#include <Radio.h>
#include <Cooling.h>

//General Settings
  //Mode
  /*
  Operating - 0 - For normal operation, initializes then runs main control fn in loop
    May disable logging object in its library config to free up memory, if relevant

  Headless Diagnostic - 1 - Runs diagnostic test 100 times, then stops, saves to sd
    DO NOT CONNECT TO TEENSY WHEN MAIN POWER IS ON OR IT COULD DAMAGE YOUR LAPTOP
    This is used for testing in sitations where the main power is on, and data will be retrieved from sd

  Serial Diagnostic - 2 - Runs diagnostic test continuously, prints to serial

  Headless Horseman - 3 - For operation where data is not recorded
    This *may* increase performance, but at least helps to ensure SD card doesnt fill and mess thing up
    NOTE: Recommend disabling logging object in its include

  */
  #define MODE 0

  //Startup
  #define WAIT_SERIAL_STARTUP 0  //Set headless mode or not
  #define HOME_ON_STARTUP 1
  //#define RUN_DIAGNOSTIC_STARTUP 0

  //Log
  #define LOG_LEVEL LOG_LEVEL_NOTICE
  #define SAVE_THRESHOLD 1000 //Sets how often the log object will save to sd when in operating mode
  //Note: By default the log requires and outputs to the SD card, and can be changed in setup

  //Actuator


  //Diagnostic Mode
  #define DIAGNOSTIC_MODE_SHOTS 100  //Number of times diagnostic mode is run

//<--><--><--><-->< Base Systems ><--><--><--><--><-->

//ODrive Settings
  #define odrive_starting_timeout 1000 //NOTE: In ms

  //PINS

  //CREATE OBJECT
  // ODrive odrive(Serial1);

//LOGGING AND SD SETTINGS
  //Create file to log to
  File logFile;
  //Cannot create logging object until init

//<--><--><--><-->< Sub-Systems ><--><--><--><--><-->
//COOLING SETTINGS
  Cooling cooler_o;

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
  #define hall_inbound 22
  #define hall_outbound 23
  #define gearTooth_engine 41
  #define gearTooth_gearbox 40


  //CREATE OBJECT

  static void external_count_egTooth();

  Actuator actuator(Serial1, enc_A, enc_B, gearTooth_engine, 0, hall_inbound, hall_outbound, &external_count_egTooth, PRINTTOSERIAL);

  static void external_count_egTooth(){
    actuator.count_egTooth();
  }
  
//FREE FUNCTIONS

//NOTE: May want to test expanding this function to take in a file object to save
void save_log() {
  //Closes and then opens the file stream
  logFile.close();
  logFile = SD.open("log.txt", FILE_WRITE);
}

void setup() {
  //-------------Wait for serial-----------------
  if(WAIT_SERIAL_STARTUP) {
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  }

  //-------------Loading and Initializing SD-----------------
  if (!SD.begin(BUILTIN_SDCARD)) {
    //This means that no SD card was found or there was an error with it
    //In this case, we will switch to the headless horseman mode and continue to operate with no logging
    //This behaviour is arbitrary, and may be changed in the future
    #define MODE 3
  }
  if (SD.exists("settings.txt")) {
    //This means the settings file exists, so we will load it
    //This is where the bulk of the development for this feature will occur as we need to read in certain values, then set them in the program accordingly
    
    
    } else {
    //This means the settings file does not exist, but there is an SD card present
    //In this case, we will operate in headless diagnostic mode as we dont know the user intention
    #define MODE 1
    //Set to completely headless operation if no SD card is present when trying to run the car
  }


  //-------------Logging and SD Card-----------------
  
  logFile = SD.open("log.txt", FILE_WRITE);

  Log.begin(LOG_LEVEL, &logFile, false);
  Log.notice("Initialization Started" CR);
  Log.verbose("Time: %d" CR, millis());

  save_log();

  //------------------Odrive------------------

  //At this time the following code is depricated, but until we make final decisions about the odrive class, we will leave it in

  // int odrive_init = actuator.odrive.init(1000);
  // if (odrive_init) {
  //   Log.error("ODrive Init Failed code: %d" CR, odrive_init);
  // }
  // else {
  //   Log.verbose("ODrive Init Success code: %d" CR, odrive_init);
  // }
  // Serial.println("ODrive init: " + String(odrive_init));
  // for (int i = 0; i < 20; i++) {
  //   Serial.println(odrive.get_voltage());
  // }
  // logFile.close();
  // logFile = SD.open("log.txt", FILE_WRITE);

  //------------------Cooling------------------

  cooler_o.init();
  //-------------Actuator-----------------
  //General Init
  int o_actuator_init = actuator.init(odrive_starting_timeout);
  if(o_actuator_init) {
    Log.error("Actuator Init Failed code: %d" CR, o_actuator_init);
    Serial.println("Actuator init failed code: " + String(o_actuator_init));
  }
  else {
    Log.verbose("Actuator Init Success code: %d" CR, o_actuator_init);
    Log.notice("Proportional gain (x1000): %d" CR, (1000.0 * actuator.get_p_value()));
    Serial.println("Actuator init success code: " + String(o_actuator_init));
  }
  save_log();
  //Homing if enabled
  if (HOME_ON_STARTUP) {
    int o_homing[3];
    actuator.homing_sequence(o_homing);
    if (o_homing[0]) {
      Log.error("Homing Failed code: %d" CR, o_homing[0]);
    }
    else {
      Log.verbose("Homing Success code: %d" CR, o_homing[0]);
      Log.notice("Homing results, inbound: %d, outbound: %d" CR, o_homing[1], o_homing[2]);
    }
  }
  Log.verbose("Initialization Complete" CR);
  Log.notice("Starting mode %d" CR, MODE);
  save_log();
}

//OPERATING MODE
#if MODE == 0

int o_control[10];
int save_count = 0;
int last_save = 0;
void loop() {
  //Main control loop, with actuator
  actuator.control_function(o_control);
  //<status, rpm, actuator_velocity, inbound_triggered, outbound_triggered, time_started, time_finished, enc_position>

  //Report output with log
  if (o_control[0] == 3){
    Log.verbose("Status: %d  RPM: %d, Act Vel: %d, Enc Pos: %d, Inb Trig: %d, Otb Trig: %d, Start: %d, End: %d" CR,
    o_control[0], o_control[1], o_control[2], o_control[7], o_control[3], o_control[4], o_control[5], o_control[6]);
  }
  else {
    // Log.notice("Status: %d  RPM: %d, Act Vel: %d, Enc Pos: %d, Inb Trig: %d, Otb Trig: %d, Start: %d, End: %d, Voltage: %d" CR,
    //   o_control[0], o_control[1], o_control[2], o_control[7], o_control[3], o_control[4], o_control[5], o_control[6], o_control[8]);
    // Log.notice("Temperature (*C): %d" CR, cooler_o.thermo_check());
    Log.notice("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d" CR,
    o_control[0], o_control[1], o_control[2], o_control[7], o_control[3], o_control[4], o_control[5], o_control[6], o_control[8], (o_control[9] * 1000.0), cooler_o.thermo_check());
  }
  
  //Save data to sd every SAVE_THRESHOLD
  if (save_count > SAVE_THRESHOLD) {
    int save_start = millis();
    Log.notice(actuator.odrive_errors().c_str());
    Log.verbose("Time since last save: %d" CR, save_start - last_save);
    save_log();
    save_count = 0;
    Log.verbose("Battery level ok? %d", o_control[8] > 20);
    digitalWrite(LED_BUILTIN, !(o_control[8] > 20)); //TURN LED ON IF BATTERY TOO LOW
    Log.verbose("Saved log in %d ms" CR, millis() - save_start);
  }
  save_count++;
}

//HEADLESS DIAGNOSTIC MODE
#elif MODE == 1
bool is_main_power = true;

void loop() {
  Log.notice("DIAGNOSTIC MODE" CR);
  Log.verbose("Running diagnostic function" CR);

  for (int i = 0; i < DIAGNOSTIC_MODE_SHOTS; i++) {
    //Serial.println(actuator.diagnostic(false));
    Log.notice("%d", i);
    //Assumes main power is connected
    Log.notice((actuator.diagnostic(is_main_power, false)).c_str());
    Log.notice("Thermocouple temp: %d", cooler_o.thermo_check());
    delay(100);
  }

  Log.verbose("End of diagnostic function" CR);
  logFile.close();
  exit(0);
}

//SERIAL DIAGNOSTIC MODE
#elif MODE == 2
bool is_main_power = false;

void loop() {
  //Assumes main power isnt connected as connected to serial
  Log.notice((actuator.diagnostic(is_main_power, true)).c_str());
  Log.notice("Thermo temp: %d" CR, cooler_o.thermo_check());
  Serial.println(cooler_o.thermo_check());
  //Serial.println(analogRead(38));
  delay(100);
}

//HEADLESS HORSEMAN MODE
#elif MODE == 3
int temp = 0;

void loop() {
  temp = cooler_o.thermo_check();
  Serial.println(temp);
  delay(100);

  // digitalWrite(LED_BUILTIN, digitalRead(hall_inbound));
  // Serial.println(digitalRead(hall_inbound));
  // Serial.println("huh");
  // delay(1000);
}

#endif


/*
//"When you join the MechE club thinking you could escape the annoying CS stuff like pointers and interrupts"
//                             __...------------._
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

