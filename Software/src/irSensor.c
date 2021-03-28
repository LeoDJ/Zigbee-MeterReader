#include "irSensor.h"

#include <bsp.h>
#include <app_timer.h>
#include <nrf_drv_clock.h>
#include <nrf_log.h>

#include "config.h"
#include "adc.h"

uint8_t sampleState = IR_Idle;

APP_TIMER_DEF(irSensorSampleTimer);
APP_TIMER_DEF(irSensorTriggerReflectiveSampleTimer);

int16_t irAmbientVal = 0, irReflectedVal = 0;

void irDoCalculation() {
    // NRF_LOG_INFO("IR Ambient: %4d, Reflected: %4d", irAmbientVal, irReflectedVal);
    NRF_LOG_RAW_INFO("%d,%d\n", irAmbientVal, irReflectedVal);

}

void irSensorAdcCallback(int16_t adcVal) {
    // NRF_LOG_INFO("State: %d, val: %d", sampleState, adcVal);
    switch (sampleState) {
        case IR_AmbientStarted:
            irAmbientVal = adcVal;
            nrf_gpio_pin_write(IR_LED_GPIO, 1);
            sampleState = IR_ReflectiveStarted;
            app_timer_start(irSensorTriggerReflectiveSampleTimer, APP_TIMER_TICKS(IR_LED_ON_TIME), NULL);
            break;
        case IR_ReflectiveDone:
            nrf_gpio_pin_write(IR_LED_GPIO, 0);
            irReflectedVal = adcVal;
            irDoCalculation();
            sampleState = IR_Idle;
            break;
    }
}

void irSensorTriggerReflectiveSample() {
    if (sampleState == IR_ReflectiveStarted) {
        if (adcTriggerSample()) { // trigger reflective conversion
            sampleState = IR_ReflectiveDone;
        } 
        else {
            // discard current sampling attempt completely, when conversion fails
            nrf_gpio_pin_write(IR_LED_GPIO, 0);
            sampleState = IR_Idle;
        }
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

    err_code = app_timer_create(&irSensorTriggerReflectiveSampleTimer, APP_TIMER_MODE_SINGLE_SHOT, irSensorTriggerReflectiveSample);
    APP_ERROR_CHECK(err_code);

    adcSetSensorCallback(irSensorAdcCallback);

    nrf_gpio_cfg_output(IR_LED_GPIO);
}