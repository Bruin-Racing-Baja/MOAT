class Cooler {
    //PINS
    int THERMO_PIN;

    int temperature;
    bool fanOn;

    int status;
    int report;
    /*
    Report structure:
    ABBBCD
    A: Status code (as normal)
    BBB: Temperature
    C: Fan status (0 = off, 1 = on)
    D: Other errors (specific case)
    */
    Cooler(int thermo_pin);
    int init() {return status;}
    int odometry() {return status;}
    int action() {return status;}
    int report() {return report;}
};