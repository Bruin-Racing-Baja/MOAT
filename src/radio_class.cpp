#include <SPI.h>
#include <RH_RF95.h>
#include <Radio.h>

Radio::Radio(int CS, int RST, int INT, double freq): radio(CS, INT){
    CS_PIN = CS;
    RST_PIN = RST;
    INT_PIN = INT;
    frequency = freq;
    status = 0;
    dropped = 0;
}

int Radio::init() {
    //Return 0 for nominal, 1 if radio init fails, 2 if setFrequency fails

    //Manual reset
    digitalWrite(RST_PIN, LOW);
    delay(10);
    digitalWrite(RST_PIN, HIGH);
    delay(10);

    //Test connection
    while (!radio.init()) {
        Serial.println("LoRa radio init failed");
        status = 1;
        return status;
    }
    delay(500);
    if (!radio.setFrequency(frequency)) {
        Serial.println("setFrequency failed");
        status = 2;
        return status;
    }
    radio.setTxPower(23, false);

    status = 0;
    return status;
}

int Radio::send(char* data, int len) {
    /*
    0: Success with reply
    1: Data sent but no reply (timeout)
    2: Data sent but reply failed (?)
    */
    
    char radiopacket[20] = "Hello World #      ";
    
    radio.send((uint8_t *)radiopacket, 20);

    radio.waitPacketSent(); // Wait for the transmission to complete

    // Now wait for a reply
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    delay(10);
    if (radio.waitAvailableTimeout(1000))
    { 
        // Should be a reply message for us now   
        if (radio.recv(buf, &len))
        {   //Firgure out how to read reply from this
            // Serial.print("Got reply: ");
            // Serial.println((char*)buf);

            RSSI = radio.lastRssi();  //May be in non decimal form which could be an issue   
            }
        else
        {
            //Recieve failed (for some reason)
            dropped++;
            status = 2;
            return status;
        }
    }
    else
    {
        //No reply at all, timeout
        dropped++;
        status = 1;
        return status;
    }
}