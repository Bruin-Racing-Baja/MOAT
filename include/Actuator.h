class Actuator {
    //Pins



    int position;
    bool currently_moving;

    int status;
    int report;
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
    Actuator();
    int init() {return status;}
    int odometry() {return status;}
    int action() {return status;}
    int report() {return report;}
};