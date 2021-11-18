class Cooler{
    //PINS
    int THERMO_PIN;

    int temperature;
    bool fanOn;
    /*
    Report structure:
    ABBBCD
    A: Status code (as normal)
    BBB: Temperature
    C: Fan status (0 = off, 1 = on)
    D: Other errors (specific case)
    */
    public:
        Cooler(int thermo_pin);
        int init();
};