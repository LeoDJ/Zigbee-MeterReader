#pragma once

#include <stdint.h>

void irSensorInit();
void irSensorAdcCallback(int16_t adcVal);

enum SampleStates {
    IR_Idle,
    IR_AmbientStarted,
    IR_AmbientDone,
    IR_ReflectiveStarted,
    IR_ReflectiveDone
};