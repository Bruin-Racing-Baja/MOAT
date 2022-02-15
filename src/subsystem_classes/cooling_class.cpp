#include <Arduino.h>
#include <Cooling.h>
#include <ODrive.h>

// Cooling::Cooling()
// {
//     m_fan_enabled = false;
// }

void Cooling::init() {
    int huh = 4;
}

//NOTE: The current odrive implementation won't work due to reasons we do not understand
//This needs to be fixed before we can use this class to actually control the fan
void Cooling::stop_fan()
{
    //odrive->set_velocity(motor_number, 0);
}

void Cooling::set_fan_speed(int rpm)
//NOTE: Dont know how rpm -> velocity works, so should look into that
{
    //odrive->set_velocity(motor_number, rpm);
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
