#include <Cooler.h>

Cooler::Cooler(int thermo_pin) {
    THERMO_PIN = thermo_pin;
}

int Cooler::init() {
    status = 0;
    return status;
}

double Cooler::odometry() {
    double temp = 2.389;
    return temp;
}