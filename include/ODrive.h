#ifndef ODRIVE_H
#define ODRIVE_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ODrive.h>
#include <SoftwareSerial.h>

class ODrive
{
public:
  const uint32_t k_read_timeout = 1000;  // ms
  // TODO make static
  String k_system_errors[64];
  String k_can_errors[64];
  String k_axis_errors[64];
  String k_motor_errors[64];
  String k_controller_errors[64];
  String k_encoder_errors[64];
  String k_sensorless_estimator_errors[64];
  ODrive(HardwareSerial& serial);

  int init(uint32_t timeout = 1000);

  String dump_errors();
  String dump_data_header();
  String dump_data();

  void set_velocity_gain(int axis, float vel_gain);
  void set_velocity_integrator_gain(int axis, float vel_integrator_gain);

  bool run_state(int axis, int requested_state, bool wait_for_idle, uint32_t timeout);
  void set_velocity(int axis, float velocity);

  float get_motor_current(int axis);
  float get_torque_estimate(int axis);
  float get_velocity(int axis);

  float get_voltage();
  float get_current();

private:
  int m_status;
  HardwareSerial& m_odrive_serial;

  // TODO check if all 64 bit integers work/are necessary
  uint64_t string_to_ull(String s);
  String ull_to_string(uint64_t n);

  String read_string();
  int read_int();
  uint64_t read_ull();
  float read_float();

  String dump_single_error(String error_lut[], String module, int axis, uint64_t error_code, bool indent = true);
};

#endif