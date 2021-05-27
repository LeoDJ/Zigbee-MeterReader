#pragma once

#include <stdint.h>

void irSensorInit();

void irSensorAdcCallback(int16_t adcVal);

// this callback gets called for every pulse received and provides pulses per hour as the parameter
void irSensorSetPulseCallback(void (*callback)(uint32_t pulsesPerHour));

enum SampleStates {
    IR_Idle,
    IR_AmbientStarted,
    IR_AmbientDone,
    IR_ReflectiveStarted,
    IR_ReflectiveDone
};