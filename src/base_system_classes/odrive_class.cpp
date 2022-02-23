#include <HardwareSerial.h>
#include <ODrive.h>
#include <ODriveEnums.h>
#include <SoftwareSerial.h>

template <class T>
inline Print &operator<<(Print &obj, T arg)
{
    obj.print(arg);
    return obj;
}
template <>
inline Print &operator<<(Print &obj, float arg)
{
    obj.print(arg, 4);
    return obj;
}

ODrive::ODrive(HardwareSerial &odrive_serial) : m_odrive_serial(odrive_serial)
{
    ODrive::k_system_errors[0] = "ODRIVE_ERROR_NONE";
    ODrive::k_system_errors[1] = "ODRIVE_ERROR_CONTROL_ITERATION_MISSED";
    ODrive::k_system_errors[2] = "ODRIVE_ERROR_DC_BUS_UNDER_VOLTAGE";
    ODrive::k_system_errors[3] = "ODRIVE_ERROR_DC_BUS_OVER_VOLTAGE";
    ODrive::k_system_errors[4] = "ODRIVE_ERROR_DC_BUS_OVER_REGEN_CURRENT";
    ODrive::k_system_errors[5] = "ODRIVE_ERROR_DC_BUS_OVER_CURRENT";
    ODrive::k_system_errors[6] = "ODRIVE_ERROR_BRAKE_DEADTIME_VIOLATION";
    ODrive::k_system_errors[7] = "ODRIVE_ERROR_BRAKE_DUTY_CYCLE_NAN";
    ODrive::k_system_errors[8] = "ODRIVE_ERROR_INVALID_BRAKE_RESISTANCE";
    ODrive::k_system_errors[9] = "NULL";

    ODrive::k_can_errors[0] = "CAN_ERROR_NONE";
    ODrive::k_can_errors[1] = "CAN_ERROR_DUPLICATE_CAN_IDS";
    ODrive::k_can_errors[2] = "NULL";

    ODrive::k_axis_errors[0] = "AXIS_ERROR_NONE";
    ODrive::k_axis_errors[1] = "AXIS_ERROR_INVALID_STATE";
    ODrive::k_axis_errors[12] = "AXIS_ERROR_WATCHDOG_TIMER_EXPIRED";
    ODrive::k_axis_errors[13] = "AXIS_ERROR_MIN_ENDSTOP_PRESSED";
    ODrive::k_axis_errors[14] = "AXIS_ERROR_MAX_ENDSTOP_PRESSED";
    ODrive::k_axis_errors[15] = "AXIS_ERROR_ESTOP_REQUESTED";
    ODrive::k_axis_errors[18] = "AXIS_ERROR_HOMING_WITHOUT_ENDSTOP";
    ODrive::k_axis_errors[19] = "AXIS_ERROR_OVER_TEMP";
    ODrive::k_axis_errors[20] = "AXIS_ERROR_UNKNOWN_POSITION";
    ODrive::k_axis_errors[21] = "NULL";

    ODrive::k_motor_errors[0] = "MOTOR_ERROR_NONE";
    ODrive::k_motor_errors[1] = "MOTOR_ERROR_PHASE_RESISTANCE_OUT_OF_RANGE";
    ODrive::k_motor_errors[2] = "MOTOR_ERROR_PHASE_INDUCTANCE_OUT_OF_RANGE";
    ODrive::k_motor_errors[4] = "MOTOR_ERROR_DRV_FAULT";
    ODrive::k_motor_errors[5] = "MOTOR_ERROR_CONTROL_DEADLINE_MISSED";
    ODrive::k_motor_errors[8] = "MOTOR_ERROR_MODULATION_MAGNITUDE";
    ODrive::k_motor_errors[11] = "MOTOR_ERROR_CURRENT_SENSE_SATURATION";
    ODrive::k_motor_errors[13] = "MOTOR_ERROR_CURRENT_LIMIT_VIOLATION";
    ODrive::k_motor_errors[17] = "MOTOR_ERROR_MODULATION_IS_NAN";
    ODrive::k_motor_errors[18] = "MOTOR_ERROR_MOTOR_THERMISTOR_OVER_TEMP";
    ODrive::k_motor_errors[19] = "MOTOR_ERROR_FET_THERMISTOR_OVER_TEMP";
    ODrive::k_motor_errors[20] = "MOTOR_ERROR_TIMER_UPDATE_MISSED";
    ODrive::k_motor_errors[21] = "MOTOR_ERROR_CURRENT_MEASUREMENT_UNAVAILABLE";
    ODrive::k_motor_errors[22] = "MOTOR_ERROR_CONTROLLER_FAILED";
    ODrive::k_motor_errors[23] = "MOTOR_ERROR_I_BUS_OUT_OF_RANGE";
    ODrive::k_motor_errors[24] = "MOTOR_ERROR_BRAKE_RESISTOR_DISARMED";
    ODrive::k_motor_errors[25] = "MOTOR_ERROR_SYSTEM_LEVEL";
    ODrive::k_motor_errors[26] = "MOTOR_ERROR_BAD_TIMING";
    ODrive::k_motor_errors[27] = "MOTOR_ERROR_UNKNOWN_PHASE_ESTIMATE";
    ODrive::k_motor_errors[28] = "MOTOR_ERROR_UNKNOWN_PHASE_VEL";
    ODrive::k_motor_errors[29] = "MOTOR_ERROR_UNKNOWN_TORQUE";
    ODrive::k_motor_errors[30] = "MOTOR_ERROR_UNKNOWN_CURRENT_COMMAND";
    ODrive::k_motor_errors[31] = "MOTOR_ERROR_UNKNOWN_CURRENT_MEASUREMENT";
    ODrive::k_motor_errors[32] = "MOTOR_ERROR_UNKNOWN_VBUS_VOLTAGE";
    ODrive::k_motor_errors[33] = "MOTOR_ERROR_UNKNOWN_VOLTAGE_COMMAND";
    ODrive::k_motor_errors[34] = "MOTOR_ERROR_UNKNOWN_GAINS";
    ODrive::k_motor_errors[35] = "MOTOR_ERROR_CONTROLLER_INITIALIZING";
    ODrive::k_motor_errors[36] = "MOTOR_ERROR_UNBALANCED_PHASES";
    ODrive::k_motor_errors[37] = "NULL";

    ODrive::k_encoder_errors[0] = "ENCODER_ERROR_NONE";
    ODrive::k_encoder_errors[1] = "ENCODER_ERROR_UNSTABLE_GAIN";
    ODrive::k_encoder_errors[2] = "ENCODER_ERROR_CPR_POLEPAIRS_MISMATCH";
    ODrive::k_encoder_errors[3] = "ENCODER_ERROR_NO_RESPONSE";
    ODrive::k_encoder_errors[4] = "ENCODER_ERROR_UNSUPPORTED_ENCODER_MODE";
    ODrive::k_encoder_errors[5] = "ENCODER_ERROR_ILLEGAL_HALL_STATE";
    ODrive::k_encoder_errors[6] = "ENCODER_ERROR_INDEX_NOT_FOUND_YET";
    ODrive::k_encoder_errors[7] = "ENCODER_ERROR_ABS_SPI_TIMEOUT";
    ODrive::k_encoder_errors[8] = "ENCODER_ERROR_ABS_SPI_COM_FAIL";
    ODrive::k_encoder_errors[9] = "ENCODER_ERROR_ABS_SPI_NOT_READY";
    ODrive::k_encoder_errors[10] = "ENCODER_ERROR_HALL_NOT_CALIBRATED_YET";
    ODrive::k_encoder_errors[11] = "NULL";

    ODrive::k_controller_errors[0] = "CONTROLLER_ERROR_NONE";
    ODrive::k_controller_errors[1] = "CONTROLLER_ERROR_OVERSPEED";
    ODrive::k_controller_errors[2] = "CONTROLLER_ERROR_INVALID_INPUT_MODE";
    ODrive::k_controller_errors[3] = "CONTROLLER_ERROR_UNSTABLE_GAIN";
    ODrive::k_controller_errors[4] = "CONTROLLER_ERROR_INVALID_MIRROR_AXIS";
    ODrive::k_controller_errors[5] = "CONTROLLER_ERROR_INVALID_LOAD_ENCODER";
    ODrive::k_controller_errors[6] = "CONTROLLER_ERROR_INVALID_ESTIMATE";
    ODrive::k_controller_errors[7] = "CONTROLLER_ERROR_INVALID_CIRCULAR_RANGE";
    ODrive::k_controller_errors[8] = "CONTROLLER_ERROR_SPINOUT_DETECTED";
    ODrive::k_controller_errors[9] = "NULL";

    ODrive::k_sensorless_estimator_errors[0] = "SENSORLESS_ESTIMATOR_ERROR_NONE";
    ODrive::k_sensorless_estimator_errors[1] = "SENSORLESS_ESTIMATOR_ERROR_UNSTABLE_GAIN";
    ODrive::k_sensorless_estimator_errors[2] = "SENSORLESS_ESTIMATOR_ERROR_UNKNOWN_CURRENT_MEASUREMENT";
    ODrive::k_sensorless_estimator_errors[3] = "NULL";
}

uint64_t ODrive::string_to_ull(String s)
{
    uint32_t n_0 = s.substring(0, 8).toInt();
    uint32_t n_1 = s.substring(8).toInt();

    short zeros = s.substring(8).length();
    uint32_t multiplier = 1;
    for (int i = 0; i < zeros; i++)
    {
        multiplier *= 10;
    }
    return (n_0 * multiplier) + n_1;
}

String ODrive::ull_to_string(uint64_t n)
{
    char s[1000];
    sprintf(s, "Formatted data:  %llu", n);
    return String(s);
}

String ODrive::read_string()
{
    String str = "";
    unsigned long timeout_start = millis();
    while (1)
    {
        while (!m_odrive_serial.available())
        {
            if (millis() - timeout_start >= k_read_timeout)
            {
                return str;
            }
        }
        char c = m_odrive_serial.read();
        if (c == '\n')
            break;
        str += c;
    }
    return str;
}

int ODrive::read_int()
{
    return read_string().toInt();
}

uint64_t ODrive::read_ull()
{
    return string_to_ull(read_string());
}

float ODrive::read_float()
{
    return read_string().toFloat();
}

int ODrive::init(uint32_t timeout)
{
    m_status = 0;
    m_odrive_serial.begin(115200);

    uint32_t start = millis();
    while (ODrive::get_voltage() <= 1)
    {
        if (millis() - start > timeout)
        {
            m_status = 13;
            return m_status;
        }
    }
    return m_status;
}

String ODrive::dump_single_error(String error_lut[], String module, int axis, uint64_t error_code, bool indent)
{
    String tab = indent ? "\t" : "";
    String output = tab + module + ": ";
    if (error_code == 0)
    {
        output += "no error\n";
        return output;
    }
    output += "Error(s):\n";
    for (int bit = 0; bit < 64; bit++)
    {
        if (error_lut[bit] == "NULL")
        {
            break;
        }
        if (error_code & (1ULL << bit))
        {
            if (error_lut[bit] != "")
            {

                output += tab + "\t" + error_lut[bit] + "\n";
            }
            else
            {
                output = output + tab + "\t" + "UNKOWN ERROR (" + ull_to_string(1ULL << bit) + ")\n";
            }
        }
    }
    return output;
}

String ODrive::dump_errors()
{
    String output = "timestamp: ";
    output += millis() + "\n";

    m_odrive_serial << "r error\n";
    uint32_t system_error = read_ull();

    output += dump_single_error(k_system_errors, "system", system_error, false);
    for (int axis = 0; axis < 2; ++axis)
    {
        m_odrive_serial << "r axis" << axis << ".error\n";
        uint32_t axis_error = read_ull();
        m_odrive_serial << "r axis" << axis << "motor.error\n";
        uint32_t motor_error = read_ull();
        m_odrive_serial << "r axis" << axis << "sensorless_estimator.error\n";
        uint32_t sensorless_estimator_error = read_int();
        m_odrive_serial << "r axis" << axis << "encoder.error\n";
        uint32_t encoder_error = read_ull();
        m_odrive_serial << "r axis" << axis << "controller.error\n";
        uint32_t controller_error = read_ull();

        output += dump_single_error(k_axis_errors, "axis", axis, axis_error);
        output += dump_single_error(k_motor_errors, "motor", axis, motor_error);
        output += dump_single_error(k_sensorless_estimator_errors, "sensorless_estimator", axis, sensorless_estimator_error);
        output += dump_single_error(k_encoder_errors, "encoder", axis, encoder_error);
        output += dump_single_error(k_controller_errors, "controller", axis, controller_error);
    }
    return output;
}

String ODrive::dump_data_header()
{
    return "timestamp,motor0 current (A),motor1 current (A),motor0 torque estimate (Nm),motor1 torque estimate(Nm),voltage (V),current (A)";
}

String ODrive::dump_data()
{
    String data(millis());
    return data + "," + get_motor_current(0) + "," + get_motor_current(1) + "," + get_torque_estimate(0) + "," + get_torque_estimate(1) + "," + get_voltage() + "," + get_current();
}

void ODrive::set_velocity_gain(int axis, float vel_gain)
{
    m_odrive_serial << "w axis" << axis << ".controller.config.vel_gain" << vel_gain << '\n';
}

void ODrive::set_velocity_integrator_gain(int axis, float vel_integrator_gain)
{
    m_odrive_serial << "w axis" << axis << ".controller.config.vel_integrator_gain" << vel_integrator_gain << '\n';
}

bool ODrive::run_state(int axis, int requested_state, bool wait_for_idle, uint32_t timeout)
{
    uint32_t timeout_count = timeout / 100;
    m_odrive_serial << "w axis" << axis << ".requested_state " << requested_state << '\n';
    if (wait_for_idle)
    {
        do
        {
            delay(100);
            m_odrive_serial << "r axis" << axis << ".current_state\n";
        } while (read_int() != 1 && --timeout_count > 0);
    }

    return timeout_count > 0;
}

void ODrive::set_velocity(int axis, float velocity)
{
    m_odrive_serial << "v " << axis << " " << velocity << " "
                    << "0.0f"
                    << "\n";
}

float ODrive::get_motor_current(int axis)
{
    m_odrive_serial << "r axis" << axis << ".motor.current_control.Iq_measured\n";
    return read_float();
}

float ODrive::get_torque_estimate(int axis)
{
    m_odrive_serial << "r axis" << axis << ".motor.config.torque_constant";
    float kt = read_float();
    return kt * get_motor_current(axis);
}

float ODrive::get_velocity(int axis)
{
    m_odrive_serial << "r axis" << axis << ".encoder.vel_estimate\n";
    return read_float();
}

float ODrive::get_voltage()
{
    m_odrive_serial << "r vbus_voltage\n";
    return read_float();
}

float ODrive::get_current()
{
    m_odrive_serial << "r ibus\n";
    return read_float();
}