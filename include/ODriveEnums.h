#ifndef ODRIVE_ENUMS_H
#define ODRIVE_ENUMS_H
#include <string>

#define AXIS_STATE_UNDEFINED 0
#define AXIS_STATE_IDLE 1
#define AXIS_STATE_STARTUP_SEQUENCE 2
#define AXIS_STATE_FULL_CALIBRATION_SEQUENCE 3
#define AXIS_STATE_MOTOR_CALIBRATION 4
#define AXIS_STATE_ENCODER_INDEX_SEARCH 6
#define AXIS_STATE_ENCODER_OFFSET_CALIBRATION 7
#define AXIS_STATE_CLOSED_LOOP_CONTROL 8
#define AXIS_STATE_LOCKIN_SPIN 9
#define AXIS_STATE_ENCODER_DIR_FIND 10
#define AXIS_STATE_HOMING 11
#define AXIS_STATE_ENCODER_HALL_POLARITY_CALIBRATION 12
#define AXIS_STATE_ENCODER_HALL_PHASE_CALIBRATION 13

String system_errors[64];
String can_errors[64];
String axis_errors[64];
String motor_errors[64];
String controller_errors[64];
String encoder_errors[64];
String sensorless_estimator_errors[64];

void populate_errors()
{
    system_errors[0] = "ODRIVE_ERROR_NONE";
    system_errors[1] = "ODRIVE_ERROR_CONTROL_ITERATION_MISSED";
    system_errors[2] = "ODRIVE_ERROR_DC_BUS_UNDER_VOLTAGE";
    system_errors[3] = "ODRIVE_ERROR_DC_BUS_OVER_VOLTAGE";
    system_errors[4] = "ODRIVE_ERROR_DC_BUS_OVER_REGEN_CURRENT";
    system_errors[5] = "ODRIVE_ERROR_DC_BUS_OVER_CURRENT";
    system_errors[6] = "ODRIVE_ERROR_BRAKE_DEADTIME_VIOLATION";
    system_errors[7] = "ODRIVE_ERROR_BRAKE_DUTY_CYCLE_NAN";
    system_errors[8] = "ODRIVE_ERROR_INVALID_BRAKE_RESISTANCE";
    system_errors[9] = "NULL";

    can_errors[0] = "CAN_ERROR_NONE";
    can_errors[1] = "CAN_ERROR_DUPLICATE_CAN_IDS";
    can_errors[2] = "NULL";

    axis_errors[0] = "AXIS_ERROR_NONE";
    axis_errors[1] = "AXIS_ERROR_INVALID_STATE";
    axis_errors[12] = "AXIS_ERROR_WATCHDOG_TIMER_EXPIRED";
    axis_errors[13] = "AXIS_ERROR_MIN_ENDSTOP_PRESSED";
    axis_errors[14] = "AXIS_ERROR_MAX_ENDSTOP_PRESSED";
    axis_errors[15] = "AXIS_ERROR_ESTOP_REQUESTED";
    axis_errors[18] = "AXIS_ERROR_HOMING_WITHOUT_ENDSTOP";
    axis_errors[19] = "AXIS_ERROR_OVER_TEMP";
    axis_errors[20] = "AXIS_ERROR_UNKNOWN_POSITION";
    axis_errors[21] = "NULL";

    motor_errors[0] = "MOTOR_ERROR_NONE";
    motor_errors[1] = "MOTOR_ERROR_PHASE_RESISTANCE_OUT_OF_RANGE";
    motor_errors[2] = "MOTOR_ERROR_PHASE_INDUCTANCE_OUT_OF_RANGE";
    motor_errors[4] = "MOTOR_ERROR_DRV_FAULT";
    motor_errors[5] = "MOTOR_ERROR_CONTROL_DEADLINE_MISSED";
    motor_errors[8] = "MOTOR_ERROR_MODULATION_MAGNITUDE";
    motor_errors[11] = "MOTOR_ERROR_CURRENT_SENSE_SATURATION";
    motor_errors[13] = "MOTOR_ERROR_CURRENT_LIMIT_VIOLATION";
    motor_errors[17] = "MOTOR_ERROR_MODULATION_IS_NAN";
    motor_errors[18] = "MOTOR_ERROR_MOTOR_THERMISTOR_OVER_TEMP";
    motor_errors[19] = "MOTOR_ERROR_FET_THERMISTOR_OVER_TEMP";
    motor_errors[20] = "MOTOR_ERROR_TIMER_UPDATE_MISSED";
    motor_errors[21] = "MOTOR_ERROR_CURRENT_MEASUREMENT_UNAVAILABLE";
    motor_errors[22] = "MOTOR_ERROR_CONTROLLER_FAILED";
    motor_errors[23] = "MOTOR_ERROR_I_BUS_OUT_OF_RANGE";
    motor_errors[24] = "MOTOR_ERROR_BRAKE_RESISTOR_DISARMED";
    motor_errors[25] = "MOTOR_ERROR_SYSTEM_LEVEL";
    motor_errors[26] = "MOTOR_ERROR_BAD_TIMING";
    motor_errors[27] = "MOTOR_ERROR_UNKNOWN_PHASE_ESTIMATE";
    motor_errors[28] = "MOTOR_ERROR_UNKNOWN_PHASE_VEL";
    motor_errors[29] = "MOTOR_ERROR_UNKNOWN_TORQUE";
    motor_errors[30] = "MOTOR_ERROR_UNKNOWN_CURRENT_COMMAND";
    motor_errors[31] = "MOTOR_ERROR_UNKNOWN_CURRENT_MEASUREMENT";
    motor_errors[32] = "MOTOR_ERROR_UNKNOWN_VBUS_VOLTAGE";
    motor_errors[33] = "MOTOR_ERROR_UNKNOWN_VOLTAGE_COMMAND";
    motor_errors[34] = "MOTOR_ERROR_UNKNOWN_GAINS";
    motor_errors[35] = "MOTOR_ERROR_CONTROLLER_INITIALIZING";
    motor_errors[36] = "MOTOR_ERROR_UNBALANCED_PHASES";
    motor_errors[37] = "NULL";

    encoder_errors[0] = "ENCODER_ERROR_NONE";
    encoder_errors[1] = "ENCODER_ERROR_UNSTABLE_GAIN";
    encoder_errors[2] = "ENCODER_ERROR_CPR_POLEPAIRS_MISMATCH";
    encoder_errors[3] = "ENCODER_ERROR_NO_RESPONSE";
    encoder_errors[4] = "ENCODER_ERROR_UNSUPPORTED_ENCODER_MODE";
    encoder_errors[5] = "ENCODER_ERROR_ILLEGAL_HALL_STATE";
    encoder_errors[6] = "ENCODER_ERROR_INDEX_NOT_FOUND_YET";
    encoder_errors[7] = "ENCODER_ERROR_ABS_SPI_TIMEOUT";
    encoder_errors[8] = "ENCODER_ERROR_ABS_SPI_COM_FAIL";
    encoder_errors[9] = "ENCODER_ERROR_ABS_SPI_NOT_READY";
    encoder_errors[10] = "ENCODER_ERROR_HALL_NOT_CALIBRATED_YET";
    encoder_errors[11] = "NULL";

    controller_errors[0] = "CONTROLLER_ERROR_NONE";
    controller_errors[1] = "CONTROLLER_ERROR_OVERSPEED";
    controller_errors[2] = "CONTROLLER_ERROR_INVALID_INPUT_MODE";
    controller_errors[3] = "CONTROLLER_ERROR_UNSTABLE_GAIN";
    controller_errors[4] = "CONTROLLER_ERROR_INVALID_MIRROR_AXIS";
    controller_errors[5] = "CONTROLLER_ERROR_INVALID_LOAD_ENCODER";
    controller_errors[6] = "CONTROLLER_ERROR_INVALID_ESTIMATE";
    controller_errors[7] = "CONTROLLER_ERROR_INVALID_CIRCULAR_RANGE";
    controller_errors[8] = "CONTROLLER_ERROR_SPINOUT_DETECTED";
    controller_errors[9] = "NULL";

    sensorless_estimator_errors[0] = "SENSORLESS_ESTIMATOR_ERROR_NONE";
    sensorless_estimator_errors[1] = "SENSORLESS_ESTIMATOR_ERROR_UNSTABLE_GAIN";
    sensorless_estimator_errors[2] = "SENSORLESS_ESTIMATOR_ERROR_UNKNOWN_CURRENT_MEASUREMENT";
    sensorless_estimator_errors[3] = "NULL";
}

#endif
