#include <Actuator.h>
#include <Constant.h>
#include <HardwareSerial.h>
#include <ODrive.h>
#include <queue>
#include <SoftwareSerial.h>
#include <TimerThree.h>

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
  : encoder(constant_in.encoder_a_pin, constant_in.encoder_b_pin), odrive(serial)
{
  Constant constant = constant_in;
  // Save pin values

  m_print_to_serial = print_to_serial;

  // initialize count vairables
  m_gb_tooth_count = gb_tooth_count;
  m_eg_tooth_count = eg_tooth_count;
  m_gb_rpm = 0;
  m_eg_rpm = 0;
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

int Actuator::init(int odrive_timeout)
{
  status = 0;
  interrupts();

  // Initialize Odrive object
  int o_init = odrive.init(odrive_timeout);
  if (o_init != 0)
  {
    status = o_init;
    return status;
  }

  // Runs encoder index search to find z index
  bool encoder_index_search = odrive.run_state(constant.actuator_motor_number, 6, true, 5);

  // If successful, set ODRV to idle
  if(!encoder_index_search) 
  {
    odrive.run_state(constant.actuator_motor_number, 1, true, 1);
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

  odrive.set_velocity(constant.actuator_motor_number, 2);

  while (digitalReadFast(constant.hall_outbound_pin) == 1)
  {
    delay(10);
    if (digitalReadFast(constant.hall_outbound_pin) == 0)
    {
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
  // odrive.run_state(constant.actuator_motor_number, 1, false, 0);  // Idle state
  // digitalWrite(LED_BUILTIN, LOW);

  m_encoder_inbound = m_encoder_outbound - constant.encoder_count_shift_length;


  out[1] = m_encoder_inbound;
  out[2] = m_encoder_outbound;
  return out;
}

int* Actuator::control_function(int* out)
{
  // Returns an array of ints in format
  //<status, rpm, actuator_velocity, fully shifted in, fully shifted out, time_started, time_finished, enc_pos,
  // odrive_volt, odrive_current>
  out[5] = millis();
  m_control_function_count++;
  if (millis() - m_last_control_execution > constant.cycle_period)
  {
    out[0] = 0;
    out[3] = 0;
    out[4] = 0;

    // Calculate Engine Speed + Filter Engine Speed
    float dt = millis() - m_last_control_execution;
    m_eg_rpm = calc_engine_rpm(dt);
    m_gb_rpm = calc_gearbox_rpm(dt);
    out[ROLLING_FRAME] = calc_gearbox_rpm_rolling(m_gb_rpm);
    out[EXP_DECAY] = calc_gearbox_rpm_exponential(m_gb_rpm);
    m_eg_rpm = (constant.exponential_filter_alpha * m_eg_rpm) + (1 - constant.exponential_filter_alpha) * m_currentrpm_eg_accum;
    m_currentrpm_eg_accum = m_eg_rpm;

    m_gb_rpm = (constant.exponential_filter_alpha * m_gb_rpm) + (1 - constant.exponential_filter_alpha) * m_currentrpm_gb_accum;
    m_currentrpm_gb_accum = m_gb_rpm;
    // this is an exponential moving average
    //"averages" the last 1/EXP_FILTER_CONST data points
    // chosen because very simple to implement - use better filters in the future?

    out[1] = m_eg_rpm;
    out[11] = m_gb_rpm;
    out[12] = *m_gb_tooth_count;
    // currentrpm_eg = analogRead(A2)*4;
    m_last_control_execution = millis();

    // If motor is spinning too slow, then shift all the way out
    if (m_eg_rpm < constant.minimum_rpm)
    {
      if ((digitalReadFast(constant.hall_outbound_pin) == 0 || odrive.get_encoder_pos(constant.actuator_motor_number) >= m_encoder_outbound))
      {
        // If below min rpm and shifted out all the way
        odrive.set_velocity(constant.actuator_motor_number, 0);  // Shift out

        out[0] = 4;                                     // Report status
        out[2] = 0;                                     // Report velocity
        out[4] = 1;
        out[8] = odrive.get_voltage();
        out[9] = 1e6 * odrive.get_cur();
        out[7] = odrive.get_encoder_pos(constant.actuator_motor_number);
        out[6] = millis();
        return out;
      }
      else
      {
        // If below min rpm and not shifted out all the way
        odrive.set_velocity(constant.actuator_motor_number, 3);  // Shift out
        out[0] = 5;
        out[2] = 3;
        out[8] = odrive.get_voltage();
        out[9] = 1e6 * odrive.get_cur();
        out[7] = odrive.get_encoder_pos(constant.actuator_motor_number);
        out[6] = millis();
        return out;
      }
    }

    // Compute error

    // If error is within a certain deviation from the desired value, do not shift
    out[REF_RPM] = calc_reference_rpm(out[ROLLING_FRAME]);
    error = out[REF_RPM] - m_eg_rpm;
    int error_deriv = (error - prev_error) / dt;
    int motor_velocity = constant.proportional_gain * error + constant.derivative_gain * error_deriv;
    prev_error = error;
    // if (abs(error) <= rpm_allowance) {
    //     error = 0;
    // }

    // TODO
    // MIN/MAX instead of if else
    // Check halls 0/1 when triggered ITS 0
    // Change output from int array to array of pointers
    // Attach to interrupt

    // INWARDS IN NEGATIVE
    // HALL TRIGGERED IS 0

    // If error will over or under actuate actuator then set error to 0.
    if (digitalReadFast(constant.hall_outbound_pin) == 0 || odrive.get_encoder_pos(constant.actuator_motor_number) >= m_encoder_outbound)
    {
      // Shifted out completely
      if (motor_velocity > 0)
        motor_velocity = 0;
      out[0] = 6;
      out[4] = 1;
    }
    else if (digitalReadFast(constant.hall_inbound_pin) == 0 || odrive.get_encoder_pos(constant.actuator_motor_number) <= m_encoder_inbound)
    {
      // Shifted in completely
      if (motor_velocity < 0)
        motor_velocity = 0;
      out[0] = 7;
      out[3] = 1;
    }

    // Multiply by gain and set new motor velocity.

    out[2] = motor_velocity;

    odrive.set_velocity(constant.actuator_motor_number, motor_velocity);
    odrive.run_state(constant.actuator_motor_number, 8, false, 0);
    out[8] = odrive.get_voltage();
    out[9] = 1e6 * odrive.get_cur();
    out[7] = odrive.get_encoder_pos(constant.actuator_motor_number);
    out[6] = millis();
    return out;
  }

  // Return status if attempt to run the control function too soon
  out[0] = 3;
  out[6] = millis();
  return out;
}


int Actuator::control_function_two(int* out){
    //returns status of actuator

    //  Things to thinkabout:
    //  what happens to derivative term in calc engine rpm if it hasn't been called for awhile
    //  what happens when encoder inbound is re read --> how should we tell ourselves what the actual
    //  between in and outbound is?

    uint32_t timestamp = millis();
    out[T_START] = timestamp;
    out[STATUS] = 0;
    uint32_t dt = timestamp - m_last_control_execution;
    if (dt < constant.cycle_period) return 3; //return 3 if control function was called too soon
    m_last_control_execution = timestamp;

    // Calculate Engine Speed + Filter Engine Speed
    out[DT] = dt;
    out[RPM_COUNT] = *m_eg_tooth_count;
    m_eg_rpm = calc_engine_rpm(dt);
    out[RPM] = m_eg_rpm;

    // update encoder inbound if we weren't quite right
    if(digitalReadFast(constant.hall_inbound_pin) == 0) m_encoder_inbound = encoder.read();

    //  My state? diagram that describes what the actuator should be doing given
    //  its shift fork position and engine rpm
    //
    //  Engine Speed:       1      Engage RPM       2      Target rpm     3
    //  Position:  shfit to 1 rotation  |  Shfit to engage      |   Vel PID
    //    (+)       before engage       |                       |                           
    //  Outbound------------------------------------------------------------------------          
    //             shift to 1 rotation  |  Shift to engage      |   Vel PID
    //             before engage        |                       |
    //  Engage--------------------------------------------------------------------------
    //             Vel PID              |  Vel PID              |  Vel PID
    //  In + half-----------------------------------------------------------------------
    //  rotation   Vel PID              |  Vel PID              |  Vel PID min(-1 rot/s)  
    //                                  |                       |
    //  Inbound------------------------------------------------------------------------- 
    //   (-)       Vel PID              |  Vel PID              | VEL PID min (0 rot/s)
    
    

    //errors and motor_velocity
    uint32_t position_error;    //cpr
    int motor_velocity;         //rps


    //Collumn 1: engine rpm less than engage rpm
    if (m_eg_rpm < constant.engine_engage ){
        
        if(encoder.read() > m_encoder_engage){
            //if engine speed is less than engage rpm & not engaged position p control to just before engage
            position_error = ( m_encoder_engage + constant.encoder_engage_buffer - encoder.read()); //enc greater than buffer error is negative 
            motor_velocity = position_error * constant.linear_distance_per_rotation * constant.position_p_gain; //TODO convert to linear distance
            //TODO Update STATUS
            out[STATUS] = 21;
        } else  {
            //if engine speed is less than engage rpm & fork is between engage and inbound vel pid control out
            motor_velocity = calc_motor_rps(dt);
            out[STATUS] = 22;
        }
    }
    //Collumn 2: engine rpm between desired and engage rpm
    else if(m_eg_rpm < constant.desired_rpm){
        if(encoder.read() > m_encoder_engage){
            //if engine speed is less than desired rpm & position is less than Engage position p control to just before engage
            position_error = (m_encoder_engage - encoder.read()); //enc greater than engage error is negative 
            motor_velocity = position_error * constant.linear_distance_per_rotation * constant.position_p_gain; //TODO convert to linear distance
            out[STATUS] = 23;
        }
        else if (encoder.read() > m_encoder_engage - constant.encoder_engage_buffer){
            motor_velocity = 0; //if engine wants to shift out just before engage stop it from shifing to avoid oscillations
            out[STATUS] = 24;
        }
        else{
            //if engine rpm is less than engage rpm and fork is between engage and inbound vel pid control out
            motor_velocity = calc_motor_rps(dt);
            out[STATUS] = 25;
        }
      }
    // Collumn 3: engine rpm greater than desired rpm
    else{
        motor_velocity = calc_motor_rps(dt);
        if(encoder.read() < m_encoder_inbound && motor_velocity < 0){
            motor_velocity = 0;
            //if engine rpm is above desired rpm & shift fork not near inbound vel pid control in
            out[STATUS] = 26;
        }
        else if(encoder.read() > m_encoder_inbound){
            //if engine rpm is above desired rpm & shift fork close to inbound constrain motor_velocity
            if(motor_velocity < -1) motor_velocity = -1;
            out[STATUS] = 27;
        }
        else{
            //if engine rpm is above desired rpm & shift fork at outbound constrain motor_velocity to 0
            motor_velocity = 0;
            out[STATUS] = 28;
        }
    }

    
    // Check to see if shifted all the way in or out
    out[HALL_IN] = digitalReadFast(constant.hall_inbound_pin) == 0;
    out[HALL_OUT] = digitalReadFast(constant.hall_outbound_pin) == 0;
    out[ENC_IN] = encoder.read() <= m_encoder_inbound;
    out[ENC_OUT] = encoder.read() >= m_encoder_outbound;

    // Shifted in all the way
    if (out[HALL_IN] || out[ENC_IN]) {
      if (motor_velocity < 0) motor_velocity = 0;
    }

    // Shifted out all the way
    if (out[HALL_OUT] || out[ENC_OUT]) {
      if (motor_velocity > 0) motor_velocity = 0;
    }

    if (motor_velocity == 0)
    {
      odrive.run_state(constant.actuator_motor_number, 1, false, 0);
    }
    else
    {
      odrive.set_velocity(constant.actuator_motor_number, motor_velocity);
      odrive.run_state(constant.actuator_motor_number, 8, false, 0); //maybe we should always be in state 8 as to be faster
    }
    
    out[ACT_VEL] = motor_velocity;
    out[ENC_POS] = encoder.read();
    out[ODRV_VOLT] = odrive.get_voltage();
    out[ODRV_CUR] =  1e6 * odrive.get_cur();
    out[T_STOP] = millis();
    return 0; // Also need to think about this with getty
}

int Actuator::calc_motor_rps(int dt){
    error = constant.desired_rpm - m_eg_rpm;
    int error_deriv = (error - prev_error) / dt;  // 16ms in between runs rn
    int motor_velocity = constant.proportional_gain * error + constant.derivative_gain * error_deriv;
    prev_error = error; //TODO error and prev_error need m_ added to them
    return motor_velocity;
}


//----------------Geartooth Functions----------------//

float Actuator::calc_gearbox_rpm(float dt)
// Secondary rpm
{
  noInterrupts();
  float rps = float(*m_gb_tooth_count - m_last_gb_tooth_count) / dt;
  rps *= 6.0/17.0;
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
    // output += "Odrive voltage: " + String(odrive.get_voltage()) + "\n";
    // output += "Odrive speed: " + String(odrive.get_vel(constant.actuator_motor_number)) + "\n";
    // output += "Encoder count: " + String(encoder.read()) + "\n";
  }
  // output += "Outbound limit: " + String(m_encoder_outbound) + "\n";
  // output += "Inbound limit: " + String(m_encoder_inbound) + "\n";
  // output += "Outbound reading: " + String(digitalReadFast(constant.hall_outbound_pin)) + "\n";
  // output += "Inbound reading: " + String(digitalReadFast(constant.hall_inbound_pin)) + "\n";
  // output += "dt term: " + String(m_serial_dt) + "\n";
  // output += "Engine Gear Tooth Count: " + String(*m_eg_tooth_count) + "\n";
  // output += "Engine RPM: " + String(calc_engine_rpm(m_serial_dt)) + "\n";
  output += "Gearbox gear tooth count: " + String(*m_gb_tooth_count) + "\n";
  float gearbox_rpm = calc_gearbox_rpm(m_serial_dt);
  output += "Gearbox RPM: " + String(gearbox_rpm) + "\n";
  output += "Gearbox RPM Rolling: " + String(calc_gearbox_rpm_rolling(gearbox_rpm)) + "\n";
  output += "Gearbox RPM Exponential: " + String(calc_gearbox_rpm_exponential(gearbox_rpm)) + "\n";
  // output += "Estop Signal: " + String(digitalRead(36)) + "\n";
  output = String(gearbox_rpm) + ", " + String(calc_gearbox_rpm_rolling(gearbox_rpm)) + ", " + String(calc_gearbox_rpm_exponential(gearbox_rpm)) + "\n";
  output = String(millis()/10.0 - 100) + ", " + String(calc_reference_rpm(millis()/10.0-100)) + "\n";
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

void Actuator::test_voltage()
{
  // Reads bus voltage immediately after the odrive moves
  delay(1000);
  Serial.println("Reading Voltage");
  odrive.run_state(constant.actuator_motor_number, 8, false, 0);  // Tells Odrive to rotate motor
  odrive.set_velocity(constant.actuator_motor_number, -1);
  for (int i = 0; i < 250; i++)
  {
    Serial.println(odrive.get_voltage());  // Show bus voltage on serial moniter
    delay(10);
  }
  odrive.run_state(constant.actuator_motor_number, 1, false, 0);  // Tell Odrive to stop rotating
  odrive.set_velocity(constant.actuator_motor_number, 0);
}

float Actuator::get_p_value()
{
  return constant.proportional_gain;
}

String Actuator::odrive_errors()
{
  return odrive.dump_errors();
}

int Actuator::fully_shift(bool direction, int timeout)
{
  // Shifts the motor all the way in or out
  // direction = true is in, false is out
  int speed;
  int software_limit_pin;
  if (direction) {
    speed = -2;
    software_limit_pin = constant.hall_inbound_pin;
  }
  else {
    speed = 2;
    software_limit_pin = constant.hall_outbound_pin;
  }
  int status = 0;
  // int start_time = millis();

  odrive.run_state(constant.actuator_motor_number, 8, false, 0);  // Enter velocity control mode
  delay(1000);

  // Home outbound
  int start = millis();

  odrive.set_velocity(constant.actuator_motor_number, speed);

  while (digitalReadFast(software_limit_pin) == 1)
  {
    delay(10);
    if (digitalReadFast(constant.hall_outbound_pin) == 0)
    {
      break;
    }
    if (millis() - start > constant.homing_timeout)
    {
      status = 1;
      odrive.run_state(constant.actuator_motor_number, 1, false, 0);
      return status;
    }
  }
  odrive.set_velocity(constant.actuator_motor_number, 0);         // Stop spinning after homing
  odrive.run_state(constant.actuator_motor_number, 1, false, 0);  // Idle state

  return status;
}