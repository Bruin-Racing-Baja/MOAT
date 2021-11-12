#include <Cooler.h>

Cooler::Cooler(int thermo_pin) {
    THERMO_PIN = thermo_pin;
}

int Cooler::init() {
    status = 0;
    return status;
}