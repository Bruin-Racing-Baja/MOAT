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
#define WAIT_SERIAL_STARTUP 1
#define HOME_ON_STARTUP 0
// NOTE: To set model 20 / 21 check the Constant.h file

// Constants Object
Constant constant;

// Logging
#define LOG_LEVEL LOG_LEVEL_NOTICE
#define SAVE_THRESHOLD 1000  // Sets how often the log object will save to SD when in operating mode

// Diagnostic Mode
#define DIAGNOSTIC_MODE_SHOTS 100  // Number of times diagnostic mode is run

//<--><--><--><-->< Base Systems ><--><--><--><--><-->

// Logging and SD
File log_file;
String log_name = "log.txt";

// Logging titles
unsigned int STATUS = 0;
unsigned int ACT_VEL = 1;
unsigned int ENC_IN = 2;
unsigned int ENC_OUT = 3;
unsigned int T_START = 4;
unsigned int T_STOP = 5;
unsigned int ENC_POS = 6;
unsigned int ODRV_VOLT = 7;
unsigned int ODRV_CUR = 8; 
unsigned int RPM = 9;
unsigned int ESTOP_IN = 10;
unsigned int ESTOP_OUT = 11;
unsigned int WHL_RPM = 12;
unsigned int WHL_COUNT = 13;
unsigned int RPM_COUNT = 14;
unsigned int DT = 15;
unsigned int ROLLING_FRAME = 16;
unsigned int EXP_DECAY = 17;
unsigned int REF_RPM = 18;
unsigned int ENC_VEL = 19;

//<--><--><--><-->< Subsystems ><--><--><--><--><-->
Cooling cooler_o(constant);

// Actuator settings
#define PRINT_TO_SERIAL false

// PINS CAR
#define ENC_A_PIN 2
#define ENC_B_PIN 3
#define HALL_INBOUND_PIN 23
#define HALL_OUTBOUND_PIN 22
#define GEARTOOTH_ENGINE_PIN 41
#define GEARTOOTH_GEARBOX_PIN 40

volatile unsigned long ext_eg_tooth_count = 0;
volatile unsigned long ext_gb_tooth_count = 0;

Actuator actuator(Serial1, constant, &ext_eg_tooth_count, &ext_gb_tooth_count, PRINT_TO_SERIAL);

// externally declared for interrupt
void external_count_eg_tooth(){
  ext_eg_tooth_count++;
}
void external_count_gb_tooth(){
  ext_gb_tooth_count++;
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
  // Serial.println("ESTOP PRESSED" + String(millis()));
}

void setup()
{
  Serial.println("Starting...");
  //-------------Attach E-Stop interrupt-----------------//
  // interrupts();
  // attachInterrupt(ESTOP_PIN, odrive_estop, RISING);

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

  //-------------Logging and SD Card-----------------
  int log_file_number = 0;
  while (SD.exists(("log_" + String(log_file_number) + ".txt").c_str()))
  {
    log_file_number++;
  }
  log_name = "log_" + String(log_file_number) + ".txt";
  Serial.println(constant.engine_geartooth_pin);
  Serial.println("Logging at: " + log_name);
  Serial.println(constant.gearbox_overdrive_rpm);

  log_file = SD.open(log_name.c_str(), FILE_WRITE);

  Log.begin(LOG_LEVEL, &log_file, false);
  Log.notice("Initialization Started" CR);
  // This is for the data analysis tool to be able to change the log order easily
  Log.verbose("Time: %d" CR, millis());

  save_log();

  //------------------ODrive------------------//

  //------------------Cooling------------------//

  cooler_o.init();

  //-------------Actuator-----------------//
  // General Init
  Serial.println("Actuator Init");
  int o_actuator_init = actuator.init(constant.homing_timeout);
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

  // Geartooth Interrupts
  Serial.println(constant.engine_geartooth_pin);
  Serial.println(constant.gearbox_geartooth_pin);
  pinMode(constant.engine_geartooth_pin, INPUT_PULLUP);
  pinMode(constant.gearbox_geartooth_pin, INPUT_PULLUP);
  attachInterrupt(constant.engine_geartooth_pin, external_count_eg_tooth, FALLING);
  attachInterrupt(constant.gearbox_geartooth_pin, external_count_gb_tooth, FALLING);

  // Homing
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
  // This message is critical as it sets the order that the analysis script will read the data in
  Log.notice("status, rpm, rpm_count, dt, act_vel, enc_pos, enc_vel, hall_in, hall_out, s_time, f_time, o_vol, o_curr, roll_frame, exp_decay, ref_rpm, therm1, therm2, therm3, estop" CR);
  save_log();
  Serial.println("Starting mode " + String(MODE));
}

// OPERATING MODE
#if MODE == 0

int o_control[30];
int save_count = 0;
int last_save = 0;
int o_return = 0;

void loop()
{
  // Report output with log
  actuator.control_function(o_control);
  
  if (o_control[STATUS] != 3)
  {
    // For log output format check log statement after log begins in init
    Log.notice("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %F, %F, %F, %F" CR, 
    o_control[STATUS], 
    o_control[RPM],
    o_control[RPM_COUNT],
    o_control[DT],
    o_control[ACT_VEL], 
    o_control[ENC_POS],
    o_control[ENC_VEL],
    o_control[ESTOP_IN], 
    o_control[ESTOP_OUT],
    o_control[T_START], 
    o_control[T_STOP], 
    o_control[ROLLING_FRAME],
    o_control[EXP_DECAY],
    o_control[REF_RPM],
    o_control[ODRV_VOLT], 
    o_control[ODRV_CUR] / 1000.0,
    cooler_o.get_thermistor(0), 
    cooler_o.get_thermistor(1), 
    cooler_o.get_thermistor(2)
    );
  }

  // Save data to sd every SAVE_THRESHOLD
  if (save_count > SAVE_THRESHOLD)
  {
    save_log();
    save_count = 0;
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
  //Log.notice((actuator.diagnostic(is_main_power, 10, true)).c_str());
  //Serial.print(cooler_o.diagnostic());
  actuator.move_back_and_forth_slowly();
  delay(100);
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