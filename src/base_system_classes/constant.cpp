#include <Constant.h>
#include <SD.h>

void Constant::init(File settingsFile_i, int defaultValueMode = 0)
{
  /*
  Passed settings file to read from
  If settings file cant be opened, passing an int will set the default value mode, where the mode is the passed int
  and all the other values will be set to default values (Can pass a nullptr to not use a settings file)
  */
  settingsFile = settingsFile_i;
  if (defaultValueMode)
  {
    // Set mode to passed value, leave all others as defaults
    mode = isDefault;
  }
  else
  {
    Constant::read_ints();
    Constant::read_floats();
  }
}

int Constant::read_ints()
{
  for (int i = 0; i < int_size; i++)
  {
    settingsFile.readStringUntil(':');   // Ignore writing up until the colon
    *ints[i] = settingsFile.parseInt();  // Save the next value into the location of the pointer
  }
  return 0;
}

int Constant::read_floats() {
    for (int i = 0; i < float_size; i++) {
        String dump = settingsFile.readStringUntil(':'); //Ignore writing up until the colon
        *floats[i] = settingsFile.parseFloat(); //Save the next value into the location of the pointer
    }
    return 0;
}

String Constant::get_values(){
    String output = "";
    for (int i = 0; i < int_size; i++) {
        output += *ints[i];
        output += ", ";
    }
    for (int i = 0; i < float_size; i++) {
        output += String(*floats[i]);
        output += ", ";
    }
    return output;
}