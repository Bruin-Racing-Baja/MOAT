The encoding of information to be passed from module to module must be encoded in an unsigned integer to be sent.
However, the needs of the message can change, as well as the length of these messages.
Considerations:
    Some data will have different ranges, and thus will have differing lengths of information
    Some systems / messages will not need to report everything all the time EX: Nominal readings
    Some precision in the data will be lost, as decimals will be truncated to integers. If this loss is unacceptable something else may be figured out
    The current system only allows for data points of length 99
Header:
    Contains information about which systems are rporting as well as the lengths and type of their report
    Each system has 3 digits in the header, the first being the data type and the other 2 give the length of the payload
    Key:
        0 - System is nominal / no report
        First digit:
            1 - Error code
                Error code is of known length 4, so the other 2 digits are not used
            2 - Data point
            3 - Both
                Followed by a length for the data point. The package will have the error code followed by the data point
Payload:
    In the same order as they occur in the header
    Data will be given consecutively, as the length of each entry will be known

System order:
    Radio
    Actuator
EX
000000 --> Radio and actuator are both nominal
000100 --> Radio nominal, actuator is reporting an error code with length 4
    NOTE: 000178 Will still be read the same, as its assumed the error code will only have a length of 4
000234 --> Radio nominal, actuator is reporting a data point with length of 34
327000 --> Actuator is nominal, radio is reporting an error code and a 27 length data point (the payload will have the error code followed immediately by the data point)

-----Research-----
After further research, the message should probably be encoded in binary instead of some other type of integer as that doesnt make much sense
