class Actuator {
    //Pins


    int status;
    int position;
    bool currently_moving;
    /*
    Report Structure:
    ABBBCCCDDDEF
    A: Status code
    BBB: Position
    CCC: Motor speed
    DDD: Wheel speed
    E: Currently moving
    F: Other error (specific)
    */
    public:
        Actuator();
        int init();
        int * odometry();
        int action();
        
};