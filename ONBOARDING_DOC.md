Hello! The goal of this document is to get any member of the Baja team up to speed on how they can aid in the devlopment of the code, as well as provide guidelines for maintaining the code in the coming years.
I will go over some of the principles I have tried to stick to while writing the code. These should allow for the code to last a long time and avoid constant rewrites, so any changes to the code should adhere to these standards.

Github:
I know most people are MechE, but even basic use of Github features can make maintaining the code much easier.
    -Larger scale projects should have their own branch, and then be merged back into the main branch. 
    -Branches should also be used whenever someone else is working on the code simulteaneously to prevent interference between the collaborators
    -When branches are merged back into the main branch, it allows the opportunity for a peer code review
        NOTE: While code reviews won't be enforced by the repo, it is still beenficial to have someone do a once-over if possible

Layout of code:
    The code will exist across seperate files, with the most important being main, the class files, and the respective class header files.
    The bulk of the implementation for each system will be achieved in the classes, while the main file will call high-level methods provided by the classes
        -This makes the higher-level code easier to read, vital if we plan to have different members collaborate on this code
    Each "subsystem" will have its own class. Even if we only intend on creating one object from the class, it is useful for code readability and organization to have a class implemented

Main layout:
    --Includes--
        Include all the header files for the system classes as well as other libraries and dependencies
    --Settings--
        Define constants that we may want to change here, so that they can be easily viewed and changed in one place
        Also define pins here
        Follow the pattern for the comments there to organize the statements by subsystem
        DO NOT randomly assign them in the class files, as it will be a headache to dig through if we want to change something on the fly
            EX: If there is a constant that is VERY unlikely to change (like the max buffer size of a board) it is fine to place in the class file, but if we potentially would want to change it put it here
    --Initialize--
        Initializes each subsystem using the methods provided by each class
        Order here isn't hard set and can be changed, but there are some reasons for having the order set right now
    --Main Loop--
        Currently don't know how this will shake out, but when finished this doc ~should~ get updated
        Currently the idea is to put the main control loop on a timer so it will always trigger at the same time, making the control code more reliable

Error codes:
    Error codes will allow us to debug any issues on the fly. They will direct us to which system / sensor is experiencing problems and may provide some info as to how to fix them. Keep error codes in mind when adding any new features as it will help in the longer term to maintain the code.
    The error code document provides the key to the different codes, and new codes that are created should be put there
    Most error codes will be sent over the radio so we can see them, which will be added to the signal to be sent

Message Protocol:
    This is kinda a seperate beast, and has its own document for itself
    Basically, we have to create a standard to communicate between the radios that works for us
    For now I *think* it should all be handled by either a function or class, but it is not fully implemented. Hopefully this doc will get updated for how it works

Comments:
    Ensure you comment your code effectively and keep it well organized
    This will help to keep the code much easier to work on in the future, so please try and explain your code in the code

Closing thoughts:
    If you have any issues or questions slack me @Getty :)

CURRENT PROJECTS:
    Radio communication //  Getty
    Control code timing //  Getty
    Actuator class      //  Drew