#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ADC_SAMPLE_NUM  1 // number of active channels

// VDDH/5 / 2 => 0.6V * 10 = 6V full range -> 16383 / 6V = 2730.66 | 4095 / 6V = 682.5
#define ADC_COUNTS_PER_VOLT 682

void adcInit();
bool adcTriggerSample();
void adcSetSensorCallback(void (*callback)(int16_t));
void adcTriggerVccReading(); // returns mV, blocking
void adcSetVccCallback(void (*callback)(uint32_t));

enum AdcSamplingState {
    ADC_SampleSensor,
    ADC_SetupSampleVcc,
    ADC_SampleVcc
};