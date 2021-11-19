#include <Actuator.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <Encoder.h>
#include <TimerOne.h>

// Print with stream operator
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

void be_happy() {
    ;
}

Actuator::Actuator(HardwareSerial& serial, const int enc_A, const int enc_B, const int egTooth, const int gbTooth, const int hall_inbound, const int hall_outbound, void (*external_interrupt_handler)())
    :Serial(serial), encoder(enc_A, enc_B){
    // save pin values
    m_egTooth = egTooth;
    m_gbTooth = gbTooth;
    m_hall_inbound = hall_inbound;
    m_hall_outbound = hall_outbound;
    m_external_interrupt_handler = external_interrupt_handler;

    hasRun = false;
}

int Actuator::init(){
    control_function_count = 0;
    Serial.begin(115200); //This is connection to ODrive

    //TODO: Have some kinda timout for this
    while(!Serial); //Wait for arduino -- Odrive connection

    run_state(0, 1, true, 0); //Sets ODrive to IDLE

    run_state(0, 8, false, 0); //Enter velocity control mode
    //TODO: Enums for IDLE, VELOCITY_CONTROL

    //Home outbound
    int start = millis();
    set_velocity(10);
    while (digitalReadFast(m_hall_outbound) == 1) {
        m_encoder_outbound = encoder.read();
        if (millis() - start > HOMING_TIMEOUT) {
            status = 0041;
            return status;
        }
    }
    //Home inbound - [IN PURGATORY]
    // set_velocity(-10);
    // while (digitalReadFast(m_hall_inbound) == 1) {
    //     m_encoder_inbound = encoder.read();
    // }

    set_velocity(0); //Stop spinning after homing
    run_state(0, 1, false, 0); //Idle state

    Timer1.initialize(CYCLE_TIME);
    Timer1.attachInterrupt(m_external_interrupt_handler);
}

void Actuator::control_function(){
    
    control_function_count++;
    run_state(0, 1, true, 0); //Sets ODrive to IDLE
    // run_state(0, 8, false, 0); //Enter velocity control mode
    // if(!hasRun){
    //     Serial.println("OogaBookga!");
    //     set_velocity(10);
    //     run_state(0, 8, false, 0);
    //     delay(5000);
    //     run_state(0, 1, false, 0);
    //     Serial.println(dump_errors());
    //     hasRun = true;
    // }
}

void Actuator::set_velocity(float velocity) {
    Serial << "v " << m_motor_number  << " " << velocity << " " << "0.0f" << "\n";;
}

int32_t Actuator::read_int() {
    return read_string().toInt();
}
 
bool Actuator::run_state(int axis, int requested_state, bool wait_for_idle, float timeout) {
    int timeout_ctr = (int)(timeout * 10.0f);
    Serial << "w axis" << axis << ".requested_state " << requested_state << '\n';
    if (wait_for_idle) {
        do {
            delay(100);
            Serial << "r axis" << axis << ".current_state\n";
        } while (read_int() != 1 && --timeout_ctr > 0);
    }

    return timeout_ctr > 0;
}

String Actuator::read_string() {
    String str = "";
    static const unsigned long timeout = 1000;
    unsigned long timeout_start = millis();
    for (;;) {
        while (!Serial.available()) {
            if (millis() - timeout_start >= timeout) {
                return str;
            }
        }
        char c = Serial.read();
        if (c == '\n')
            break;
        str += c;
    }
    return str;
}

String Actuator::dump_errors(){
    String output= "";
    output += "system: ";

    Serial<< "r error\n";
    output += Actuator::read_string();
    for (int axis = 0; axis < 2; ++axis){
        output += "\naxis";
        output += axis;

        output += "\n  axis: ";
        Serial<< "r axis"<<axis<<".error\n";
        output += Actuator::read_string();

        output += "\n  motor: ";
        Serial<< "r axis"<<axis<<".motor.error\n";
        output += Actuator::read_string();

        output += "\n  sensorless_estimator: ";
        Serial<< "r axis"<<axis<<".sensorless_estimator.error\n";
        output += Actuator::read_string();

        output += "\n  encoder: ";
        Serial<< "r axis"<<axis<<".encoder.error\n";
        output += Actuator::read_string();

        output += "\n  controller: ";
        Serial<< "r axis"<<axis<<".controller.error\n";
        output += Actuator::read_string();
    }
    return output;
}

int Actuator::get_encoder_pos(){
    return encoder.read();
}