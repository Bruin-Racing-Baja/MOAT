#ifndef COOLING_H
#define COOLING_H

#include <Arduino.h>
#include <ODrive.h>
#include <Constant.h>

#define AREF 3.3
#define ADC_RESOLUTION 10

class Cooling
{
  // therm 1 - belt
  // therm 2 - secondary
  // therm 3 - primary

public:
  Cooling(Constant constant_in, HardwareSerial& serial);
  int control_function();
  float get_temperature();
  float get_thermistor(int thermistor_pin);
  void stop_fan();
  void start_fan();
  String diagnostic();
private:
  unsigned long m_last_control_execution;
  unsigned long m_control_execution_count;
  Constant constant;
  ODrive odrive;
};

#endif  //! COOLING_H