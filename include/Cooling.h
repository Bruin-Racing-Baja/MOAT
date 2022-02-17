#ifndef COOLING_H
#define COOLING_H

#include <Arduino.h>
#include <ODrive.h>

#define AREF 3.3
#define ADC_RESOLUTION 10

class Cooling
{
    static const int k_motor_number = 1;
    static const double k_temp_threshold = 100; // C
    static const int k_thermocouple_pin = 38;   // C
    static const float k_voltage = 3.3;
    static const int k_cooling_rpm = 500;
    static const int k_cycle_period_millis = 10;

public:
    void init();
    void control_function();

    float get_temperature();
    void stop_fan();
    void set_fan_speed(int rpm);

private:
    bool m_fan_enabled;
    unsigned long m_last_control_execution;
    unsigned long m_control_execution_count;
};

#endif //! COOLING_H