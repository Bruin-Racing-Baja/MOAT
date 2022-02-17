#include <Arduino.h>
#include <SD.h>
#include <SD_Reader.h>
#include <SPI.h>

Sd::Sd(File *log_file): m_log_file(*log_file)
{
}

int Sd::init()
{
    if (!SD.begin(BUILTIN_SDCARD))
    {
        m_status = 4001;
        return m_status;
    }
    else
    {
        m_status = 0;
        return m_status;
    }
    m_log_file = SD.open("data.txt", FILE_WRITE);
}

int Sd::write(String data)
{
    // Takes in a string and writes it to SD card
    if (m_log_file)
    {
        m_log_file.println(data);
    }
}