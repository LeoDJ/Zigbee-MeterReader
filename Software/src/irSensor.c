#include "irSensor.h"

#include <bsp.h>
#include <app_timer.h>
#include <nrf_drv_clock.h>
#include <nrf_log.h>

#include "config.h"
#include "adc.h"

uint8_t sampleState = IR_Idle;

APP_TIMER_DEF(irSensorSampleTimer);

int16_t irAmbientVal = 0, irReflectedVal = 0;

void irDoCalculation() {
    NRF_LOG_INFO("IR Ambient: %4d, Reflected: %4d", irAmbientVal, irReflectedVal);
}

// TODO: currently the second sample trigger doesn't work for some reason, enough for today tho

void irSensorAdcCallback(int16_t adcVal) {
    NRF_LOG_INFO("State: %d, val: %d", sampleState, adcVal);
    switch (sampleState) {
        case IR_AmbientStarted:
            irAmbientVal = adcVal;
            // TODO: enable IR LED GPIO
            // TODO: maybe delay?
            if (adcTriggerSample()) { // trigger reflective conversion
                sampleState = IR_ReflectiveStarted;
            } 
            else {
                // discard current sampling attempt completely
                // TODO: disable IR LED GPIO
                sampleState = IR_Idle;
            }
            break;
        case IR_ReflectiveStarted:
            // TODO: disable IR LED GPIO
            irReflectedVal = adcVal;
            irDoCalculation();
            sampleState = IR_Idle;
            break;
    }
}

uint32_t tmpCounter = 0;

void irSensorSample() {
    LEDS_INVERT(BSP_LED_0_MASK);
    bool success = adcTriggerSample(); // will return false, if battery measurement is currently in progress
    if (success) {
        sampleState = IR_AmbientStarted;
    }
}

void irSensorInit() {
    ret_code_t err_code;
    err_code = nrf_drv_clock_init();                        //Initialize the clock source specified in the nrf_drv_config.h file, i.e. the CLOCK_CONFIG_LF_SRC constant
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);

    // create app timer
    err_code = app_timer_create(&irSensorSampleTimer, APP_TIMER_MODE_REPEATED, irSensorSample);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(irSensorSampleTimer, APP_TIMER_TICKS(SAMPLING_FREQUENCY_US / 1000), NULL);
    APP_ERROR_CHECK(err_code);

    adcSetSensorCallback(irSensorAdcCallback);
}