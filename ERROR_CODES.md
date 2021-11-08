This is a collection of what each error code for each system means. When sending an error code, it should be incl

Firslty, there is the status code for each class which is explained by this.
STATUS CODE
    0: Nominal, after odometry -> action cycle, redy to report
    1: Error, look at error code
    2: Not initialized
    3: Odometry complete, no action
    4: Performing action

Then, each class has its own set of error codes, explained below. Error code groupings are technically arbitrary, but I would recommend following the general ideas as laid out in the actuator codes below. Basically, grouping the errors to a family of things that can go wrong, then going specifically into what went wrong

---------------[ Actuator ]---------------
POSITION OUT OF BOUNDS
    0001 - Encoder reported a position outside of bounds
    0002 - Outer Limit switch depressed
    0003 - Inner limit switch depressed
UNABLE TO CHANGE POSITION
    0011 - No change in encoder reading despite taking action

UNABLE TO READ ENCODER VALUE
    0021 - Unable to read value from encoder
    0022 - Unable to read stored encoder value

STORED ENCODER VALUE
    0031 - Unable to locate a stored encoder value
    0032 - Unable to store encoder value (perhaps out of storage?)

HOMING
    0041 - Unable to complete homing process

---------------[ Radio ]---------------
INITIALIZATION ERROR
    0001 - LoRa radio init failed (Teensy -/-> radio module)
    0002 - SetFrequency failed

SENT MESSAGE FAILURE
    0011 - Reply failed (but was given a response)
    0012 - No reply, timeout (Unable to connect to reciever)