#pragma once

#include <stdint.h>

#define ADC_SAMPLE_NUM  1 // number of active channels

// VDDH/5 / 2 => 0.6V * 10 = 6V full range -> 16383 / 6V = 2730.66 | 4095 / 6V = 682.5
#define ADC_COUNTS_PER_VOLT 682

void adcInit();
uint32_t adcGetVcc(); // returns mV, blocking