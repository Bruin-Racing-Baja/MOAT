#include <HardwareSerial.h>
#include <ODrive.h>
#include <SoftwareSerial.h>

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

ODrive::ODrive(HardwareSerial& serial) : OdriveSerial(serial)
{
}

int ODrive::init(int timeout)
{
  /*
  Initializes ODrive <--> Teensy
  Will wait for connection, and return error if unsuccessful after timeout

  */
  OdriveSerial.begin(115200);

  long start = millis();
  while (ODrive::get_voltage() <= 1)
  {
    if (millis() - start > timeout)
    {
      status = 13;
      return status;
    }
  }
  status = 0;
  return status;
};

//-----------------ODrive Setters--------------//
bool ODrive::run_state(int axis, int requested_state, bool wait_for_idle, float timeout)
{
  // Dont set odrive to same state again
  if (requested_state == m_current_state) return false;

  int timeout_ctr = (int)(timeout * 10.0f);
  OdriveSerial << "w axis" << axis << ".requested_state " << requested_state << '\n';
  if (wait_for_idle)
  {
    do
    {
      delay(100);
      OdriveSerial << "r axis" << axis << ".current_state\n";
    } while (read_int() != 1 && --timeout_ctr > 0);
  }
  if (timeout_ctr > 0)
  {
    m_current_state = requested_state;
    return true;
  }
  else return false;
};

void ODrive::set_velocity(int motor_number, float velocity)
{
  OdriveSerial << "v " << motor_number << " " << velocity << " "
               << "0.0f"
               << "\n";
  ;
}

//-----------------ODrive Getters--------------//
float ODrive::get_vel(int motor_number)
{
  OdriveSerial << "r axis" << motor_number << ".encoder.vel_estimate\n";
  return ODrive::read_float();
}

float ODrive::get_voltage()
{
  OdriveSerial << "r vbus_voltage\n";
  return ODrive::read_float();
}

float ODrive::get_encoder_pos(int motor_number){
  OdriveSerial << "r axis" << motor_number << ".encoder.shadow_count\n";
  return ODrive::read_float();
}

float ODrive::get_cur()
{
  OdriveSerial << "r ibus\n";
  return ODrive::read_float();
}

String ODrive::dump_errors()
{
  String output = "";
  output += "system: ";

  OdriveSerial << "r error\n";
  output += ODrive::read_string();
  for (int axis = 0; axis < 2; ++axis)
  {
    output += "\naxis";
    output += axis;

    output += "\n  axis: ";
    OdriveSerial << "r axis" << axis << ".error\n";
    output += ODrive::read_string();

    output += "\n  motor: ";
    OdriveSerial << "r axis" << axis << ".motor.error\n";
    output += ODrive::read_string();

    output += "\n  sensorless_estimator: ";
    OdriveSerial << "r axis" << axis << ".sensorless_estimator.error\n";
    output += ODrive::read_string();

    output += "\n  encoder: ";
    OdriveSerial << "r axis" << axis << ".encoder.error\n";
    output += ODrive::read_string();

    output += "\n  controller: ";
    OdriveSerial << "r axis" << axis << ".controller.error\n";
    output += ODrive::read_string();
  }
  return output;
}

String ODrive::read_string()
{
  String str = "";
  static const unsigned long timeout = 1000;
  unsigned long timeout_start = millis();
  for (;;)
  {
    while (!OdriveSerial.available())
    {
      if (millis() - timeout_start >= timeout)
      {
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

float ODrive::read_float()
{
  return read_string().toFloat();
}

int32_t ODrive::read_int()
{
  return read_string().toInt();
}
