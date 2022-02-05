#include <Arduino.h>
#include <Encoder.h>

class ODrive{
    public:
    ODrive(
        HardwareSerial& serial
        
    );
    int init(int timeout = 1000);
    bool run_state(int axis, int requested_state, bool wait_for_idle, float timeout);
    void set_velocity();

    float get_vel(int motor_number);
    float get_voltage();
    int32_t read_int();
    String read_string();
    String dump_errors();
    float read_float();

    private:
    int status;
    HardwareSerial& OdriveSerial;

    float get_voltage();
};