#include <Actuator.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <Encoder.h>

// Print with stream operator
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

Actuator::Actuator(HardwareSerial& serial, const int enc_A, const int enc_B, const int egTooth, const int gbTooth, const int hall_inbound, const int hall_outbound)
    :serial_(serial), encoder(enc_A, enc_B){
    // save pin values
    m_egTooth = egTooth;
    m_gbTooth = gbTooth;
    m_hall_inbound = hall_inbound;
    m_hall_outbound = hall_outbound;

    hasRun = false;
}

void Actuator::initialize(){
    Serial.println("Connecting to Odrive!");
    serial_.begin(115200);

    Serial.begin(115200);
    while (!Serial) ; // wait for Arduino Serial Monitor to open

    run_state(0, 1, true, 0);

    Serial.println("Connected");
    Serial.println("Calibrating");

    run_state(0, 8, false, 0);
    while (digitalReadFast(m_hall_outbound) == 1) {
        set_velocity(10);
        m_encoder_outbound = encoder.read();
    }
    Serial.print("encoder outbound: ");
    Serial.println(m_encoder_outbound);


    while (digitalReadFast(m_hall_inbound) == 1) {
        set_velocity(-10);
        m_encoder_inbound = encoder.read();
    }
    Serial.print("encoder inbound: ");
    Serial.println(m_encoder_inbound);

    set_velocity(0);
    run_state(0, 1, false, 0);
    
}

void Actuator::control_function(){
    
    if(!hasRun){
        Serial.println("OogaBookga!");
        set_velocity(10);
        run_state(0, 8, false, 0);
        delay(5000);
        run_state(0, 1, false, 0);
        Serial.println(dump_errors());
        hasRun = true;
    }
}

void Actuator::set_velocity(float velocity) {
    serial_ << "v " << m_motor_number  << " " << velocity << " " << "0.0f" << "\n";;
}

int32_t Actuator::read_int() {
    return read_string().toInt();
}
 
bool Actuator::run_state(int axis, int requested_state, bool wait_for_idle, float timeout) {
    int timeout_ctr = (int)(timeout * 10.0f);
    serial_ << "w axis" << axis << ".requested_state " << requested_state << '\n';
    if (wait_for_idle) {
        do {
            delay(100);
            serial_ << "r axis" << axis << ".current_state\n";
        } while (read_int() != 1 && --timeout_ctr > 0);
    }

    return timeout_ctr > 0;
}

String Actuator::read_string() {
    String str = "";
    static const unsigned long timeout = 1000;
    unsigned long timeout_start = millis();
    for (;;) {
        while (!serial_.available()) {
            if (millis() - timeout_start >= timeout) {
                return str;
            }
        }
        char c = serial_.read();
        if (c == '\n')
            break;
        str += c;
    }
    return str;
}

String Actuator::dump_errors(){
    String output= "";
    output += "system: ";

    serial_<< "r error\n";
    output += Actuator::read_string();
    for (int axis = 0; axis < 2; ++axis){
        output += "\naxis";
        output += axis;

        output += "\n  axis: ";
        serial_<< "r axis"<<axis<<".error\n";
        output += Actuator::read_string();

        output += "\n  motor: ";
        serial_<< "r axis"<<axis<<".motor.error\n";
        output += Actuator::read_string();

        output += "\n  sensorless_estimator: ";
        serial_<< "r axis"<<axis<<".sensorless_estimator.error\n";
        output += Actuator::read_string();

        output += "\n  encoder: ";
        serial_<< "r axis"<<axis<<".encoder.error\n";
        output += Actuator::read_string();

        output += "\n  controller: ";
        serial_<< "r axis"<<axis<<".controller.error\n";
        output += Actuator::read_string();
    }
    return output;
}

int Actuator::get_encoder_pos(){
    return encoder.read();
}