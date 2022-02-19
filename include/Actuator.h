#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <Arduino.h>
#include <ArduinoLog.h>
#include <Encoder.h>
#include <ODrive.h>
class Actuator
{
public:
    const static int k_enc_ppr = 88;

    const static int k_motor_number = 0;       // odrive axis
    const static int k_homing_timeout = 50000; // ms
    const static int k_min_rpm = 1000;
    const static int k_cycle_period = 1e5; // us
    const int k_rpm_allowance = 30;

    const static float k_linear_distance_per_rotation = 0.125; // inches/rotation
    const static float k_linear_shift_length = 3.5;            // inches
    const static int32_t k_encoder_count_shift_length = (k_linear_shift_length / k_linear_distance_per_rotation) * 4 * 2048;
    const float k_cycle_period_minutes = (k_cycle_period / 1e6) / 60;
    const int k_cycle_period_millis = k_cycle_period / 1e4;
    const static int k_eg_teeth_per_rotation = 88;

    // reference signals form tyler
    const unsigned int k_eg_idle = 1750;
    const unsigned int k_eg_engage = 2100;
    const unsigned int k_eg_launch = 2600;
    const unsigned int k_eg_torque = 2700; // going for this one
    const unsigned int k_eg_power = 3400;
    const unsigned int k_desired_rpm = 2250;
    const float k_rpm_target_multiplier = 1.5;

    // pid constants
    const float k_p_gain = 0.015;

    Actuator(
        HardwareSerial &serial,
        const int enc_a_pin,
        const int enc_b_pin,
        const int eg_tooth_pin,
        const int hall_inbound_pin,
        const int hall_outbound_pin,
        bool print_to_serial);

    int init(int odrive_timeout, void (*external_count_eg_tooth)());
    int *control_function(int *out);
    int *homing_sequence(int *out);

    int get_status_code();
    int get_encoder_pos();
    float get_p_value();
    float communication_speed();
    void count_eg_tooth();
    String odrive_errors();

    String diagnostic(bool is_mainpower_on, bool serial_out);

private:
    int target_rpm();
    void test_voltage();
    int status;
    Encoder encoder;
    ODrive odrive;

    // Functions that get information from Odrive
    int get_encoder_count();

    // Functions that help calculate rpm
    unsigned long m_last_control_execution;
    float calc_engine_rpm(float dt);

    // Const
    bool m_print_to_serial;

    // member variables for sensor pins
    int m_eg_tooth_pin;
    int m_hall_inbound_pin;
    int m_hall_outbound_pin;

    // Encoder limit values
    int32_t m_encoder_outbound; // out of the car
    int32_t m_encoder_inbound;  // towards the engine

    // Debugging vars
    int m_control_function_count;
    bool m_has_run;

    // running gear tooth sensor counts
    volatile unsigned long m_eg_tooth_count;
    unsigned long m_last_eg_tooth_count;
    int m_eg_rpm;
};

#endif
