#include <Actuator.h>
#include <Constant.h>
#include <Encoder.h>
#include <HardwareSerial.h>
#include <ODrive.h>
#include <SoftwareSerial.h>

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

Actuator::Actuator(HardwareSerial& serial, Constant* constant_i, bool print_to_serial)
  : encoder(enc_a_pin, enc_b_pin), odrive(serial)
{
  // Save pin values
  m_eg_tooth_pin = eg_tooth_pin;
  m_hall_inbound_pin = hall_inbound_pin;
  m_hall_outbound_pin = hall_outbound_pin;

  m_print_to_serial = print_to_serial;

  // initialize count vairables
  m_eg_tooth_count = 0;
  m_last_eg_tooth_count = 0;
  m_last_control_execution = 0;
  m_eg_rpm = 0;

  // Test variable
  m_has_run = false;

  // limit variables
  m_encoder_outbound = encoder.read();
  m_encoder_inbound = -666;

  // Save pointer to constant object
  constant = constant_i;
}

int Actuator::init(int odrive_timeout, void (*external_count_eg_tooth)())
{
  // Attaches geartooth sensor to interrupt
  interrupts();  // allows interupts
  attachInterrupt(m_eg_tooth_pin, external_count_eg_tooth, FALLING);

  // Initialize Odrive object
  int o_init = odrive.init(odrive_timeout);
  if (o_init != 0)
  {
    status = o_init;
    return status;
  }

  odrive.run_state(k_motor_number, 1, true, 0);  // Sets ODrive to IDLE

  status = 0;
  return status;
}

int* Actuator::homing_sequence(int* out)
{
  // Returns an array of ints in format <status, inbound, outbound>
  out[0] = 0;

  odrive.run_state(k_motor_number, 8, false, 0);  // Enter velocity control mode
  // TODO: Enums for IDLE, VELOCITY_CONTROL
  delay(1000);

  // Home outbound
  digitalWrite(LED_BUILTIN, HIGH);
  int start = millis();

  odrive.set_velocity(k_motor_number, 2);

  while (digitalReadFast(m_hall_outbound_pin) == 1)
  {
    delay(10);
    if (digitalReadFast(m_hall_outbound_pin) == 0)
    {
      break;
    }
    m_encoder_outbound = encoder.read();
    if (millis() - start > k_homing_timeout)
    {
      out[0] = 2;
      odrive.run_state(k_motor_number, 0, false, 0);
      out[1] = -1;
      out[2] = -1;
      return out;
    }
  }
  odrive.set_velocity(k_motor_number, 0);         // Stop spinning after homing
  odrive.run_state(k_motor_number, 1, false, 0);  // Idle state
  digitalWrite(LED_BUILTIN, LOW);

  m_encoder_inbound = m_encoder_outbound - k_encoder_count_shift_length;

  // Home inbound - [IN PURGATORY]
  //  set_velocity(-10);
  //  while (digitalReadFast(m_hall_inbound) == 1) {
  //      m_encoder_inbound = encoder.read();
  //  }

  // //Testing encoder reading by shifting all the way back to the inbound
  // delay(500);
  // odrive.run_state(motor_number, 8, false, 0);
  // odrive.set_velocity(motor_number, -2);
  // digitalWrite(LED_BUILTIN, HIGH);
  // while(encoder.read() > m_encoder_inbound){
  //     Serial.print("encoder inbound: ");
  //     Serial.println(m_encoder_inbound);
  //     Serial.print("current encoder position");
  //     Serial.println(encoder.read());
  // }
  // digitalWrite(LED_BUILTIN, LOW);
  // odrive.set_velocity(motor_number, 0); //Stop spinning after homing
  // odrive.run_state(motor_number, 1, false, 0); //Idle state

  out[1] = m_encoder_inbound;
  out[2] = m_encoder_outbound;
  return out;
}

void Actuator::control_function(int* status, int* rpm, int* actuator_velocity, int* inbound_triggered,
                                int* outbound_triggered, int* time_start, int* time_end, int* encoder_position,
                                int* odrive_voltage, int* odrive_current, int* error_out)
{
  // Returns an array of ints in format
  //<status, rpm, actuator_velocity, fully shifted in, fully shifted out, time_started, time_finished, enc_pos,
  // odrive_volt, odrive_current>
  *time_start = millis();
  m_control_function_count++;
  if (millis() - m_last_control_execution > constant->k_actuator_cycle_period)
  {
    *status = 0;
    *inbound_triggered = 777;
    *outbound_triggered = 0;

    // Calculate Engine Speed + Filter Engine Speed
    float dt = millis() - m_last_control_execution;
    m_eg_rpm = calc_engine_rpm(dt);

    m_eg_rpm = ((constant->k_exp_filt_alpha) * m_eg_rpm) + (1 - (constant->k_exp_filt_alpha)) * m_currentrpm_eg_accum;
    m_currentrpm_eg_accum = m_eg_rpm;
    // this is an exponential moving average
    //"averages" the last 1/EXP_FILTER_CONST data points
    // chosen because very simple to implement - use better filters in the future?

    *rpm = m_eg_rpm;
    // currentrpm_eg = analogRead(A2)*4;
    m_last_control_execution = millis();

    // If motor is spinning too slow, then shift all the way out
    if (m_eg_rpm < k_min_rpm)
    {
      if ((digitalReadFast(m_hall_outbound_pin) == 0 || encoder.read() >= m_encoder_outbound))
      {
        // If below min rpm and shifted out all the way
        odrive.run_state(k_motor_number, 1, false, 0);  // SET IDLE
        *status = 4;                                    // Report status
        *actuator_velocity = 0;                         // Report velocity
        *outbound_triggered = 1;
        *odrive_voltage = odrive.get_voltage();
        *odrive_current = odrive.get_cur();
        *encoder_position = encoder.read();
        *time_end = millis();
        return;
      }
      else
      {
        // If below min rpm and not shifted out all the way
        odrive.set_velocity(k_motor_number, 3);  // Shift out
        *status = 5;
        *actuator_velocity = 3;
        *odrive_voltage = odrive.get_voltage();
        *odrive_current = odrive.get_cur();
        *encoder_position = encoder.read();
        *time_end = millis();
        return;
      }
    }

    // Compute error

    // If error is within a certain deviation from the desired value, do not shift
    error = constant->k_desired_rpm - m_eg_rpm;
    int error_deriv = (error - prev_error) / dt;  // 16ms in between runs rn
    int motor_velocity = (constant->k_proportional_gain) * error + (constant->k_derivative_gain) * error_deriv;
    prev_error = error;
    // if (abs(error) <= rpm_allowance) {
    //     error = 0;
    // }

    // If error will over or under actuate actuator then set error to 0.
    if (digitalReadFast(m_hall_outbound_pin) == 0 || encoder.read() >= m_encoder_outbound)
    {
      // Shifted out completely
      if (motor_velocity > 0)
        motor_velocity = 0;
      *status = 6;
      *outbound_triggered = 1;
    }
    else if (digitalReadFast(m_hall_inbound_pin) == 0 || encoder.read() <= m_encoder_inbound)
    {
      // Shifted in completely
      if (motor_velocity < 0)
        motor_velocity = 0;
      *status = 7;
      *inbound_triggered = 777;
    }

    // Multiply by gain and set new motor velocity.

    *actuator_velocity = motor_velocity;

    if (motor_velocity == 0)
    {
      odrive.run_state(k_motor_number, 1, false, 0);
    }
    else
    {
      odrive.set_velocity(k_motor_number, motor_velocity);
      odrive.run_state(k_motor_number, 8, false, 0);
    }
    *odrive_voltage = odrive.get_voltage();
    *odrive_current = odrive.get_cur();
    *encoder_position = encoder.read();
    *time_end = millis();
    *error_out = error;
    return;
  }

  // Return status if attempt to run the control function too soon
  *status = 3;
  *time_end = millis();
  return;
}

//----------------Geartooth Functions----------------//
void Actuator::count_eg_tooth()
{
  m_eg_tooth_count++;
}

float Actuator::calc_engine_rpm(float dt)
{
  noInterrupts();
  float freq_in_minutes = 1000 * 60 / dt;
  float rpm = (float(m_eg_tooth_count - m_last_eg_tooth_count) / k_eg_teeth_per_rotation) * freq_in_minutes;
  m_last_eg_tooth_count = m_eg_tooth_count;
  interrupts();
  return rpm;
}

int Actuator::target_rpm()
{
  // if (currentrpm_eg<EG_ENGAGE){
  //     return EG_ENGAGE;
  // }
  // if (currentrpm_eg>EG_POWER){
  //     return EG_POWER;
  // }
  // int target = RPM_TARGET_MULTIPLIER*(currentrpm_eg)+ EG_ENGAGE;
  // if(target > EG_POWER) return EG_POWER;
  // return target;
  return 0;
}
//-----------------Diagnostic Functions--------------//

String Actuator::diagnostic(bool main_power, bool print_serial = true)
{
  // General diagnostic tool to record sensor readings as well as some odrive info

  String output = "";
  output += "-----------------------------\n";
  output += "Time: " + String(millis()) + "\n";
  if (main_power)
  {
    output += "Odrive voltage: " + String(odrive.get_voltage()) + "\n";
    output += "Odrive speed: " + String(odrive.get_vel(k_motor_number)) + "\n";
    output += "Encoder count: " + String(encoder.read()) + "\n";
  }
  output += "Outbound limit: " + String(m_encoder_outbound) + "\n";
  output += "Inbound limit: " + String(m_encoder_inbound) + "\n";
  output += "Outbound reading: " + String(digitalReadFast(m_hall_outbound_pin)) + "\n";
  output += "Inbound reading: " + String(digitalReadFast(m_hall_inbound_pin)) + "\n";
  output += "Engine Gear Tooth Count: " + String(m_eg_tooth_count) + "\n";
  output += "Current rpm: " + String(m_eg_rpm) + "\n";

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
  odrive.run_state(k_motor_number, 8, false, 0);
  odrive.set_velocity(k_motor_number, .5);
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
  odrive.set_velocity(k_motor_number, 0);  // Stop spinning after homing
  odrive.run_state(k_motor_number, 1, false, 0);

  return float(com_total - com_bench) / float(data_points);
}

void Actuator::test_voltage()
{
  // Reads bus voltage immediately after the odrive moves
  delay(1000);
  Serial.println("Reading Voltage");
  odrive.run_state(k_motor_number, 8, false, 0);  // Tells Odrive to rotate motor
  odrive.set_velocity(k_motor_number, -1);
  for (int i = 0; i < 250; i++)
  {
    Serial.println(odrive.get_voltage());  // Show bus voltage on serial moniter
    delay(10);
  }
  odrive.run_state(k_motor_number, 1, false, 0);  // Tell Odrive to stop rotating
  odrive.set_velocity(k_motor_number, 0);
}

float Actuator::get_p_value()
{
  return constant->k_proportional_gain;
}

String Actuator::odrive_errors()
{
  return odrive.dump_errors();
}