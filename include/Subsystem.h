class Subsystem {
    protected:
        int status;
        int report;
        std::byte error;
    public:
        int init() {return status;} //Runs startup code
        int odometry() {return status;} //Collects relevant sensor data
        int decision() {return status;} //Performs decision making based off of sensor data
        int action() {return status;} //Acts on decision made earlier
        int report() {return report;} //Generates and returns a report to be sent
    
};