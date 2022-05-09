#include <Actuator.h>
#include <Constant.h>
#include <HardwareSerial.h>
#include <ODrive.h>
#include <queue>
#include <SoftwareSerial.h>
#include <TimerThree.h>
#include "Led.h"

// Print with stream operator
template <class T>
inline Print& operator<<(Print& obj, T arg)
{
  obj.print(arg);
  return obj;
}
template <>
inline Print& operator<<(Print& obj, float arg)
{
  obj.print(arg, 4);
  return obj;
}

Actuator::Actuator(HardwareSerial& serial, Constant constant_in, volatile unsigned long* eg_tooth_count,  volatile unsigned long* gb_tooth_count, bool print_to_serial)
  : encoder(constant_in.encoder_a_pin, constant_in.encoder_b_pin), odrive(serial, constant_in)
{
  Constant constant = constant_in;
  // Save pin values

  m_print_to_serial = print_to_serial;

  // initialize count vairables
  m_gb_tooth_count = gb_tooth_count;
  m_eg_tooth_count = eg_tooth_count;
  m_last_gb_tooth_count = 0;
  m_last_eg_tooth_count = 0;
  m_last_control_execution = 0;

  // limit variables
  m_encoder_outbound = odrive.get_encoder_pos(constant.actuator_motor_number);
  m_encoder_inbound = -666;
  m_encoder_engage = -666;

  // Construct rolling frames queue
  for (int i = 0; i < constant.gearbox_rolling_frames; i++)
  {
    m_gearbox_rpm_frames.push(0.0);
  }
}

int Actuator::init(int odrive_connection_timeout)
{
  status = 0;
  interrupts();

  // Initialize Odrive object
  int o_init = odrive.init(odrive_connection_timeout);
  if (o_init != 0)
  {
    status = o_init;
    return status;
  }

  // Runs encoder index search to find z index
  activate_led(3);
  bool encoder_index_search = odrive.run_state(constant.actuator_motor_number, 6, true, 5);

  // If successful, set ODRV to closed loop control
  if(encoder_index_search) 
  {
    odrive.run_state(constant.actuator_motor_number, 8, true, 1);
  }
  else status = 1003;

  return status;
}

int* Actuator::homing_sequence(int* out)
{
  // Returns an array of ints in format <status, inbound, outbound>
  out[0] = 0;

  odrive.run_state(constant.actuator_motor_number, 8, false, 0);  // Enter velocity control mode
  delay(1000);

  // Home outbound
  int start = millis();

  odrive.set_velocity(constant.actuator_motor_number, 1);

  while (digitalReadFast(constant.estop_outbound_pin) == 0)
  {
    delay(10);
    if (digitalReadFast(constant.estop_outbound_pin) == 1)
    {
      activate_one_led(1);
      break;
    }
    m_encoder_outbound = odrive.get_encoder_pos(constant.actuator_motor_number);
    if (millis() - start > constant.homing_timeout)
    {
      out[0] = 2;
      odrive.run_state(constant.actuator_motor_number, 1, false, 0);
      out[1], out[2] = -1;
      return out;
    }
  }
  odrive.set_velocity(constant.actuator_motor_number, 0);         // Stop spinning after homing

  m_encoder_inbound = m_encoder_outbound - constant.encoder_count_shift_length;

  out[1] = m_encoder_inbound;
  out[2] = m_encoder_outbound;
  return out;
}

int* Actuator::control_function(int* out)
{
  uint32_t timestamp = millis();
  
  uint32_t dt = timestamp - m_last_control_execution;
  if (dt < constant.cycle_period)
  {
    out[STATUS] = 3;
    return out;
  }
  m_last_control_execution = timestamp;

  m_control_function_count++;

  float eg_rpm = calc_engine_rpm(dt);
  float gb_rpm = calc_gearbox_rpm(dt);

  float gb_rolling = calc_gearbox_rpm_rolling(gb_rpm);
  float gb_exp_decay = calc_gearbox_rpm_exponential(gb_rpm);

  float ref_rpm = calc_reference_rpm(m_gb_rolling);

  float error = ref_rpm - eg_rpm;

  // Read estop pin values to report
  bool outbound_signal = digitalReadFast(constant.estop_outbound_pin);
  bool inbound_signal = digitalReadFast(constant.estop_inbound_pin);

  // Calculate control signal
  float new_motor_velocity = constant.proportional_gain * error;

  float instructed_actuator_velocity = odrive.update_velocity(constant.actuator_motor_number, new_motor_velocity);
  
  //odrive.run_state(constant.actuator_motor_number, 8, false, 0);

  // Logging
  // TODO: calculate status
  out[STATUS] = 0;  // Nominal

  if (outbound_signal){// Outbound
   activate_one_led(1);
   out[STATUS] = 1;
   }  
  else clear_all_leds();

  if (inbound_signal){
   activate_one_led(4);
   out[STATUS] = 2;  // Inbound
  }
  else clear_all_leds();

  float voltage = -1;
  float current = -1e-6;
  int encoder_pos = -1;
  float encoder_vel = -1;
  // Query ODrive for data and report
  odrive.get_voltage_current_encoder(constant.actuator_motor_number, &voltage, &current, &encoder_pos, &encoder_vel);
  out[ODRV_VOLT] = voltage;
  out[ODRV_CUR] = current * 1.0e6;
  out[ENC_VEL] = encoder_vel;
  out[ENC_POS] = encoder_pos;

  // Report control function data
  out[RPM] = eg_rpm;
  out[RPM_COUNT] = *m_eg_tooth_count;
  out[DT] = dt;
  out[ACT_VEL] = instructed_actuator_velocity;
  out[ESTOP_IN] = inbound_signal;
  out[ESTOP_OUT] = outbound_signal;
  out[T_START] = timestamp;
  out[ROLLING_FRAME] = gb_rolling;
  out[EXP_DECAY] = gb_exp_decay;
  out[REF_RPM] = ref_rpm;
  out[T_STOP] = millis();

  return out;
}

//----------------Geartooth Functions----------------//

float Actuator::calc_gearbox_rpm(float dt)
// Secondary rpm
{
  noInterrupts();
  float rps = float(*m_gb_tooth_count - m_last_gb_tooth_count) / dt;
  rps *= constant.gearbox_to_secondary_ratio; //    M20: (48/(17))/8  M21: (9)/12  
  float rpm = rps * 1000.0 * 60.0;
  m_last_gb_tooth_count = *m_gb_tooth_count;
  interrupts();
  return rpm;
}

float Actuator::calc_gearbox_rpm_rolling(float new_rpm)
// Calculate the avg gearbox rpm
// Will automatically calculate the new rpm, then calculate the avg with a queue
{
  m_gearbox_frames_average += (new_rpm - m_gearbox_rpm_frames.front()) / constant.gearbox_rolling_frames;
  m_gearbox_rpm_frames.pop();
  m_gearbox_rpm_frames.push(new_rpm);
  return m_gearbox_frames_average;
}

float Actuator::calc_gearbox_rpm_exponential(float new_rpm)
{
  float alpha = 0.05;
  float output = new_rpm * alpha + m_old_rpm * (1 - alpha);
  m_old_rpm = output;
  return output;
}

float Actuator::calc_engine_rpm(float dt)
{
  noInterrupts();
  float freq_in_minutes = 1000 * 60 / dt;
  float rpm = (float(*m_eg_tooth_count - m_last_eg_tooth_count) / constant.eg_teeth_per_rotation) * freq_in_minutes;
  m_last_eg_tooth_count = *m_eg_tooth_count;
  interrupts();
  return rpm;
}

float Actuator::calc_reference_rpm(float gearbox_rpm)
// Implemented according to a reference drawing John drew up
{
  float output;
  // Region 1: Before belt slip, so hold at engage rpm
  if (gearbox_rpm < constant.gearbox_engage_rpm)
  {
    output = constant.engine_engage;
  }
  // Region 2: Acceleration zone
  else if (gearbox_rpm < constant.gearbox_power_rpm)
  {
    output = gearbox_rpm * constant.ecvt_max_ratio;
  }
  // Region 3: Shifting zone
  else if (gearbox_rpm < constant.gearbox_overdrive_rpm)
  {
    output = constant.engine_power;
  }
  // Region 4: Overdrive zone
  else
  {
    output = gearbox_rpm * constant.overdrive_ratio;
  }

  return output;
}

//-----------------Diagnostic Functions--------------//

String Actuator::diagnostic(bool main_power, int dt, bool print_serial = true)
{
  // General diagnostic tool to record sensor readings as well as some odrive info
  m_serial_dt = millis() - m_last_serial_execution;
  m_last_serial_execution = millis();
  String output = "";
  output += "-----------------------------\n";
  output += "Time: " + String(millis()) + "\n";
  if (main_power)
  {
    output += "Odrive voltage: " + String(odrive.get_voltage()) + "\n";
    output += "Odrive current: " + String(odrive.get_cur()) + "\n";
    output += "Odrive speed: " + String(odrive.get_vel(constant.actuator_motor_number)) + "\n";
    output += "Encoder count: " + String(odrive.get_encoder_pos(constant.actuator_motor_number)) + "\n";
  }
  output += "Outbound limit: " + String(m_encoder_outbound) + "\n";
  output += "Inbound limit: " + String(m_encoder_inbound) + "\n";
  output += "Outbound estop reading: " + String(digitalReadFast(constant.estop_outbound_pin)) + "\n";
  output += "Inbound estop reading: " + String(digitalReadFast(constant.estop_inbound_pin)) + "\n";
  output += "dt term: " + String(m_serial_dt) + "\n";
  output += "Engine Gear Tooth Count: " + String(*m_eg_tooth_count) + "\n";
  output += "Engine RPM: " + String(calc_engine_rpm(m_serial_dt)) + "\n";
  output += "Gearbox gear tooth count: " + String(*m_gb_tooth_count) + "\n";
  float gearbox_rpm = calc_gearbox_rpm(m_serial_dt);
  output += "Gearbox RPM: " + String(gearbox_rpm) + "\n";
  output += "Gearbox RPM Rolling: " + String(calc_gearbox_rpm_rolling(gearbox_rpm)) + "\n";
  output += "Gearbox RPM Exponential: " + String(calc_gearbox_rpm_exponential(gearbox_rpm)) + "\n";
  output += "Estop Signal: " + String(digitalRead(36)) + "\n";
  // output = String(gearbox_rpm) + ", " + String(calc_gearbox_rpm_rolling(gearbox_rpm)) + ", " + String(calc_gearbox_rpm_exponential(gearbox_rpm)) + "\n";
  // output = String(millis()/10.0 - 100) + ", " + String(calc_reference_rpm(millis()/10.0-100)) + "\n";
  if (print_serial)
  {
    Serial.print(output);
  }
  return output;
}

float Actuator::communication_speed()
{
  // Tests communication speed with the odrive and returns the result as a float
  const int data_points = 1000;
  int com_start = 0;
  int com_end = 0;
  int com_total = 0;
  int com_bench = 0;
  // float test = 0;
  odrive.run_state(constant.actuator_motor_number, 8, false, 0);
  odrive.set_velocity(constant.actuator_motor_number, .5);
  delay(1000);

  // Benchmark
  for (int i = 0; i < data_points; i++)
  {
    com_start = millis();
    com_end = millis();
    com_bench += com_end - com_start;
  }

  Serial.println(com_bench);

  // With command to odrive
  for (int i = 0; i < data_points; i++)
  {
    com_start = millis();

    // command to odrive
    // test = odrive.get_vel(motor_number);

    com_end = millis();
    com_total += com_end - com_start;
  }
  Serial.println(com_total);
  odrive.set_velocity(constant.actuator_motor_number, 0);  // Stop spinning after homing
  odrive.run_state(constant.actuator_motor_number, 1, false, 0);

  return float(com_total - com_bench) / float(data_points);
}

String Actuator::odrive_errors()
{
  return odrive.dump_errors();
}

// int Actuator::fully_shift(bool direction, int timeout)
// {
//   // Shifts the motor all the way in or out
//   // direction = true is in, false is out
//   int speed;
//   int software_limit_pin;
//   if (direction) {
//     speed = -2;
//     software_limit_pin = constant.hall_inbound_pin;
//   }
//   else {
//     speed = 2;
//     software_limit_pin = constant.hall_outbound_pin;
//   }
//   int status = 0;
//   // int start_time = millis();

//   odrive.run_state(constant.actuator_motor_number, 8, false, 0);  // Enter velocity control mode
//   delay(1000);

//   // Home outbound
//   int start = millis();

//   odrive.set_velocity(constant.actuator_motor_number, speed);

//   while (digitalReadFast(software_limit_pin) == 1)
//   {
//     delay(10);
//     if (digitalReadFast(constant.hall_outbound_pin) == 0)
//     {
//       break;
//     }
//     if (millis() - start > constant.homing_timeout)
//     {
//       status = 1;
//       odrive.run_state(constant.actuator_motor_number, 1, false, 0);
//       return status;
//     }
//   }
//   odrive.set_velocity(constant.actuator_motor_number, 0);         // Stop spinning after homing
//   odrive.run_state(constant.actuator_motor_number, 1, false, 0);  // Idle state

//   return status;
// }

void Actuator::move_back_and_forth_slowly(){
  int period = 3000;
  if((millis()/period)%2){
    odrive.set_velocity(constant.actuator_motor_number, 1);   
  } else {
    odrive.set_velocity(constant.actuator_motor_number, -1); 
  }
  odrive.run_state(constant.actuator_motor_number, 8, false, 0);
}