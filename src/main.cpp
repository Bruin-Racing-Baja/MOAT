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
// #include <Constant.h>
#include <Cooling.h>
#include <Radio.h>

// Most settings are located in the SD card settings file, and can be changed there
#define MODE 0

#define DIAGNOSTIC_MODE_SHOTS 100
// Startup
#define WAIT_SERIAL_STARTUP_SD 0  // Set to stop program before the sd card initialization

//<--><--><--><-->< Base Systems ><--><--><--><--><-->
// ODrive Settings
#define ODRIVE_STARTING_TIMEOUT 1000  // [ms]

// LOGGING AND SD SETTINGS
File log_file;
// Create constant control object to read from sd
Constant constant;

//<--><--><--><-->< Subsystems ><--><--><--><--><-->
Cooling cooler_o(&constant);

// Acuator settings
#define PRINT_TO_SERIAL false

Actuator actuator(Serial1, &constant, PRINT_TO_SERIAL);

// externally declared for interrupt
void external_count_eg_tooth()
{
  actuator.count_eg_tooth();
}
void external_count_gb_tooth()
{
  actuator.count_gb_tooth();
}

// NOTE: May want to test expanding this function to take in a file object to save
void save_log()
{
  // Closes and then opens the file stream
  log_file.close();
  log_file = SD.open(constant.log_name.c_str(), FILE_WRITE);
}

void setup()
{
  if (WAIT_SERIAL_STARTUP_SD)
  {
    while (!Serial)  // wait for serial port to connect, before SD card init
    {
    }
  }

  //-------------Initializing SD and Loading Settings-----------------
  if (!SD.begin(BUILTIN_SDCARD))
  {
    // This means that no SD card was found or there was an error with it
    // We will halt the program
    Serial.println("No SD card found, halting");
    while (1)
    {
    }
  }
  else
  {
    if (SD.exists("settings.txt"))
    {
      // This means the settings file exists, so we will load it
      // This is where the bulk of the development for this feature will occur as we need to read in certain values,
      // then set them in the program accordingly
      File settingFile = SD.open("settings.txt", FILE_READ);
      while (settingFile.available())
      {
        settingFile.readStringUntil('$');  // This removes the comments in the beginning of the file
        constant.init(settingFile);        // Creates the constant object
      }
      settingFile.close();
      Log.verbose(constant.get_values().c_str());
    }
    else
    {
      // This means the settings file does not exist, but there is an SD card present
      // WIll halt the program
      Serial.println("No settings file found, halting");
      while (1)
      {
      }
    }
    //-------------Log file splitting-----------------
    int log_file_number = 0;
    while (SD.exists(("/logs/log_" + String(log_file_number) + ".txt").c_str()))
    {
      log_file_number++;
    }
    constant.log_name = "/logs/log_" + String(log_file_number) + ".txt";
  }
  //-------------Loading Settings-----------------
  // This implementation will not work, but it is still here as we may pursue this in the future
  // MODE = constant.mode;

  //-------------Wait for serial-----------------//
  if (constant.wait_serial_startup)
  {
    while (!Serial)  // wait for serial port to connect after SD has been read
    {
    }
  }

  //-------------Logging and SD Card-----------------

  log_file = SD.open(constant.log_name.c_str(), FILE_WRITE);

  Log.begin(constant.log_level, &log_file, false);
  Log.notice("Initialization Started" CR);
  Log.verbose("Time: %d" CR, millis());

  save_log();

  //------------------Cooling------------------//

  cooler_o.init();

  //-------------Actuator-----------------//
  // TODO: handle duplicate logs
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
  if (constant.home_on_startup)
  {
    int o_homing[3];
    actuator.homing_sequence(o_homing);
    if (o_homing[0])
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
  if (MODE == 0)
  {
    Log.notice(
        '"status", "rpm", "actuator_velocity", "encoder_position", "fully_shifted_in", "fully_shifted_out", "control_start_time", "control_stop_time", "odrive_voltage", "odrive_current", "cooler_temperature"');
  }
  save_log();
}

// OPERATING MODE
#if MODE == 0

int o_control[11];

// Define pointers to the returned values to increase readability
int* status = &o_control[0];
int* rpm = &o_control[1];
int* actuator_velocity = &o_control[2];
int* inbound_triggered = &o_control[3];
int* outbound_triggered = &o_control[4];
int* time_start = &o_control[5];
int* time_end = &o_control[6];
int* encoder_position = &o_control[7];
int* odrive_voltage = &o_control[8];
int* odrive_current = &o_control[9];
int* error = &o_control[10];
int* wheel_rpm = &o_control[11];

int save_count = 0;
int last_save = 0;
int* status = &o_control[0];
void loop()
{
  // Main control loop, with actuator
  actuator.control_function(status, rpm, actuator_velocity, inbound_triggered, outbound_triggered, time_start, time_end,
                            encoder_position, odrive_voltage, odrive_current, error, wheel_rpm);
  //<status, rpm, actuator_velocity, inbound_triggered, outbound_triggered, time_started, time_finished, enc_position>

  // Report output with log
  if (*status != 3)
  {
    Log.notice("%d, %d, %d, %d, %d, %d, %d, %d, %d, %u, %d, %d" CR, *status, *rpm, *actuator_velocity, *encoder_position,
               *inbound_triggered, *outbound_triggered, *time_start, *time_end, *odrive_voltage,
               (*odrive_current * 1000.0), cooler_o.get_temperature(), *wheel_rpm);
  }

  // Save data to sd every SAVE_THRESHOLD
  if (save_count > constant.save_threshold)
  {
    int save_start = millis();
    // Log.notice(actuator.odrive_errors().c_str());
    save_log();
    save_count = 0;
    if (!(o_control[8] > 20))
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }
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
bool is_main_power = false;

void loop()
{
  // Assumes main power isnt connected as connected to serial
  Log.notice((actuator.diagnostic(is_main_power, true)).c_str());
  Log.notice("Thermo temp: %d" CR, cooler_o.get_temperature());
  Serial.println(cooler_o.get_temperature());
  // Serial.println(analogRead(38));
  delay(100);
}

// HEADLESS HORSEMAN MODE
#elif MODE == 3
int temp = 0;

void loop()
{
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