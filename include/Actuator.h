#ifndef actuator_h
#define actuator_h

#include <Arduino.h>
#include <Encoder.h>

//CONSTANTS
#define enc_PPR 2048
#define motor_number 0  //Odrive axis
#define homing_timeout 50000 //NOTE: In ms
#define cycle_period 10e4 //In microseconds (10^-6 seconds) If u wanna use freq instead : 1/x=T
#define linearDistancePerRotation .125 //inches per rotation
#define linearShiftLength 2.75 //inches
const int32_t encoderCountShiftLength = (linearShiftLength/linearDistancePerRotation)*4*2048;
const float cycle_period_minutes = (cycle_period/1000000)/60; //the cycle period just in minutes
const int cycle_period_millis = cycle_period/10e3;
const int cycle_frequency_minutes = 1/cycle_period_minutes; 

#define egTeethPerRotation 2048

// reference signals from tyler
// ***** ENGINE ***** //
const unsigned int EG_IDLE = 1750;
const unsigned int EG_ENGAGE = 2100;
const unsigned int EG_LAUNCH = 2600;
const int EG_TORQUE = 2700; //<--- Going for this one 
const unsigned int EG_POWER = 3400; 

const float proportionalGain = .01; // gain of the p controller

class Actuator{
    public:
    Actuator(
        HardwareSerial& serial, 
        const int enc_A, 
        const int enc_B, 
        const int egTooth, 
        const int gbTooth, 
        const int hall_inbound, 
        const int hall_outbound,  
        void (*external_interrupt_handler)(), 
        void (*external_count_egTooth)(),
        bool printToSerial);
    int init(); 
    void control_function();
    int get_status_code();
    int get_encoder_pos();
    float communication_speed();
    void count_egTooth();

    private:
    int homing_sequence();
    void test_voltage();
    int status;
    HardwareSerial& OdriveSerial;
    Encoder encoder;
    void (*m_external_interrupt_handler)();
    
    


    // Functions that command ODrive
    bool run_state(int axis, int requested_state, bool wait_for_idle, float timeout);
    void set_velocity(float velocity);

    // Functions that get information from Odrive
    float get_vel();
    int get_encoder_count();
    float get_voltage();
    int32_t read_int();
    String read_string();
    String dump_errors();
    float read_float();

    //Functions that help calculate rpm
    unsigned long last_control_execution;
    void (*m_external_count_egTooth)();
    double calc_engine_rpm(float dt);

    // Const 
    bool m_printToSerial;

    // member variables for sensor pins
    int m_egTooth;
    int m_gbTooth;
    int m_hall_inbound;
    int m_hall_outbound;

    // Encoder limit values
    int32_t m_encoder_outbound; //out of the car
    int32_t m_encoder_inbound; //towards the engine

    //Debugging vars
    int control_function_count;
    bool hasRun;

    //running gear tooth sensor counts
    unsigned long egTooth_Count;
    unsigned long egTooth_Count_last;
    int currentrpm_eg;
};

#endif /*ACTUATOR_H*/