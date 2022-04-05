Each class has its own set of error codes, explained below. Error code groupings are technically arbitrary, but I would recommend following the general ideas as laid out in the actuator codes below. Basically, grouping the errors to a family of things that can go wrong, then going specifically into what went wrong

NOTE: For the impending drive day some errors may be mapped from 0-255 so i dont have to figure out a protocol yet, but we will see

--------------[ SD Card ]---------------
    4001 - SD Card not found / Failed to open
    4002 - SD Card unable to write to file
    4003 - SD Card unable to save file

Then, each class has its own set of error codes, explained below. Error code groupings are technically arbitrary, but I would recommend following the general ideas as laid out in the actuator codes below. Basically, grouping the errors to a family of things that can go wrong, then going specifically into what went wrong
<--><--><--><-->< Base Systems ><--><--><--><--><-->
---------------[ Odrive ]---------------
INITIALIZATION ERROR
    0001 - Odrive <--> Teensy connection timeout
---------------[ Radio ]---------------
INITIALIZATION ERROR
    1001 - LoRa radio init failed (Teensy -/-> radio module)
    1002 - SetFrequency failed
    1003 - Not Initiated yet

SENT MESSAGE FAILURE
    1011 - Reply failed (but was given a response)
    1012 - No reply, timeout (Unable to connect to reciever)

SENDING FAILURE
    1021 - radio.send failed

WAIT FOR REPLY SENDING FAILURE
    1031 - Recieve detected but failed (idk)
    1032 - Timeout

<--><--><--><-->< Sub-Systems ><--><--><--><--><-->
---------------[ Actuator ]---------------
Startup
    0001 - Odrive communication timed out
    0002 - Homing timeout

Control Function
    NOTE: Because of the way this is, some values may be nonsensical as they are not set or ever initialized
    These values will be marked by an X below
    0003 - Control function timing not correct
        <0XXXX00>
    0004 - Motor spinning too low, already shifted out
        <000XXX00>
    0005 - Motor spinning too low, shifting out
        <000XXX00>
    0006 - Error set to 0 as shifted all the way out
 ** 0007 - Error set to 0 as shifted all the way in

New Control Function Status Codes
    0 - Nominal, shifting in
    1 - Engine speed less than engage, shifting to right before engage

