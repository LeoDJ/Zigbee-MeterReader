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
float irSlowAvg = 0;
bool irCurrentlyTriggered = false;

uint32_t lastTrigger = 0;

void (*_irSensorPulseCallback)(uint32_t) = NULL;

void irTrigger() {
    uint32_t now = app_timer_cnt_get();

    uint32_t interval = app_timer_cnt_diff_compute(now, lastTrigger);

    uint32_t ms = 1000 * interval / APP_TIMER_TICKS(1000);
    uint32_t ticksPerHour = 3600000 / ms;

    if (_irSensorPulseCallback) {
        _irSensorPulseCallback(ticksPerHour);
    }

    lastTrigger = now;
}

void irDoCalculation() {
    // NRF_LOG_INFO("IR Ambient: %4d, Reflected: %4d", irAmbientVal, irReflectedVal);

    int16_t irVal = irReflectedVal - irAmbientVal;
    
    // initialize slow average, so it doesn't have to start from rock bottom
    if (irSlowAvg == 0) {
        irSlowAvg = irVal;
    }

    int16_t diff = irVal - irSlowAvg;

    // check if difference is over threshhold (positive or negative)
    if (diff > IR_SENSE_THRESHOLD || diff < -IR_SENSE_THRESHOLD) {
        // activate trigger only once
        if (!irCurrentlyTriggered) {
            irCurrentlyTriggered = true;
            LEDS_INVERT(BSP_LED_0_MASK);
            irTrigger();
        }
    }
    // reset the triggered state with a bit of a hysteresis
    else if (diff < IR_SENSE_THRESHOLD / 2 || diff > -IR_SENSE_THRESHOLD / 2) {
        if (irCurrentlyTriggered) {
            irCurrentlyTriggered = false;
            LEDS_INVERT(BSP_LED_0_MASK);
        }
    }

    // do slow averaging to accomodate to environment
    // only do averaging when the sensor is not currently triggered
    //   has the potential to block the automatic environment accomodation and triggering completely
    //   TODO: fix that behaviour
    if (!irCurrentlyTriggered) {
        irSlowAvg -= irSlowAvg / IR_SENSE_SLOW_AVG_SAMPLES;
        irSlowAvg += (float)irVal / IR_SENSE_SLOW_AVG_SAMPLES;
    }

    NRF_LOG_RAW_INFO("%d,%d,%d,%d\n", irAmbientVal, irReflectedVal, irVal, (int16_t)irSlowAvg);

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
    // LEDS_INVERT(BSP_LED_0_MASK);
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

void irSensorSetPulseCallback(void (*callback)(uint32_t)) {
    _irSensorPulseCallback = callback;
}