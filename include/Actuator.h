#include <Subsystem.h>
#include <Arduino.h>

#ifndef Actuator_h
#define Actuator_h

class Actuator{
    public:
    Actuator(HardwareSerial& serial, const int egTooth, const int gbTooth, const int inboundHall, const int outboundHall);
    void initialize();
    void control_function();
    int get_status_code();

    private:
    HardwareSerial& serial_;

    // Functions that command ODrive
    bool run_state(int axis, int requested_state, bool wait_for_idle, float timeout);
    void set_velocity(float velocity);

    // Functions that get information from Odrive
    int32_t read_int();
    String read_string();
    String dump_errors();

    // member variables for sensor pins
    int m_egTooth;
    int m_gbTooth;
    int m_inboundHall;
    int m_outboundHall;

    // Rando Constants
    const int m_motor_number = 0;
    const int status_code = 2;

    bool hasRun;
};

#endif