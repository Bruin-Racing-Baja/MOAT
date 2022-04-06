#ifndef COOLING_H
#define COOLING_H

#include <Arduino.h>
#include <ODrive.h>

#define AREF 3.3
#define ADC_RESOLUTION 10

class Cooling
{
  static const int k_motor_number = 1;
  static const double k_temp_threshold = 100;  // C
  static const int k_thermocouple_pin = 38;    // C
  static const float k_voltage = 3.3;          // volts
  static const int k_cooling_rpm = 500;        // rpm
  static const int k_cycle_period = 10;        // ms
  static const int k_thermistor_pins[3] = {39, 40, 41};  // C

public:
  void init();
  void control_function();

  float get_temperature();
  float get_thermistor(int thermistor_num);
  void stop_fan();
  void set_fan_speed(int rpm);

private:
  bool m_fan_enabled;
  unsigned long m_last_control_execution;
  unsigned long m_control_execution_count;
};

#endif  //! COOLING_H