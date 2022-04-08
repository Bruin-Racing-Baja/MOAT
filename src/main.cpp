/*
MOAT - Model 2O Awesome Teensy
mk. V.0
General code to oversee all functions of the Teensy
*/

#include <Arduino.h>

// Libraries
#include <SPI.h>  // MUST BE INCLUDED BEFORE ArduinoLog.h
#include <ArduinoLog.h>
#include <HardwareSerial.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <string>

// Classes
#include <Actuator.h>
#include <Constant.h>
#include <Cooling.h>
#include <Radio.h>

// Modes
/*
 * Operating (0): For normal operation. Initializes then runs main control function in loop.
 * May disable logging object in its library config to free up memory.
 *
 * Headless Diagnostic (1): Runs diagnostic test 100 times, then stops, saves to SD card.
 * This is used for testing in sitations where the main power is on, and data will be retrieved from SD.
 * NOTE: DO NOT CONNECT TO TEENSY WHEN MAIN POWER IS ON OR IT COULD DAMAGE YOUR LAPTOP
 *
 * Serial Diagnostic (2): Runs diagnostic test continuously, prints to serial.
 *
 * Headless Horseman (3): For operation where data is not recorded.
 * This *may* increase performance, but at least helps to ensure SD card doesnt fill and mess thing up.
 * NOTE: Recommend disabling logging object in its include
 *
 * Wave (4): Operates the actuator in a wave pattern while logging data to the serial monitor
 * This will go from software stop to software stop continuously to hceck ay errors with the odrive or teensy
 * 
 */
#define MODE 0

// Startup
#define WAIT_SERIAL_STARTUP 0  // Set headless mode or not
#define HOME_ON_STARTUP 1
//#define RUN_DIAGNOSTIC_STARTUP 0
#define ESTOP_PIN 34
// Logging
// NOTE: By default the log requires and outputs to the SD card (can be changed in setup)
#define LOG_LEVEL LOG_LEVEL_NOTICE
#define SAVE_THRESHOLD 1000  // Sets how often the log object will save to SD when in operating mode

// Actuator

// Diagnostic Mode
#define DIAGNOSTIC_MODE_SHOTS 100  // Number of times diagnostic mode is run

//<--><--><--><-->< Base Systems ><--><--><--><--><-->
// ODrive Settings
#define ODRIVE_STARTING_TIMEOUT 1000  // [ms]

// Pins

// Create objects
// ODrive odrive(Serial1);

// LOGGING AND SD SETTINGS
File log_file;
String log_name = "log.txt";
// Create constant control object to read from sd
Constant constant;

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
#define HALL_INBOUND_PIN 23
#define HALL_OUTBOUND_PIN 22
#define GEARTOOTH_ENGINE_PIN 41
#define GEARTOOTH_GEARBOX_PIN 40

Actuator actuator(Serial1, ENC_A_PIN, ENC_B_PIN, GEARTOOTH_ENGINE_PIN, GEARTOOTH_GEARBOX_PIN ,HALL_INBOUND_PIN, HALL_OUTBOUND_PIN,
                  PRINT_TO_SERIAL);

// externally declared for interrupt
void external_count_eg_tooth()
{
  actuator.count_eg_tooth();
}
void external_count_gb_tooth(){
  actuator.count_gb_tooth();
}



void save_log()
{
  // Closes and then opens the file stream
  log_file.close();
  log_file = SD.open(log_name.c_str(), FILE_WRITE);
}

bool estop_pressed = 0;

// Set flag and turn on LED if the estop is ever pressed
void odrive_estop()
{
  estop_pressed = 1;
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("ESTOP PRESSED" + String(millis()));
}

void setup()
{
  Serial.println("Starting...");
  //-------------Attach E-Stop interrupt-----------------//
  interrupts();
  attachInterrupt(ESTOP_PIN, odrive_estop, RISING);

  //-------------Wait for serial-----------------//
  if (WAIT_SERIAL_STARTUP)
  {
    while (!Serial)  // wait for serial port to connect. Needed for native USB port only
    {
    }
  }

  //-------------Initializing SD and Loading Settings-----------------
  if (!SD.begin(BUILTIN_SDCARD))
  {
    // This means that no SD card was found or there was an error with it
    // In this case, we will switch to the headless horseman mode and continue to operate with no logging
    // This behaviour is arbitrary, and may be changed in the future
    // constant.init(nullptr, 3);
  }
  else
  {
    if (SD.exists("settings.txt"))
    {
      // This means the settings file exists, so we will load it
      // This is where the bulk of the development for this feature will occur as we need to read in certain values,
      // then set them in the program accordingly
      // File settingFile = SD.open("settings.txt", FILE_READ);
      // while (settingFile.available())
      // {
      //   settingFile.readStringUntil('$');  // This removes the comments in the beginning of the file
      //   // constant.init(settingFile);        // Creates the constant object
      // }
    }
    else
    {
      // This means the settings file does not exist, but there is an SD card present
      // In this case, we will operate in headless diagnostic mode as we dont know the user intention
      // constant.init(nullptr, 1);
    }
  }

  //-------------Logging and SD Card-----------------
  int log_file_number = 0;
  while (SD.exists(("log_" + String(log_file_number) + ".txt").c_str()))
  {
    log_file_number++;
  }
  log_name = "log_" + String(log_file_number) + ".txt";
  Serial.println("Logging at: " + log_name);

  log_file = SD.open(log_name.c_str(), FILE_WRITE);

  Log.begin(LOG_LEVEL, &log_file, false);
  Log.notice("Initialization Started" CR);
  Log.verbose("Time: %d" CR, millis());

  save_log();

  //------------------ODrive------------------//

  // At this time the following code is depricated, but until we make final decisions about the odrive class, we will
  // leave it in

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
  // log_file.close();
  // log_file = SD.open("log.txt", FILE_WRITE);

  //------------------Cooling------------------//

  cooler_o.init();

  //-------------Actuator-----------------//
  // General Init
  int o_actuator_init = actuator.init(ODRIVE_STARTING_TIMEOUT, external_count_eg_tooth, external_count_gb_tooth);
  if (o_actuator_init)
  {
    Log.error("Actuator Init Failed code: %d" CR, o_actuator_init);
    Serial.println("Actuator init failed code: " + String(o_actuator_init));
  }
  else
  {
    Log.verbose("Actuator Init Success code: %d" CR, o_actuator_init);
    Log.notice("Proportional gain (x1000): %d" CR, (1000.0 * actuator.get_p_value()));
    Serial.println("Actuator init success code: " + String(o_actuator_init));
  }
  save_log();

  // Homing if enabled
  if (HOME_ON_STARTUP)
  {
    int o_homing[3];
    actuator.homing_sequence(o_homing);
    Serial.println("Homing sequence: " + String(o_homing[0]) + " " + String(o_homing[1]) + " " + String(o_homing[2]));
    if (o_homing[0] != 0)
    {
      Log.error("Homing Failed code: %d" CR, o_homing[0]);
    }
    else
    {
      Log.verbose("Homing Success code: %d" CR, o_homing[0]);
      Log.notice("Homing results, inbound: %d, outbound: %d" CR, o_homing[1], o_homing[2]);
    }
  }
  Log.verbose("Initialization Complete" CR);
  Log.notice("Starting mode %d" CR, MODE);
  Log.notice("status, rpm, act_vel, enc_pos, in_trig, out_trig, s_time, f_time, o_vol, o_curr, couple, wheel_speed, estop" CR);
  save_log();
  Serial.println("Starting mode " + String(MODE));
// "status", 
// "rpm", 
// "act_vel", 
// "enc_pos", 
// "in_trig", 
// "out_trig", 
// "s_time", 
// "f_time", 
// "o_vol", 
// "o_curr",
// "couple",
// "therm1",
// "therm2",
// "therm3",
// "estop",
// "wheel_count",
// "wheel_rpm",

}

// OPERATING MODE
#if MODE == 0

int o_control[15];
int save_count = 0;
int last_save = 0;
void loop()
{
  // Main control loop, with actuator
  actuator.control_function(o_control);
  //<status, rpm, actuator_velocity, inbound_triggered, outbound_triggered, time_started, time_finished, enc_position>
  // Report output with log
  if (o_control[0] == 3)
  {
    Log.verbose("Status: %d  RPM: %d, Act Vel: %d, Enc Pos: %d, Inb Trig: %d, Otb Trig: %d, Start: %d, End: %d, Wheel Speed: %d" CR,
                o_control[0], o_control[1], o_control[2], o_control[7], o_control[3], o_control[4], o_control[5],
                o_control[6], o_control[11]);
  }
  else
  {
    // Log.notice("Status: %d  RPM: %d, Act Vel: %d, Enc Pos: %d, Inb Trig: %d, Otb Trig: %d, Start: %d, End: %d,
    // Voltage: %d" CR,
    //   o_control[0], o_control[1], o_control[2], o_control[7], o_control[3], o_control[4], o_control[5], o_control[6],
    //   o_control[8]);
    // Log.notice("Temperature (*C): %d" CR, cooler_o.thermo_check());
    Log.notice("%d, %d, %d, %d, %d, %d, %d, %d, %F, %F, %d, %d, %d" CR, o_control[0], o_control[1], o_control[2], o_control[7],
               o_control[3], o_control[4], o_control[5], o_control[6], o_control[8], (o_control[9] * 1000.0),
               cooler_o.get_temperature(), o_control[11], estop_pressed);
    estop_pressed = 0;
  }

  // Save data to sd every SAVE_THRESHOLD
  if (save_count > SAVE_THRESHOLD)
  {
    // int save_start = millis();
    // // Log.notice(actuator.odrive_errors().c_str());
    // Log.verbose("Time since last save: %d" CR, save_start - last_save);
    save_log();
    save_count = 0;
    // Log.verbose("Battery level ok? %d", o_control[8] > 20);
    // digitalWrite(LED_BUILTIN, !(o_control[8] > 20));  // TURN LED ON IF BATTERY TOO LOW
    // Log.verbose("Saved log in %d ms" CR, millis() - save_start);
  }
  save_count++;
}

// HEADLESS DIAGNOSTIC MODE
#elif MODE == 1
bool is_main_power = true;

void loop()
{
  Log.notice("DIAGNOSTIC MODE" CR);
  Log.verbose("Running diagnostic function" CR);

  for (int i = 0; i < DIAGNOSTIC_MODE_SHOTS; i++)
  {
    // Serial.println(actuator.diagnostic(false));
    Log.notice("%d", i);
    // Assumes main power is connected
    Log.notice((actuator.diagnostic(is_main_power, false)).c_str());
    Log.notice("Thermocouple temp: %d", cooler_o.get_temperature());
    delay(100);
  }

  Log.verbose("End of diagnostic function" CR);
  log_file.close();
  exit(0);
}

// SERIAL DIAGNOSTIC MODE
#elif MODE == 2
bool is_main_power = true;

void loop()
{
  // Assumes main power isnt connected as connected to serial
  Log.notice((actuator.diagnostic(is_main_power, true)).c_str());
  Log.notice("Thermo temp: %d" CR, cooler_o.get_temperature());
  Serial.println(cooler_o.get_temperature());
  // Serial.println(analogRead(38));
  delay(1000);
}

// HEADLESS HORSEMAN MODE
#elif MODE == 3
int temp = 0;

void loop()
{
  // temp = cooler_o.thermo_check();
  Serial.println(temp);
  delay(100);

  // digitalWrite(LED_BUILTIN, digitalRead(hall_inbound));
  // Serial.println(digitalRead(hall_inbound));
  // Serial.println("huh");
  // delay(1000);
}

// Wave Mode
#elif MODE == 4
int shift_o;
void loop() {
  shift_o = actuator.fully_shift(true, 10000);
  Serial.println("Shifted in, status: " + String(shift_o));
  Serial.println("Estop pressed: " + String(estop_pressed));
  estop_pressed = 0;
  actuator.diagnostic(true, true);
  shift_o = actuator.fully_shift(false, 10000);
  Serial.println("Shifted out, status: " + String(shift_o));
  Serial.println("Estop pressed: " + String(estop_pressed));
  estop_pressed = 0;
  actuator.diagnostic(true, true);
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