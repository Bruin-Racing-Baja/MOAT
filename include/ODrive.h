#ifndef odrive_h
#define odrive_h

#include <Arduino.h>
#include <Encoder.h>

class ODrive{
    public:
    ODrive(
        HardwareSerial& serial,
        bool debug_i
    );
    int init(int timeout = 1000);
    bool run_state(int axis, int requested_state, bool wait_for_idle, float timeout);
    void set_velocity(int motor_number, float velocity);

    float get_vel(int motor_number);
    float get_voltage();
    int32_t read_int();
    String read_string();
    String dump_errors();
    float read_float();

    private:
    int status;
    HardwareSerial& OdriveSerial;
    bool debug;
};

#endif