#include <Actuator.h>
#include <ODrive.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <Encoder.h>
#include <TimerThree.h>

// Print with stream operator
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

Actuator::Actuator(
    HardwareSerial& serial,
    const int enc_A, 
    const int enc_B, 
    const int egTooth, 
    const int gbTooth, 
    const int hall_inbound, 
    const int hall_outbound,
    void (*external_count_egTooth)(),
    bool printToSerial)
    :encoder(enc_A, enc_B), odrive(serial){

    //Save pin values
    m_egTooth = egTooth;
    m_gbTooth = gbTooth;
    m_hall_inbound = hall_inbound;
    m_hall_outbound = hall_outbound;

    m_printToSerial = printToSerial;

    //initialize count vairables
    egTooth_Count = 0;
    egTooth_Count_last = 0;
    last_control_execution = 0;
    currentrpm_eg = 0;

    //Functions to support interrupt
    m_external_count_egTooth = external_count_egTooth;

    //Test variable
    hasRun = false;

    //limit variables
    m_encoder_outbound = -666;
    m_encoder_inbound = -666;
}

int Actuator::init(int odrive_timeout){
    //Initialize Odrive object
    int o_init = odrive.init(odrive_timeout);
    if(o_init != 0){
        status = o_init;
        return status;
    }
    
    odrive.run_state(motor_number, 1, true, 0); //Sets ODrive to IDLE 
    
    
    interrupts(); //allows interupts
    attachInterrupt(m_egTooth, m_external_count_egTooth, FALLING);

    status = 0;
    return status;
}


int* Actuator::homing_sequence(int* out){
    //Returns an array of ints in format <status, inbound, outbound>
    out[0] = 0;

    odrive.run_state(motor_number, 8, false, 0); //Enter velocity control mode
    //TODO: Enums for IDLE, VELOCITY_CONTROL
    delay(1000);

    //Home outbound
    int start = millis();
    odrive.set_velocity(motor_number, 3);
    while (digitalReadFast(m_hall_outbound) == 1) {
        m_encoder_outbound = encoder.read();
        if (millis() - start > homing_timeout) {
            out[0] = 2;
            odrive.run_state(motor_number, 0, false, 0);
            out[1], out[2] = -1;
            return out;
        }
    }
    odrive.set_velocity(motor_number, 0); //Stop spinning after homing
    odrive.run_state(motor_number, 1, false, 0); //Idle state

    m_encoder_inbound = m_encoder_outbound - encoderCountShiftLength;

    //Home inbound - [IN PURGATORY]
    // set_velocity(-10);
    // while (digitalReadFast(m_hall_inbound) == 1) {
    //     m_encoder_inbound = encoder.read();
    // }

    // //Testing encoder reading by shifting all the way back to the inbound
    // delay(500);
    // run_state(motor_number, 8, false, 0);
    // set_velocity(-3);
    // while(encoder.read() > m_encoder_inbound){
    //     Serial.print("encoder inbound: ");
    //     Serial.println(m_encoder_inbound);
    //     Serial.print("current encoder position");
    //     Serial.println(encoder.read());
    // }
    // set_velocity(0); //Stop spinning after homing
    // run_state(motor_number, 1, false, 0); //Idle state

    out[1] = m_encoder_inbound;
    out[2] = m_encoder_outbound;
    return out;
}



int* Actuator::control_function(int* out){
    //Returns an array of ints in format
    //<status, rpm, actuator_velocity, fully shifted in, fully shifted out, time_started, time_finished>
    out[5] = millis();
    control_function_count++;
    if(millis()-last_control_execution > cycle_period_millis){
        out[0] = 0;
        out[3] = 0;
        out[4] = 0;

        //Calculate Engine Speed
        float dt = millis()-last_control_execution;
        currentrpm_eg = calc_engine_rpm(dt);
        out[1] = currentrpm_eg;
        //currentrpm_eg = analogRead(A2)*4;
        last_control_execution = millis();

        //If motor is spinning too slow, then shift all the way out
        if(currentrpm_eg < min_rpm) {
            if((digitalReadFast(m_hall_outbound) == 0 || encoder.read() >= m_encoder_outbound)){
                //If below min rpm and shifted out all the way
                odrive.run_state(motor_number, 1, false, 0); //SET IDLE
                out[0] = 4; //Report status
                out[2] = 0; //Report velocity
                out[4] = 1;
                out[6] = millis();
                return out;
            }
            else {
                //If below min rpm and not shifted out all the way
                odrive.set_velocity(motor_number, 3); //Shift out
                out[0] = 5;
                out[2] = 3;
                out[6] = millis();
                return out;
            }
        }

        //Compute error

        //If error is within a certain deviation from the desired value, do not shift
        int error = currentrpm_eg - desired_rpm;
        if (abs(error) <= rpm_allowance) {
            error = 0;
        }

        //If error will over or under actuate actuator then set error to 0.
        if(digitalReadFast(m_hall_outbound) == 0 || encoder.read() >= m_encoder_outbound){
            //Shifted out completely
            if(error > 0) error = 0;
            out[0] = 6;
            out[4] = 1;
        } 
        else if (digitalReadFast(m_hall_inbound) == 0 || encoder.read()<= m_encoder_inbound){
            //Shifted in completely
            if(error < 0) error = 0;
            out[0] = 7;
            out[3] = 1;
        }

        //Multiply by gain and set new motor velocity. 
        int motor_velocity = proportionalGain*error;
        out[2] = motor_velocity;

        if(motor_velocity == 0) {
            odrive.run_state(motor_number, 1, false, 0);
        }
        else {
            odrive.set_velocity(motor_number, motor_velocity);
            odrive.run_state(motor_number, 8, false, 0);
        }
        out[6] = millis();
        return out;
    }

    //Return status if attempt to run the control function too soon
    out[0] = 3;
    out[6] = millis();
    return out;
}

//----------------Geartooth Functions----------------//
void Actuator::count_egTooth(){
    egTooth_Count++;
}

double Actuator::calc_engine_rpm(float dt){
    noInterrupts();
    double freqInminutes = 1000*60/dt;
    float rpm = (float(egTooth_Count - egTooth_Count_last)/egTeethPerRotation)*freqInminutes;
    egTooth_Count_last = egTooth_Count;
    interrupts();
    return rpm;
}

//-----------------Diagnostic Functions--------------//

String Actuator::diagnostic(bool printSerial = true){
    //General diagnostic tool to record sensor readings as well as some odrive info

    String output = "";
    output += "-----------------------------\n";
    output += "Time: " +String(millis())+"\n";
    output += "Odrive voltage: " +String(odrive.get_voltage())+"\n";
    output += "Odrive speed: " +String(odrive.get_vel(motor_number))+"\n";
    output += "Encoder count: " +String(encoder.read())+"\n";
    output += "Outbound limit: " +String(m_encoder_outbound)+"\n";
    output += "Inbound limit: " +String(m_encoder_inbound)+"\n";
    output += "Outbound reading: " +String(digitalReadFast(m_hall_outbound))+"\n";
    output += "Inbound reading: " +String(digitalReadFast(m_hall_inbound))+"\n";
    output += "Engine Gear Tooth Count: " +String(egTooth_Count)+"\n";
    output += "Current rpm: " +String(currentrpm_eg)+"\n";

    if(m_printToSerial && printSerial){
        Serial.print(output);
    }
    return output;
}

float Actuator::communication_speed(){
    //Tests communication speed with the odrive and returns the result as a float
    const int data_points = 1000;
    int com_start = 0;
    int com_end = 0;
    int com_total = 0;
    int com_bench = 0;
    //float test = 0;
    odrive.run_state(motor_number, 8, false, 0);
    odrive.set_velocity(motor_number, .5); 
    delay(1000);

    //Benchmark
    for(int i = 0; i < data_points; i++){
        com_start = millis();
        com_end = millis();
        com_bench += com_end-com_start;
    }
    
    Serial.println(com_bench);

    //With command to odrive
    for(int i = 0; i < data_points; i++){
        com_start = millis();

        //command to odrive
        //test = odrive.get_vel(motor_number);

        com_end = millis();
        com_total += com_end-com_start;
    }
    Serial.println(com_total);
    odrive.set_velocity(motor_number, 0); //Stop spinning after homing
    odrive.run_state(motor_number, 1, false, 0);

    return float(com_total-com_bench)/float(data_points);
}

void Actuator::test_voltage(){
    //Reads bus voltage immediately after the odrive moves
    delay(1000);
    Serial.println("Reading Voltage");
    odrive.run_state(motor_number, 8, false, 0); //Tells Odrive to rotate motor
    odrive.set_velocity(motor_number, -1); 
    for(int i = 0; i < 250; i++){
        Serial.println(odrive.get_voltage()); //Show bus voltage on serial moniter
        delay(10);
    }
    odrive.run_state(motor_number, 1, false, 0); //Tell Odrive to stop rotating
    odrive.set_velocity(motor_number, 0); 
}


float Actuator::get_p_value() {
    return proportionalGain;
}