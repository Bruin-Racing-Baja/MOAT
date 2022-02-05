#include <Arduino.h>
#include <Cooling.h>

Cooling::Cooling()
{
    m_fan_enabled = false;
}

void Cooling::stop_fan()
{
    //Odrive Function Needed
}

void Cooling::set_fan_speed(int rpm)
{
    //Odrive Function Needed
}

float Cooling::thermo_check()
{
    int raw = analogRead(m_thermocouple_pin);
    float voltage = raw * (AREF / (pow(2, ADC_RESOLUTION) - 1));
    return (voltage - 1.25) / 0.005;
}

void Cooling::control_function()
{
    m_control_execution_count++;
    if (millis() - m_last_control_execution > m_cycle_period_millis)
    {
        float temperature = thermo_check();
        m_fan_enabled = thermo_check() > m_temp_threshold;
        if (m_fan_enabled)
        {
            set_fan_speed(m_cooling_rpm);
        }
        else
        {
            stop_fan();
        }
    }
}
