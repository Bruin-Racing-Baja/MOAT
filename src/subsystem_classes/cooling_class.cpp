#include <Arduino.h>
#include <Cooling.h>
#include <ODrive.h>

// Cooling::Cooling()
// {
//     m_fan_enabled = false;
// }

void Cooling::init()
{
}

// NOTE: The current odrive implementation won't work due to reasons we do not understand
// This needs to be fixed before we can use this class to actually control the fan
void Cooling::stop_fan()
{
  // odrive->set_velocity(motor_number, 0);
}

void Cooling::set_fan_speed(int rpm)
// NOTE: Dont know how rpm -> velocity works, so should look into that
{
  // odrive->set_velocity(motor_number, rpm);
}

float Cooling::get_temperature()
{
  int raw = analogRead(k_thermocouple_pin);
  float voltage = raw * (AREF / (pow(2, ADC_RESOLUTION) - 1));
  return (voltage - 1.25) / 0.005;
}

float Cooling::get_thermistor(int thermistor_num)
{
  if (thermistor_num < 0 || thermistor_num > sizeof(k_thermistor_pins))
  {
    return 4242;
  }
  int raw = analogRead(k_thermistor_pins[thermistor_num]);
  float R2 = 10000 * (1023.0 / raw - 1.0);
  float logR2 = log(R2);
  float T = 1 / (0.001129148 + 0.000234125 * logR2 + 0.0000000876741 * logR2 * logR2 * logR2);
  return T - 273.15;
}

void Cooling::control_function()
{
  m_control_execution_count++;
  if (millis() - m_last_control_execution > k_cycle_period)
  {
    float temperature = get_temperature();
    m_fan_enabled = temperature > k_temp_threshold;
    if (m_fan_enabled)
    {
      set_fan_speed(k_cooling_rpm);
    }
    else
    {
      stop_fan();
    }
  }
}
