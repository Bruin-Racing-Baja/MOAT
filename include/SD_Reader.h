#ifndef sd_h
#define sd_h

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define pin BUILTIN_SDCARD;

class Sd{
    public:
    Sd();
    int init();
    int write(String);
    int save();
    File getFileStream();

    private:
    int status;
    bool exposed_stream = false;
    File dataFile;

};

#endif