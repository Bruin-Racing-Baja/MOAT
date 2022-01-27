#include <RH_RF95.h>

#include <string>

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
    
    int encode(String in);
    
    public:
        Radio(int CS, int RST, int INT, double freq);
        int init(); //Initializes vital stuffs
        int getStatus();
        bool checkConnection();
        int send(String in, int timeout = 0);
};