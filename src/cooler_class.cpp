class Cooling {
    int status = 2;
    //Measurement in this case is value from thermocoupler, so no array required
    double lastMeasurements;

    int init() {
        //Run setup procedures and return appropriate status code
        return 0;
    }

    double odometry() {
        //Read all relevant sensors, and return their values in a double
        //In this case, read the data from the thermocoupler then process and return a usable value

        return 0.202;
    }

    int action() {
        //Perform the action that the object is designed to perform
        //In this case, turn on the cooling fan
        return 0;
    }
};