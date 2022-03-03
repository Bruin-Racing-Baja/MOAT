#ifndef constant_h
#define constant_h

#include <SD.h>
#include <string>

struct Constant
{
  void init(File settingsFile, int defaultValueMode = 0);
  int isDefault;
  String get_values();

  // General
  int mode;
  int home_on_startup;
  int wait_serial_startup;

  // Log
  int log_level;
  int save_threshold;
  String log_name = "/logs/log_default.txt";

  // Actuator
  int k_actuator_cycle_period = 10;  // ms

  // PID
  float k_proportional_gain = .015;  // gain of the p controller
  float k_derivative_gain = .0;      //.005;
  float k_integral_gain = .0001;
  float k_exp_filt_alpha = 0.5;

  // Dynamic target RPM
  int k_desired_rpm = 2250;  // rpm

  // Cooling
  int k_temp_threshold = 100;       // C
  int k_cooling_rpm = 500;          // rpm
  int k_cooling_cycle_period = 10;  // ms

private:
  File settingsFile;

  int read_ints();
  int read_floats();
  int int_size = 10;
  int float_size = 4;
  int* ints[10] = {
    &mode,          &home_on_startup,  &wait_serial_startup,    &log_level,     &save_threshold,
    &k_cooling_rpm, &k_temp_threshold, &k_cooling_cycle_period, &k_desired_rpm, &k_actuator_cycle_period
  };

  float* floats[4] = { &k_proportional_gain, &k_integral_gain, &k_derivative_gain, &k_exp_filt_alpha };
};

#endif