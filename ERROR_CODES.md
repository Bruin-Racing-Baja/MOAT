This is a collection of what each error code for each system means. When sending an error code, it should be incl

Firslty, there is the status code for each class which is explained by this.
STATUS CODE
    0: Nominal, after odometry -> action cycle, redy to report
    1: Error, look at error code
    2: Not initialized
    3: Odometry complete, no action
    4: Performing action

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
POSITION OUT OF BOUNDS
    2001 - Encoder reported a position outside of bounds
    2002 - Outer Limit switch depressed
    2003 - Inner limit switch depressed
UNABLE TO CHANGE POSITION
    2011 - No change in encoder reading despite taking action

UNABLE TO READ ENCODER VALUE
    2021 - Unable to read value from encoder
    2022 - Unable to read stored encoder value

STORED ENCODER VALUE
    2031 - Unable to locate a stored encoder value
    2032 - Unable to store encoder value (perhaps out of storage?)

HOMING
    2041 - Homing Timeout

STARTUP
    2051 - Cannot initiate serial connection to ODRIVE
    2052 - run_state(0, 1, true, 0) failed