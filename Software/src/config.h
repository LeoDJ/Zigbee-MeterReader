#pragma once

#define IR_LED_GPIO         45  // P1.13
#define IR_SENS_AIN         SAADC_CH_PSELP_PSELP_AnalogInput0
#define SAMPLING_FREQUENCY  10                  // [Hz] Sampling frequency of IR sensor
#define IR_LED_ON_TIME      5                   // [ms] On-time of the IR LED during reflective sampling
#define IR_SENSE_GAIN       NRF_SAADC_GAIN1_3     // Gain of the sensor ADC channel

#define IR_SENSE_SLOW_AVG_SAMPLES   100
#define IR_SENSE_THRESHOLD          100


// Helpers
#define SAMPLING_FREQUENCY_US   (1000000 / SAMPLING_FREQUENCY)