#include <Actuator.h>

Actuator::Actuator() {
    status = 0;
}

int Actuator::init() {
    status = 0;
    return status;
}

int* Actuator::odometry() {
    int return_code[4] = {0, 0, 0, 0};
    return return_code;
}