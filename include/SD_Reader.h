#ifndef SD_H
#define SD_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

class Sd
{
public:
  Sd(File* log_file);
  int init();
  int write(String data);
  int save();
  File* get_file_stream();

private:
  int m_status;
  bool m_exposed_stream = false;
  File m_log_file;
};

#endif