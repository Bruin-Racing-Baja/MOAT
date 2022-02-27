#ifndef constant_h
#define constant_h

#include <SD.h>

struct Constant
{
  void init(File settingsFile, int defaultValueMode = 0);
  int mode;
  int desired_rpm;

  float p;

  int isDefault;

private:
  File settingsFile;

  int read_ints();
  int read_floats();
  int int_size = 2;
  int float_size = 1;
  int* ints[2] = { &mode, &desired_rpm };
  float* floats[3] = { &p };
};

#endif