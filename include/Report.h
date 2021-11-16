class Report {
    int* actuator_report[4] = {0, 0, 0, 0};
    double battery_report;
    int* cooling_report;

    // char* strFromArray(int* array, int len);

    public:
        void a_odometry(int* odometry_results);
        void a_action(int action_status);
        void cooling(int* in);

        char* getReport(); //Returns fully compiled report
        //TODO: A DESTRUCTOR
};