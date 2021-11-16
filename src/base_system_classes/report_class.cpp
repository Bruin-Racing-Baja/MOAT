#include <Report.h>
#include <string>

void Report::a_odometry(int* odometry_results) {
    //Take in [status, position, velocity]
    for (int i=0; i<3; i++) {
        *(actuator_report + i) = &(odometry_results+i);
    }
}

void Report::a_action(int in) {
    //Take in status after action
    actuator_report[3] = in;
}

void Report::cooling(int* in) {
    cooling_report = in;
}

char* Report::getReport() {
    //Returns array of ints to be transmitted
    //ORDER: [a_status, a_pos, a_vel, a_action_status]
    return strFromArray(actuator_report, 4);
}

char* strFromArray(int* in, int len) {
    char* s_out;
    for (int i = 0; i < len; i++) {
        s_out += ('0' + in[i]);
    }
    return s_out;
}