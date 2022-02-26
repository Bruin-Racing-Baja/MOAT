#ifndef constant_h
#define constant_h

#include <SD.h>
#include <String>

struct Constant{
    void init(File settingsFile, int defaultValueMode = 0);
    int isDefault;

    // General
    int mode;
    bool home;
    bool wait_serial;
    
    // Diagnostic Mode
    int diagnostic_mode_shots;

    // Log
    int log_level;
    int save_interval;
    String log_name;

    // PID
    const float k_proportional_gain = .015; // gain of the p controller
    const float k_derivative_gain   = .0;//.005;    
    const float k_integral_gain     = .0001;
    const float k_exp_filt_alpha    = 0.5;   

    private:
    File settingsFile;

    int read_ints();
    int read_floats();
    int int_size = 2;
    int float_size = 1;
    int *ints[2] = {&mode, &desired_rpm};
    float *floats[3] = {&p};
};

#endif