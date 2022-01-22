#include <Actuator.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <Encoder.h>
#include <TimerThree.h>

// Print with stream operator
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

Actuator::Actuator(HardwareSerial& serial,
    const int enc_A, 
    const int enc_B, 
    const int egTooth, 
    const int gbTooth, 
    const int hall_inbound, 
    const int hall_outbound, 
    void (*external_interrupt_handler)(), 
    void (*external_count_egTooth)(),
    bool printToSerial)
    :OdriveSerial(serial), encoder(enc_A, enc_B){

    //Save pin values
    m_egTooth = egTooth;
    m_gbTooth = gbTooth;
    m_hall_inbound = hall_inbound;
    m_hall_outbound = hall_outbound;

    m_printToSerial = printToSerial;

    //initialize count vairables
    egTooth_Count = 0;
    egTooth_Count_last = 0;

    //Functions to support interrupt
    m_external_interrupt_handler = external_interrupt_handler;
    m_external_count_egTooth = external_count_egTooth;

    //Test variable
    hasRun = false;
}

int Actuator::init(){
    status = 0;
    control_function_count = 0; //Testing var

    if(m_printToSerial) Serial.println("Connecting to Odrive");

    OdriveSerial.begin(115200); //This is connection to ODrive


    long start = millis();
    while(get_voltage() <= 1){
        if(millis() - start > homing_timeout){
            status = 0051;
            if(m_printToSerial) Serial.println("Connection Failed");
            return -status;
        }
    } //Wait for Teensy <--> Odrive connection (Uses same timeout as homing)

    if(m_printToSerial) Serial.println("Connected");

    run_state(motor_number, 1, true, 0); //Sets ODrive to IDLE 
    status = homing_sequence();
    if(status != 0) return status;

    //test_voltage(); //voltage testing code

    //Starts interrupt timer and attaches method to interrupt
    interrupts(); //allows interupts
    Timer3.initialize(cycle_period);
    /*
    Let me tell you kids a story about a burning dorm and 3 hours to spare.
    All jokes aside, basically we are unable to attach the actuator control function method
    directly to the interupt as it wouldn't know which object to execute the method on.
    As we only will have one object, we always know which object to execute the method on,
    thus this function calls the method on the object we make.
    Basically: CS VooDoo magic words make things way harder than they should be
    */
    Timer3.attachInterrupt(m_external_interrupt_handler);

    attachInterrupt(m_egTooth, m_external_count_egTooth, FALLING);
    return status;
}

int Actuator::homing_sequence(){
    run_state(motor_number, 8, false, 0); //Enter velocity control mode
    //TODO: Enums for IDLE, VELOCITY_CONTROL
    delay(1000);
    //Home outbound
    int start = millis();
    set_velocity(1);
    while (digitalReadFast(m_hall_outbound) == 1) {
        m_encoder_outbound = get_encoder_pos();
        if (millis() - start > homing_timeout) {
            status = 0041;
            run_state(motor_number, 0, false, 0);
            return status;
        }
    }

    m_encoder_inbound = m_encoder_outbound - encoderCountShiftLength;

    //Home inbound - [IN PURGATORY]
    // set_velocity(-10);
    // while (digitalReadFast(m_hall_inbound) == 1) {
    //     m_encoder_inbound = encoder.read();
    // }

    Serial.print(dump_errors());

    

    //Testing encoder reading by shifting all the way back to the inbound
    delay(500);
    run_state(motor_number, 8, false, 0);
    set_velocity(-2);
    while(get_encoder_pos() > m_encoder_inbound){
        Serial.print("encoder inbound: ");
        Serial.println(m_encoder_inbound);
        Serial.print("current encoder position");
        Serial.println(get_encoder_pos());
    }
    set_velocity(0); //Stop spinning after homing
    run_state(motor_number, 1, false, 0); //Idle state

    if(m_printToSerial){
        Serial.println("--------------------------");
        Serial.print("encoder outbound: ");
        Serial.println(m_encoder_outbound);
        Serial.print("encoder inbound: ");
        Serial.println(m_encoder_inbound);
        Serial.print("current encoder position");
    }

    return 0;
}



void Actuator::control_function(){
    control_function_count++;

    //Calculate Gear tooth speed on engine
    
}

//----------------Geartooth Functions----------------//
void Actuator::count_egTooth(){
    egTooth_Count++;
}

int Actuator::calc_engine_rpm(){
    noInterrupts();
    float rpm = (egTooth_Count - egTooth_Count_last)*cycle_frequency_minutes/egTeethPerRotation;
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
    run_state(motor_number, 8, false, 0);
    set_velocity(.5); 
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
        test = get_vel();

        com_end = millis();
        com_total += com_end-com_start;
    }
    Serial.println(com_total);
    set_velocity(0); //Stop spinning after homing
    run_state(motor_number, 1, false, 0);

    return float(com_total-com_bench)/float(data_points);
}




//Been having bus voltage issues with the odrive. I want to see what happens
//to bus voltage right after I command the odrive to move.
void Actuator::test_voltage(){
    delay(1000);
    Serial.println("Reading Voltage");
    run_state(motor_number, 8, false, 0); //Tells Odrive to rotate motor
    set_velocity(-1); 
    for(int i; i < 250; i++){
        Serial.println(get_voltage()); //Show bus voltage on serial moniter
        delay(10);
    }
    run_state(motor_number, 1, false, 0); //Tell Odrive to stop rotating
    set_velocity(0); 
}





//-----------------ODrive Setters--------------//
bool Actuator::run_state(int axis, int requested_state, bool wait_for_idle, float timeout) {
    int timeout_ctr = (int)(timeout * 10.0f);
    OdriveSerial << "w axis" << axis << ".requested_state " << requested_state << '\n';
    if (wait_for_idle) {
        do {
            delay(100);
            OdriveSerial << "r axis" << axis << ".current_state\n";
        } while (read_int() != 1 && --timeout_ctr > 0);
    }

    return timeout_ctr > 0;
}

void Actuator::set_velocity(float velocity) {
    OdriveSerial << "v " << motor_number  << " " << velocity << " " << "0.0f" << "\n";;
}



//-----------------ODrive Getters--------------//
float Actuator::get_vel() {
	OdriveSerial<< "r axis" << motor_number << ".encoder.vel_estimate\n";
	return Actuator::read_float();
}

int Actuator::get_encoder_count(){
    OdriveSerial<< "r axis" << motor_number << ".encoder.shadow_count\n";
    return Actuator::read_int();
}

float Actuator::get_voltage() {
    OdriveSerial<< "r vbus_voltage\n";
    return Actuator::read_float();
}

String Actuator::dump_errors(){
    String output= "";
    output += "system: ";

    OdriveSerial<< "r error\n";
    output += Actuator::read_string();
    for (int axis = 0; axis < 2; ++axis){
        output += "\naxis";
        output += axis;

        output += "\n  axis: ";
        OdriveSerial<< "r axis"<<axis<<".error\n";
        output += Actuator::read_string();

        output += "\n  motor: ";
        OdriveSerial<< "r axis"<<axis<<".motor.error\n";
        output += Actuator::read_string();

        output += "\n  sensorless_estimator: ";
        OdriveSerial<< "r axis"<<axis<<".sensorless_estimator.error\n";
        output += Actuator::read_string();

        output += "\n  encoder: ";
        OdriveSerial<< "r axis"<<axis<<".encoder.error\n";
        output += Actuator::read_string();

        output += "\n  controller: ";
        OdriveSerial<< "r axis"<<axis<<".controller.error\n";
        output += Actuator::read_string();
    }
    return output;
}


String Actuator::read_string() {
    String str = "";
    static const unsigned long timeout = 1000;
    unsigned long timeout_start = millis();
    for (;;) {
        while (!OdriveSerial.available()) {
            if (millis() - timeout_start >= timeout) {
                return str;
            }
        }
        char c = OdriveSerial.read();
        if (c == '\n')
            break;
        str += c;
    }
    return str;
}

float Actuator::read_float() {
    return read_string().toFloat();
}

int32_t Actuator::read_int() {
    return read_string().toInt();
}



//Function for when the encoder is plugged into teensy probably will be removed
int Actuator::get_encoder_pos(){
    return encoder.read();
}