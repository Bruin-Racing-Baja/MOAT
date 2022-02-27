This is the main loop code for the BruinRacing Baja Teensy microcontroller.

Documentation for the goals and implementation of this system will be detailed here for ease of access and to ensure anyone with the code can actually find it as it will be bundled together.
DISCLAIMER: Im typing this with no spellcheck and im too lazy to make a doc so dont mind the spelling

Structure:
Each subsystem will have its own object (actuator, cooling, etc.) which will have a standard set of methods and members (detailed later) for the sake of readability. This will also allow the program to scale up to include other subsytems such as brakes, driver interface, etc. New objects should follow the same general structure as shown in the other classes, but implementation of each overall method will depend on the individual subsystem. For example, both the cooling and actuator have an odometry method which behave very similarly, even though the actual way they measure the data varies.

Initialization:
Will go through basic systems first (Battery, radio, data logging), then will begin going through each subsystems init function to start up that system. Basic systems will allow for logging and potentially transmitting any critical errors that may occur. (Error codes will be explored in depth later on, but THEY ARE IMPORTANT so dont ignore them) From there, the program will enter the main operating cycle

Basis of cycle:
Odometry -> Decision -> Action -> Report -> Odometry -> ...
This may change, but the idea is to have each sensor take measurements simultaneously, decide how to act, and take action all at the same time. After that, data from Odometry and Decision will be reported, since the result of the action hasnt been measured yet. This makes the report more complex as it really reports the odometry and decision but does not know the result of the action. This is necessary however to keep the amount of time between measurement and action at a minimum. 
    This then leads to the fact that perhaps each system should complete a cycle before moving on to the next cycle. Given enough time this will hopefully be tested to see if there is any significant change, but it could so it is mentioned here

Structure of each class:
    Base Systems are systems that do not follow the normal cycle of events as laid out above. For example, the battery is an important part of the system that should be monitored but doesn't realy need to adhrere to the odometry -> action cycle, so it can be it's own class. These systems should usually have a report and init method, but because each one can vary there is no strict rule for this.

    Subsystems HAVE TO inherit from the base class. This ensures that systems have the same basic cycle behaviour. The class can also include other members specifically needed for that subsystems implementation, and these should follow good public / private practice (look it up or take CS31/32, or dont i'm a MechE you are probably MechE so it probably doesnt matter anyways). Try to keep to this for the objects to end up being consistent, allowing for easier long-term maintenence so we don't have to re-write this every year or have it turn into some mystical black-box with noone who understands it.

Error codes
    These can range from useless to life-saving depending on how much effort we put into implementing them. If executed well, we would be able to pinpoint errors quickly and save ourselves a lot of headaches in the future.
    Each system should have a 1 bit status code along with an 8-bit error code as dictated by the inheritance. The status code follows this basic structure
    STATUS CODE
    0: Nominal, after odometry -> action cycle, redy to report
    1: Error, look at error code
    2: Not initialized
    3: Odometry complete, no action
    4: Performing action

    In addition, there will be an error code, stored as an int. What each code means should be determined by whoever is creating the subsystem class, and should be included in that class header file. I may also make an error code document for easier reference where they could be found.