#include "adc.h"
#include <nrf_log.h>


// set input to VDDH and divide by 10
nrf_saadc_channel_config_t adc_vddh_cfg = {
    .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
    .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
    .gain       = NRF_SAADC_GAIN1_2,
    .reference  = NRF_SAADC_REFERENCE_INTERNAL,
    .acq_time   = NRF_SAADC_ACQTIME_10US,
    .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
    .burst      = NRF_SAADC_BURST_ENABLED,
    .pin_p      = SAADC_CH_PSELP_PSELP_VDDHDIV5,
    .pin_n      = NRF_SAADC_INPUT_DISABLED,
};

static nrf_saadc_value_t adcBuf[2][ADC_SAMPLE_NUM];

static nrf_saadc_value_t convertedSamples[1];

void adcCallback(nrf_drv_saadc_evt_t const * p_event) {
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE) {
        nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, 1);
        convertedSamples[0] = p_event->data.done.p_buffer[0];
    }
    
}

void adcDoSample() {
    nrf_drv_saadc_sample();
}

bool adcBlockUntilConversionComplete() {
    uint32_t timeout = 10000; // just guessed
    while (nrf_saadc_event_check(NRF_SAADC_EVENT_END) && timeout > 0) {
        timeout--;
    }
    return !!timeout; // true if successful and no timeout occured
}

uint32_t adcGetVcc() {
    adcDoSample();
    adcBlockUntilConversionComplete(); // just assume this works without a problem for now
    nrf_saadc_value_t adcVal = convertedSamples[0];
    uint32_t batVolt = ((uint32_t)adcVal) * 1000 / ADC_COUNTS_PER_VOLT;
    NRF_LOG_INFO("ADC: %d -> %d mV", adcVal, batVolt);
    return batVolt;
}

void adcInit() {

    nrf_drv_saadc_config_t saadc_config = {
        .low_power_mode = true,
        .resolution = NRF_SAADC_RESOLUTION_14BIT,
        .oversample = NRF_SAADC_OVERSAMPLE_8X,
        .interrupt_priority = APP_IRQ_PRIORITY_LOW
    };

    nrf_drv_saadc_init(&saadc_config, adcCallback);
    nrf_drv_saadc_channel_init(0, &adc_vddh_cfg);
    nrf_drv_saadc_buffer_convert(adcBuf[0], ADC_SAMPLE_NUM);
    nrf_drv_saadc_buffer_convert(adcBuf[1], ADC_SAMPLE_NUM);
}