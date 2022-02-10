#ifndef COOLING_H
#define COOLING_H

#include <Arduino.h>

#include <ODrive.h>

#define AREF 3.3
#define ADC_RESOLUTION 10
#define motor_number 1  //ODrive axis

class Cooling
{
    static const double m_temp_threshold = 100; // C
    static const int m_thermocouple_pin = A0;   // C
    static const float m_voltage = 3.3;
    static const int m_cooling_rpm = 500;
    static const int m_cycle_period_millis = 10;

public:
    Cooling(ODrive*);

    void init();
    void control_function();

    float thermo_check();
    void stop_fan();
    void set_fan_speed(int rpm);

private:
    bool m_fan_enabled;
    unsigned long m_last_control_execution;
    unsigned long m_control_execution_count;
    ODrive *odrive;
};

#endif //! COOLING_H