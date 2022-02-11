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
    int o_init = odrive.init(odrive_timeout);
    if(o_init != 0){
        status = o_init;
        return status;
    }
    
    odrive.run_state(motor_number, 1, true, 0); //Sets ODrive to IDLE 
    
    status = 0;
    return status;
    //Timer3.initialize(cycle_period);

    // interrupts(); //allows interupts
    // attachInterrupt(m_egTooth, m_external_count_egTooth, FALLING);
}

int* Actuator::homing_sequence(){
    //Returns an array of ints in format <status, inbound, outbound>

    odrive.run_state(motor_number, 8, false, 0); //Enter velocity control mode
    //TODO: Enums for IDLE, VELOCITY_CONTROL
    delay(1000);
    //Home outbound
    int start = millis();
    odrive.set_velocity(motor_number, 3);
    while (digitalReadFast(m_hall_outbound) == 1) {
        m_encoder_outbound = get_encoder_pos();
        if (millis() - start > homing_timeout) {
            status = 0041;
            odrive.run_state(motor_number, 0, false, 0);
            //log.error("Homing outbound failed, code: %d", status);
            int out[3] = {status, -1, -1};
            return out;
        }
    }
    odrive.set_velocity(motor_number, 0); //Stop spinning after homing
    odrive.run_state(motor_number, 1, false, 0); //Idle state

    //NOTE: All these commented logs show where we should be able to get data and useful readouts
    //log.notice("Homed outbound successfully, code: %d", status);

    m_encoder_inbound = m_encoder_outbound - encoderCountShiftLength;

    //Home inbound - [IN PURGATORY]
    // set_velocity(-10);
    // while (digitalReadFast(m_hall_inbound) == 1) {
    //     m_encoder_inbound = encoder.read();
    // }

    Serial.print(odrive.dump_errors());

    

    // //Testing encoder reading by shifting all the way back to the inbound
    // delay(500);
    // run_state(motor_number, 8, false, 0);
    // set_velocity(-3);
    // while(get_encoder_pos() > m_encoder_inbound){
    //     Serial.print("encoder inbound: ");
    //     Serial.println(m_encoder_inbound);
    //     Serial.print("current encoder position");
    //     Serial.println(get_encoder_pos());
    // }
    // set_velocity(0); //Stop spinning after homing
    // run_state(motor_number, 1, false, 0); //Idle state

    if(m_printToSerial){
        Serial.println("--------------------------");
        Serial.print("encoder outbound: ");
        Serial.println(m_encoder_outbound);
        Serial.print("encoder inbound: ");
        Serial.println(m_encoder_inbound);
        Serial.print("current encoder position");
    }
    //log.notice("Encoder inbound: %u Encoder outbound: %u", m_encoder_inbound, m_encoder_outbound);

    int out[3] = {status, m_encoder_inbound, m_encoder_outbound};
    return out;
}



void Actuator::control_function(){
    control_function_count++;
    if(millis()-last_control_execution > cycle_period_millis){

        //Calculate Engine Speed
        float dt = millis()-last_control_execution;
        currentrpm_eg = calc_engine_rpm(dt);
        //currentrpm_eg = analogRead(A2)*4;
        last_control_execution = millis();

        //Print Engine Speed
        if (m_printToSerial){
        Serial.print("Current rpm: ");
        Serial.println(currentrpm_eg);
        Serial.print("GearToothCount: ");
        Serial.println(egTooth_Count);
        }

        //log.notice("Current rpm, Gear Tooth Count: %f, %u", currentrpm_eg, egTooth_Count);

        // //Compute error
        // int error = currentrpm_eg - 2700;

        // //If error will over or under actuate actuator then set error to 0.
        // if(digitalReadFast(m_hall_outbound) == 0 || get_encoder_pos() >= m_encoder_outbound){
        //     if(error > 0) error = 0;
        //     Serial.println("hi");
        // } 
        // else if (digitalReadFast(m_hall_inbound) == 0 || get_encoder_pos()<= m_encoder_inbound){
        //     if(error < 0) error = 0;
        //     Serial.println("hey");
        // }

        // //Multiply by gain and set new motor velocity. 
        // int motor_velocity = proportionalGain*error;
        // if (m_printToSerial){
        // Serial.print("Current motor_velocity: ");
        // Serial.println(motor_velocity);
        // }

        // if(motor_velocity == 0){
        //     run_state(motor_number, 1, false, 0);
        // } else{
        //     set_velocity(motor_velocity);
        //     run_state(motor_number, 8, false, 0);
        // }
        
    }
}


//------Diagnostic Function to print Sensors---------//
String Actuator::diagnostic(bool printSerial = true){

    if(m_printToSerial && printSerial){
        Serial.println("-----------------------------");
        Serial.print("Current time: ");
        Serial.println(millis());
        //Odrive voltage
        // Serial.print("Odrive Voltage: ");
        // Serial.println( odrive.get_voltage());
        // //Odrive speed
        // Serial.print("Odrive's current Speed: ");
        // Serial.println(odrive.get_vel(motor_number));
        // //Encoder Count
        // Serial.print("Current Encoder Count:");
        // Serial.print(get_encoder_pos());
        //outbound encoder reading
        Serial.print("Outbound Limit: ");
        Serial.println(m_encoder_outbound);
        //inbound encoder reading
        Serial.print("Inbound Limit: ");
        Serial.println(m_encoder_inbound);
        //If inbound hall on
        Serial.print("Inbound Hall reading: ");
        Serial.println(digitalReadFast(m_hall_inbound));
        //if outbound hall on
        Serial.print("outbound Hall reading: ");
        Serial.println(digitalReadFast(m_hall_outbound));
        //current Gear Tooth count
        Serial.print("Engine Gear Tooth Count: ");
        Serial.println(egTooth_Count);
        //current Engine Speed
        Serial.print("Engine rpm: ");
        Serial.println(currentrpm_eg);
    }

    String output = "";
    output += "-----------------------------\n";
    output += "Time: " +String(millis())+"\n";
    output += "Odrive voltage: " +String(odrive.get_voltage())+"\n";
    output += "Odrive speed: " +String(odrive.get_vel(motor_number))+"\n";
    output += "Encoder count: " +String(get_encoder_pos())+"\n";
    output += "Outbound limit: " +String(m_encoder_outbound)+"\n";
    output += "Inbound limit: " +String(m_encoder_inbound)+"\n";
    output += "Outbound reading: " +String(digitalReadFast(m_hall_outbound))+"\n";
    output += "Inbound reading: " +String(digitalReadFast(m_hall_inbound))+"\n";
    output += "Engine Gear Tooth Count: " +String(egTooth_Count)+"\n";
    output += "Current rpm: " +String(currentrpm_eg)+"\n";

    return output;
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
//John asked how long it takes to talk to the odrive so this is a little benchmark test for debugging
float Actuator::communication_speed(){
    const int data_points = 1000;
    int com_start = 0;
    int com_end = 0;
    int com_total = 0;
    int com_bench = 0;
    float test = 0;
    odrive.run_state(motor_number, 8, false, 0);
    odrive.set_velocity(motor_number, .5); 
    delay(1000);

    //Benchmark
    for(int i; i < data_points; i++){
        com_start = millis();
        com_end = millis();
        com_bench += com_end-com_start;
    }
    
    Serial.println(com_bench);

    //With command to odrive
    for(int i; i < data_points; i++){
        com_start = millis();

        //command to odrive
        test = odrive.get_vel(motor_number);

        com_end = millis();
        com_total += com_end-com_start;
    }
    Serial.println(com_total);
    odrive.set_velocity(motor_number, 0); //Stop spinning after homing
    odrive.run_state(motor_number, 1, false, 0);

    //log.notice("Communication speed: %f", (float(com_total - com_bench)/float(data_points)));
    return float(com_total-com_bench)/float(data_points);
}




//Been having bus voltage issues with the odrive. I want to see what happens
//to bus voltage right after I command the odrive to move.
void Actuator::test_voltage(){
    delay(1000);
    Serial.println("Reading Voltage");
    odrive.run_state(motor_number, 8, false, 0); //Tells Odrive to rotate motor
    odrive.set_velocity(motor_number, -1); 
    for(int i; i < 250; i++){
        Serial.println(odrive.get_voltage()); //Show bus voltage on serial moniter
        delay(10);
    }
    odrive.run_state(motor_number, 1, false, 0); //Tell Odrive to stop rotating
    odrive.set_velocity(motor_number, 0); 
}

//-----------------ODrive Getters--------------//
// int Actuator::get_encoder_count(){
//     OdriveSerial<< "r axis" << motor_number << ".encoder.shadow_count\n";
//     return odrive.read_int();
// }

//Function for when the encoder is plugged into teensy probably will be removed
int Actuator::get_encoder_pos(){
    return encoder.read();
}