#ifndef odrive_h
#define odrive_h

#include <Arduino.h>
#include <Constant.h>
#include <HardwareSerial.h>
#include <ODrive.h>
#include <SoftwareSerial.h>

class ODrive
{
public:
  ODrive(HardwareSerial& serial, Constant constant);
  int init(int timeout);
  bool run_state(int axis, int requested_state, bool wait_for_idle, float timeout);
  void set_velocity(int motor_number, float velocity);
  float update_velocity(int motor_number, float velocity);
  float get_encoder_pos(int motor_number);
  float get_vel(int motor_number);
  float get_voltage();
  void get_voltage_current_encoder(int motor_number, float *voltage, float *current, int *encoder_count, float *encoder_vel);
  int32_t read_int();
  String read_string();
  String dump_errors();
  float read_float();
  float get_cur();

private:
  int m_current_state = -1;
  int status;
  float m_current_velocity = 0.0f;
  HardwareSerial& OdriveSerial;
  Constant constant;
  float get_voltage_private();
};

#endif