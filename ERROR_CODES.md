Each class has its own set of error codes, explained below. Error code groupings are technically arbitrary, but I would recommend following the general ideas as laid out in the actuator codes below. Basically, grouping the errors to a family of things that can go wrong, then going specifically into what went wrong


<--><--><--><-->< Base Systems ><--><--><--><--><-->

---------------[ Odrive ]---------------
INITIALIZATION ERROR
    0001 - Odrive <--> Teensy connection timeout

--------------[ SD Card ]---------------
    4001 - SD Card not found / Failed to open
    4002 - SD Card unable to write to file
    4003 - SD Card unable to save file

<--><--><--><-->< Sub-Systems ><--><--><--><--><-->

---------------[ Actuator ]---------------
Startup
    1001 - Odrive communication timed out
    1002 - Homing timeout
    1003 - Failed encoder index search
