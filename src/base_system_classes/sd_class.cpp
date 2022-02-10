#include <Arduino.h>
#include <SD_Reader.h>
#include <SPI.h>
#include <SD.h>


Sd::Sd() {
    File dataFile;
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
}

int Sd::write(String i) {
    //Takes in a string and writes it to SD card
    dataFile = SD.open("data.txt", FILE_WRITE);
    if (dataFile) {
        dataFile.println(i);
    }
}

File Sd::getFileStream() {
    /*
    MAKE SURE YOU CLOSE THE FILE OR IT WONT SAVE
    Sd.save will close this file stream and save it
    This should only be used if you know what you are doing

    */
    exposed_stream = true;
    return dataFile;
}
