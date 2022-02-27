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
    String log_name = "log.txt";

    // Actuator
    const static int k_actuator_cycle_period = 10;     // ms

    // PID
    float k_proportional_gain = .015; // gain of the p controller
    float k_derivative_gain   = .0;//.005;    
    float k_integral_gain     = .0001;
    float k_exp_filt_alpha    = 0.5;   

    // Dynamic target RPM
    int k_desired_rpm = 2250; // rpm
    float k_rpm_target_multiplier = 1.5; 

    // Cooling
    static const double k_temp_threshold = 100; // C
    static const int k_cooling_rpm = 500; //rpm
    static const int k_cooling_cycle_period = 10; // ms


    private:
    File settingsFile;

    int read_ints();
    int read_floats();
    int int_size = 2;
    int float_size = 1;
    int *ints[2] = {&mode, &k_desired_rpm};
    float *floats[3] = {&k_proportional_gain, &k_derivative_gain, &k_integral_gain};
};

#endif