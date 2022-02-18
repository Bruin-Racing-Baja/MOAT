#include <Constant.h>
#include <SD.h>

Constant::Constant(File settingsFile_i, int defaultValueMode = 0){
    /*
    Passed settings file to read from
    If settings file cant be opened, passing an int will set the default value mode, where the mode is the passed int
    and all the other values will be set to default values (Can pass a nullptr to not use a settings file)
    */
    settingsFile = settingsFile_i;
    isDefault = defaultValueMode;
}

int Constant::init() {
    if (isDefault) {
        //Set mode to passed value, leave all others as defaults
        mode = isDefault;
        return 0;
    }
    else {
        Constant::read_ints();
        Constant::read_floats();
        return 0;
    }
    
}

int Constant::read_ints() {
    for (int i = 0; i < int_size; i++) {
        String dump = settingsFile.readStringUntil(':'); //Ignore writing up until the colon
        *ints[i] = settingsFile.parseInt(); //Save the next value into the location of the pointer
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