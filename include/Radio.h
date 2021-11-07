#include <RH_RF95.h>

class Radio {
    int CS_PIN;
    int RST_PIN;
    int INT_PIN;
    int status_code;
    double frequency;
    RH_RF95 radio; //Radio object for use with library
    Radio(int CS, int RST, int INT, double freq);
    int init() {return status_code;}
};