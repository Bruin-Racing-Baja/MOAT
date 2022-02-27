#ifndef constant_h
#define constant_h

#include <SD.h>

struct Constant{
    void init(File settingsFile, int defaultValueMode = 0);
    int mode;
    int desired_rpm;
    int log_num;

  float p;

  int isDefault;
    File settingsFile;

    int read_ints();
    int read_floats();
    int int_size = 2;
    int float_size = 1;
    int *ints[3] = {&mode, &desired_rpm, &log_num};
    float *floats[1] = {&p};
};

#endif