class Report {
    int actuator_report;
    double battery_report;
    int cooling_report;

    public:
        int actuator(int* in);
        int battery(double voltage);
        int cooling_temp(double in);
        int cooling_fan(int in);

        int get_report(); //Gives fully compiled report
};