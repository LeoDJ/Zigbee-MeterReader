#pragma once

#include <nrf_drv_saadc.h>

#define ADC_SAMPLE_NUM  1 
// VDDH/5 / 2 => 0.6V * 10 = 6V full range -> 16383 / 6V = 2730.66
#define ADC_COUNTS_PER_VOLT 2731

void adcInit();
void adcDoSample();
uint32_t adcGetVcc(); // returns mV, blocking