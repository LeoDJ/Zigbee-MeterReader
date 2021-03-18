#pragma once

#include <nrf_drv_saadc.h>

#define ADC_SAMPLE_NUM  1 

void adcInit();
void adcDoSample();