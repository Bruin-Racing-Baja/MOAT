#include <SPI.h>
#include <RH_RF95.h>
#include <Radio.h>

Radio::Radio(int CS, int RST, int INT, double freq): radio(CS, INT) {
    CS_PIN = CS;
    RST_PIN = RST;
    INT_PIN = INT;
    frequency = freq;
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
        return 1;
    }
    delay(500);
    if (!radio.setFrequency(frequency)) {
        Serial.println("setFrequency failed");
        return 2;
    }
    radio.setTxPower(23, false);

    return 0;
}