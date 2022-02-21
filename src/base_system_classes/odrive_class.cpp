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

ODrive::ODrive(HardwareSerial &odrive_serial) : m_odrive_serial(odrive_serial) { populate_errors(); }

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
    String output = "";

    m_odrive_serial << "r error\n";
    uint32_t system_error = read_ull();

    output += dump_single_error(system_errors, "system", system_error, false);
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

        output += dump_single_error(axis_errors, "axis", axis, axis_error);
        output += dump_single_error(motor_errors, "motor", axis, motor_error);
        output += dump_single_error(sensorless_estimator_errors, "sensorless_estimator", axis, sensorless_estimator_error);
        output += dump_single_error(encoder_errors, "encoder", axis, encoder_error);
        output += dump_single_error(controller_errors, "controller", axis, controller_error);
    }
    return output;
}

String ODrive::dump_data_header()
{
    return "timestamp,velocity0 (rpm),velocity1 (rpm),voltage (V),current (A)";
}

String ODrive::dump_data()
{
    String data(millis());
    return data + "," + get_velocity(0) + "," + get_velocity(1) + "," + get_voltage() + "," + get_current();
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
