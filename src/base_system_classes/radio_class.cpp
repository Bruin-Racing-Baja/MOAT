#include <RH_RF95.h>
#include <Radio.h>
#include <SPI.h>

#include <string>

Radio::Radio(int CS, int RST, int INT, double freq) : radio(CS, INT)
{
    CS_PIN = CS;
    RST_PIN = RST;
    INT_PIN = INT;
    frequency = freq;
    status = 1003;
    dropped = 0;
}

int Radio::init()
{

    // Manual reset
    digitalWrite(RST_PIN, LOW);
    delay(10);
    digitalWrite(RST_PIN, HIGH);
    delay(10);

    // Test connection
    while (!radio.init())
    {
        status = 1001;
        return status;
    }
    delay(500);
    if (!radio.setFrequency(frequency))
    {
        status = 1002;
        return status;
    }
    radio.setTxPower(23, false);

    status = 0;
    return status;
}

bool Radio::checkConnection()
{
    /*
    NOTE: This method stops the control loop and is not meant to be fast.
    Returns a boolean value.
    */
    std::string sending = "Hello There!";
    std::string confirmation = "Gerneral Kenobi";
    radio.send((uint8_t *)sending.c_str(), sending.length());
    radio.waitPacketSent();

    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    delay(10);
    if (radio.waitAvailableTimeout(1000))
    {
        if (radio.recv(buf, &len))
        {
            if ((char *)buf == confirmation.c_str())
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

int Radio::send(String in, int timeout = 0)
{
    /*
    This method takes a string and sends it over the radio module
    By default it will not wait or look for any response from the module
    If passed a timeout value, it will wait for a response from the module
    Please note however that this method will not return the response from the module, and only return a status code
    With no timeout the method will take little time, with a timeout it can take the length of the timeout in addition to any delays that occur
    */

    if (!timeout)
    {
        if (radio.send((uint8_t *)in.c_str(), in.length()))
        {
            status = 0;
            return status;
        }
        else
        {
            status = 1021;
            return status;
        }
    }
    else
    {
        // Sends message over radio and blocks until a reply is recieved or timeout
        if (radio.send((uint8_t *)in.c_str(), in.length()))
        {
            radio.waitPacketSent();

            // Initialize input buffer
            uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
            uint8_t len = sizeof(buf);

            delay(10);
            if (radio.waitAvailableTimeout(timeout))
            {
                // Means we have a reply
                if (radio.recv(buf, &len))
                {
                    // Figuring this out, but (char*)buf has the message back
                    RSSI = radio.lastRssi();
                }
                else
                {
                    // Recieve failed ¯\_(ツ)_/¯
                    status = 1031;
                    return status;
                }
            }
            else
            {
                // No reply detected, so timeout
                status = 1032;
                return status;
            }
        }
        else
        {
            status = 1021;
            return status;
        }
    }

    // radio.waitPacketSent(); // Wait for the transmission to complete

    // // Now wait for a reply
    // uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    // uint8_t len = sizeof(buf);

    // delay(10);
    // if (radio.waitAvailableTimeout(1000))
    // {
    //     // Should be a reply message for us now
    //     if (radio.recv(buf, &len))
    //     {   //Firgure out how to read reply from this
    //         // Serial.print("Got reply: ");
    //         // Serial.println((char*)buf);

    //         RSSI = radio.lastRssi();  //May be in non decimal form which could be an issue
    //         }
    //     else
    //     {
    //         //Recieve failed (for some reason)
    //         dropped++;
    //         status = 2;
    //         return status;
    //     }
    // }
    // else
    // {
    //     //No reply at all, timeout
    //     dropped++;
    //     status = 1;
    //     return status;
    // }
}

int Radio::waitReplySend(String in, String &out, int timeout = 0)
{
    /*
    Similar to the send method, but this method will wait for a reply from the radio module and update the out variable with the reply
    */
}

//-----------------Radio Getters--------------//
int Radio::getStatus()
{
    return status;
}