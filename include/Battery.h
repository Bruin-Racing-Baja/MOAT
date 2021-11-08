class Battery {
    double voltage;
    double lastVoltage; //May make this an array to store past 5 voltages to see discharge rate
    public:
        double measureVoltage() {return voltage;}
};