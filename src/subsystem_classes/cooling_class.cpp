#include <Arduino.h>
#include <Cooling.h>
#include <ODrive.h>

Cooling::Cooling(Constant constant_in, HardwareSerial& serial) : odrive(serial, constant_in)
{
  Constant constant = constant_in;
}

// NOTE: The current odrive implementation won't work due to reasons we do not understand
// This needs to be fixed before we can use this class to actually control the fan
void Cooling::stop_fan()
{
  odrive.run_state(constant.cooling_motor_number, 1, 0, 1);
}

void Cooling::start_fan()
{
  odrive.run_state(constant.cooling_motor_number, 8, 0, 1);
}

// float Cooling::get_temperature()
// {
//   int raw = analogRead(k_thermocouple_pin);
//   float voltage = raw * (AREF / (pow(2, ADC_RESOLUTION) - 1));
//   return (voltage - 1.25) / 0.005;
// }

float Cooling::get_thermistor(int thermistor_pin)
{
  int raw = analogRead(thermistor_pin);
  float R2 = 10000 * (1023.0 / raw - 1.0);
  float logR2 = log(R2);
  float T = 1 / (0.001129148 + 0.000234125 * logR2 + 0.0000000876741 * logR2 * logR2 * logR2);
  return T - 273.15;
}

int Cooling::control_function()
{
  m_control_execution_count++;
  if (millis() - m_last_control_execution > constant.cooling_cycle_period)
  {
    // 180 F (82.2 C) at secondary (therm 3)
    float temperature = Cooling::get_thermistor(constant.thermistor_3_pin);
    if (temperature > constant.cooling_target_temperature + constant.cooling_temperature_tolerance)
    {
      Cooling::start_fan();
      return 2;
    }
    else if (temperature < constant.cooling_target_temperature - constant.cooling_temperature_tolerance)
    {
      Cooling::stop_fan();
      return 1;
    }
    // Within temp tolerance
    return 3;
  }
  else return 0;
}

String Cooling::diagnostic(){
  String output = "";
  output += "Thermistor 1: " + String(get_thermistor(constant.thermistor_1_pin)) + "\n";
  output += "Thermistor 2: " + String(get_thermistor(constant.thermistor_2_pin)) + "\n";
  output += "Thermistor 3: " + String(get_thermistor(constant.thermistor_3_pin)) + "\n";
  return output;
}
