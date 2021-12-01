#include <RH_RF95.h>

class Radio {
    //PINS
    int CS_PIN;
    int RST_PIN;
    int INT_PIN;
    //SETTINGS
    double frequency;
    
    int RSSI; // Signal strength
    int dropped; // Packets dropped

    int status;
    
    RH_RF95 radio; //Radio object for use with library
    
    public:
        Radio(int CS, int RST, int INT, double freq);
        int init();
        int getStatus();
        bool checkConnection();
        int send(const char* data, int len);
};