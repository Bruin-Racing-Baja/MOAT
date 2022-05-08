#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <Arduino.h>
#include <SPI.h>
#include <ArduinoLog.h>
#include <Constant.h>
#include <Encoder.h>
#include <queue>
#include <ODrive.h>

class Actuator
{
public:
  const static int k_enc_ppr = 88;

  const int k_rpm_allowance = 30;

  //LOG OUTPUTS  
  //<status, rpm, actuator_velocity, fully shifted in, fully shifted out, time_started, time_finished, enc_pos,
  //odrive_volt, odrive_current>
  unsigned int STATUS = 0;
  unsigned int ACT_VEL = 1;
  unsigned int ENC_IN = 2;
  unsigned int ENC_OUT = 3;
  unsigned int T_START = 4;
  unsigned int T_STOP = 5;
  unsigned int ENC_POS = 6;
  unsigned int ODRV_VOLT = 7;
  unsigned int ODRV_CUR = 8; 
  unsigned int RPM = 9;
  unsigned int HALL_IN = 10;
  unsigned int HALL_OUT = 11;
  unsigned int WHL_RPM = 12;
  unsigned int WHL_COUNT = 13;
  unsigned int RPM_COUNT = 14;
  unsigned int DT = 15;
  unsigned int ROLLING_FRAME = 16;
  unsigned int EXP_DECAY = 17;
  unsigned int REF_RPM = 18;


  Actuator(HardwareSerial& serial, Constant constant, 
          volatile unsigned long* eg_tooth_count,  volatile unsigned long* gb_tooth_count,
          bool print_to_serial);

  int init(int odrive_timeout);
  int* control_function(int* out);
  int control_function_two(int* out);
  int* homing_sequence(int* out);

  int get_status_code();
  int get_encoder_pos();
  float get_p_value();
  float communication_speed();
  // float get_odrive_current();
  String odrive_errors();

  String diagnostic(bool is_mainpower_on, int dt, bool serial_out);
  int fully_shift(bool direction, int timeout);

private:
  int target_rpm();
  void test_voltage();
  int status;
  Constant constant;
  Encoder encoder;
  ODrive odrive;

  // Functions that get information from Odrive
  int get_encoder_count();

  // Functions that help calculate rpm
  unsigned long m_last_control_execution;
  float calc_engine_rpm(float dt);

  // Members to handle rolling frame gearbox rpm
  float calc_gearbox_rpm(float dt);
  float calc_gearbox_rpm_rolling(float dt);
  std::queue<float> m_gearbox_rpm_frames;
  float m_gearbox_frames_average = 0;

  // Handling exponential decay
  float m_old_rpm = 0;
  float calc_gearbox_rpm_exponential(float dt);

  // For reference scheduling
  float calc_reference_rpm(float gearbox_rpm);

  //Functions that help calculate motor speed
  int calc_motor_rps(int dt);

  // Const
  bool m_print_to_serial;

  // Encoder limit values
  int32_t m_encoder_outbound;  // out of the car
  int32_t m_encoder_inbound;   // towards the engine
  int32_t m_encoder_engage;    // when belt enganged

  // Debugging vars
  long m_control_function_count = 0;
  bool m_has_run;
  unsigned long m_last_serial_execution = 0;
  float m_serial_dt;

  // running gear tooth sensor counts
  volatile unsigned long* m_gb_tooth_count;
  volatile unsigned long* m_eg_tooth_count;
  unsigned long m_last_eg_tooth_count;
  unsigned long m_last_gb_tooth_count;
  // float m_eg_rpm = 0;
  // float m_currentrpm_eg_accum = 0;
  // float m_gb_rpm = 0;
  // float m_currentrpm_gb_accum = 0;
  float m_past_motor_velocity = 0;


  // running control terms
  int m_error = 0;
  float m_gb_rolling;
  float m_gb_exp_decay;
  float m_ref_rpm;
  bool m_outbound_signal;
  bool m_inbound_signal;
  float m_motor_velocity;
};

#endif
