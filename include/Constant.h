#ifndef constant_h
#define constant_h

#include <SD.h>
#include <map>

struct Constant
{
  int init(bool m21); // Sets the model number
  // These constants change between models

  // Pins
  const int estop_pin;
  const int encoder_a_pin;
  const int encoder_b_pin;
  const int hall_inbound_pin;
  const int hall_outbound_pin;
  const int engine_geartooth_pin;
  const int gearbox_geartooth_pin;
  const int thermistor_1_pin;
  const int thermistor_2_pin;
  const int thermistor_3_pin;

  // Actuator Constants
  const int actuator_motor_number = 0;    // odrive axis
  const int homing_timeout = 50e6;        // ms
  const int cycle_period = 10;      // ms

  const float proportional_gain;
  const float integral_gain;
  const float derivative_gain;
  const float exponential_filter_alpha;

  // Constants

  // These constants do not change between models

  // Engine Constants
  const unsigned int engine_idle = 1750;      // rpm
  const unsigned int engine_engage = 2100;    // rpm
  const unsigned int engine_launch = 2600;    // rpm
  const unsigned int engine_torque = 2700;    // rpm
  const unsigned int engine_power = 3400;     // rpm
  const unsigned int desired_rpm = 2250;      // rpm
  const float rpm_target_multiplier = 1.5;

  const static int minimum_rpm = 1000;        // rpm

  

  // Linear Actuator Math
  constexpr static float linear_distance_per_rotation = 0.125;  // inches/rotation
  constexpr static float linear_shift_length = 3;             // inches
  constexpr static int32_t encoder_count_shift_length =
      (linear_shift_length / linear_distance_per_rotation) * 4 * 2048; //encoder count
  constexpr static float linear_engage_length = 1;  //inches
  constexpr static int32_t encoder_engage_dist = 
      (linear_engage_length / linear_distance_per_rotation) * 4 * 2048; //encoder count
  constexpr static float linear_engage_buffer = .2; // inches
  constexpr static int32_t encoder_engage_buffer = 
      (linear_engage_buffer) / linear_distance_per_rotation * 4 * 2048; //encoder count
  constexpr static float k_cycle_period_minutes = (cycle_period / 1e3) / 60;  // minutes
  constexpr static int k_eg_teeth_per_rotation = 88;
  

  private:

  std::map<String, int> m20_pins = {
    {"estop", 36},
    {"enc_a", 2},
    {"enc_b", 3},
    {"hall_inbound", 23},
    {"hall_outbound", 22},
    {"engine_geartooth", 41},
    {"gearbox_geartooth", 40},
    {"thermistor_1", 24},
    {"thermistor_2", 38},
    {"thermistor_3", 39}
  };

  std::map<String, float> m20_float_constants = {
    {"proportional_gain", 0.015},
    {"integral_gain", 0},
    {"derivative_gain", 0},
    {"exponential_filter_alpha", 0.5}
  };


  // void init(File settingsFile, bool m21, int defaultValueMode = 0);
  int mode;
  int desired_rpm;
  int log_num;

  float p;

  int isDefault;
  File settingsFile;

  int read_ints();
  int read_floats();
  int int_size = 2;
  int float_size = 1;
  int* ints[3] = { &mode, &desired_rpm, &log_num };
  float* floats[1] = { &p };
};

#endif