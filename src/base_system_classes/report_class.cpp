#include <Report.h>

int Report::actuator(int in) {
    return 0;
}

int Report::cooling(int in) {
    //IN format: ABBB | A = 1 if fan is on, 0 if off | BBB is temp
    return in;
}