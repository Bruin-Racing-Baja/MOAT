#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <Arduino.h>
#include <SPI.h>
#include <ArduinoLog.h>
#include <Constant.h>
#include <Encoder.h>
#include <ODrive.h>
class Actuator
{
public:
  const static int k_enc_ppr = 88;
  const static int k_motor_number = 0;       // odrive axis
  const static int k_homing_timeout = 50e3;  // ms
  const static int k_min_rpm = 1000;         // rpm

  const int k_rpm_allowance = 30;

  constexpr static float k_linear_distance_per_rotation = 0.125;  // inches/rotation
  constexpr static float k_linear_shift_length = 3.5;             // inches
  constexpr static int32_t k_encoder_count_shift_length =
      (k_linear_shift_length / k_linear_distance_per_rotation) * 4 * 2048;
  constexpr static int k_eg_teeth_per_rotation = 88;

  // reference signals form tyler
  const unsigned int k_eg_idle = 1750;    // rpm
  const unsigned int k_eg_engage = 2100;  // rpm
  const unsigned int k_eg_launch = 2600;  // rpm
  const unsigned int k_eg_torque = 2700;  // rpm
  const unsigned int k_eg_power = 3400;   // rpm

  //  pins
  const int enc_a_pin = 2;
  const int enc_b_pin = 3;
  const int eg_tooth_pin = 41;
  const int hall_inbound_pin = 22;
  const int hall_outbound_pin = 23;

  Actuator(HardwareSerial& serial, Constant* constant_i, bool print_to_serial);

  int init(int odrive_timeout, void (*external_count_eg_tooth)());
  void control_function(int* status, int* rpm, int* actuator_velocity, int* inbound_triggered, int* outbound_triggered,
                        int* time_start, int* time_end, int* encoder_position, int* odrive_voltage, int* odrive_current,
                        int* error_out);
  int* homing_sequence(int* out);

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
  Constant* constant;

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
  int32_t m_encoder_outbound;  // out of the car
  int32_t m_encoder_inbound;   // towards the engine

  // Debugging vars
  int m_control_function_count;
  bool m_has_run;

  // running gear tooth sensor counts
  volatile unsigned long m_eg_tooth_count;
  unsigned long m_last_eg_tooth_count;
  float m_eg_rpm = 0;
  float m_currentrpm_eg_accum = 0;

  // running control terms
  int error = 0;
  int prev_error = 0;
};

#endif
