#include <Arduino.h>
#include <SD_Reader.h>
#include <SPI.h>
#include <SD.h>


Sd::Sd(File* log_i) {
    logFile = *log_i;
}

int Sd::init() {
    if (!SD.begin(BUILTIN_SDCARD)) {
        status = 4001;
        return status;
    }
    else {
        status = 0;
        return status;
    }
    logFile = SD.open("data.txt", FILE_WRITE);
}

int Sd::write(String i) {
    //Takes in a string and writes it to SD card
    if (logFile) {
        logFile.println(i);
    }
}