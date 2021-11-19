#include <Arduino.h>
#include <Encoder.h>

#ifndef actuator_h
#define actuator_h


class Actuator{
    public:
    Actuator(HardwareSerial& serial, const int enc_A, const int enc_B, const int egTooth, const int gbTooth, const int hall_inbound, const int hall_outbound, void (*external_interrupt_handler)());
    int init();
    void control_function();
    int get_status_code();
    int get_encoder_pos();


    private:
    int status;
    HardwareSerial& Serial;
    Encoder encoder;
    void (*m_external_interrupt_handler)();


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
    int m_hall_inbound;
    int m_hall_outbound;

    // Encoder limit values
    int m_encoder_outbound;
    int m_encoder_inbound;

    //Constants
    const int m_motor_number = 0;
    const int status_code = 2;
    int HOMING_TIMEOUT = 30000;
    //Can use either berh
    int CYCLE_FREQ = 1/5000;
    int CYCLE_TIME = 1/CYCLE_FREQ;
    static int control_function_count;
    bool hasRun;
};

#endif