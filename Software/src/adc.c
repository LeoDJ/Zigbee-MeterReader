#include "adc.h"
#include <nrf_log.h>
#include <nrfx_saadc.h>


// set input to VDDH and divide by 10
nrf_saadc_channel_config_t adcChCfg_vddh = {
    .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
    .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
    .gain       = NRF_SAADC_GAIN1_2,
    .reference  = NRF_SAADC_REFERENCE_INTERNAL,
    .acq_time   = NRF_SAADC_ACQTIME_40US,
    .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
    .burst      = NRF_SAADC_BURST_ENABLED,
    .pin_p      = SAADC_CH_PSELP_PSELP_VDDHDIV5,
    .pin_n      = NRF_SAADC_INPUT_DISABLED,
};

// nrfx_saadc_config_t adcCfg_vddh = {
//     .low_power_mode = true,
//     .resolution = NRF_SAADC_RESOLUTION_14BIT,
//     .oversample = NRF_SAADC_OVERSAMPLE_8X,
//     .interrupt_priority = APP_IRQ_PRIORITY_LOW
// };

nrf_saadc_channel_config_t adcChCfg_sensor = {
    .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
    .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
    .gain       = NRF_SAADC_GAIN1_6,
    .reference  = NRF_SAADC_REFERENCE_INTERNAL,
    .acq_time   = NRF_SAADC_ACQTIME_10US,
    .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
    .burst      = NRF_SAADC_BURST_ENABLED,
    .pin_p      = SAADC_CH_PSELP_PSELP_AnalogInput0,
    .pin_n      = NRF_SAADC_INPUT_DISABLED,
};

nrfx_saadc_config_t adcCfg_sensor = {
    .low_power_mode = true,
    .resolution = NRF_SAADC_RESOLUTION_12BIT,
    .oversample = NRF_SAADC_OVERSAMPLE_DISABLED,
    .interrupt_priority = APP_IRQ_PRIORITY_LOW
};

static nrf_saadc_value_t adcBuf[2][ADC_SAMPLE_NUM];

static nrf_saadc_value_t convertedSamples[1];

void adcCallback(nrfx_saadc_evt_t const * p_event) {
    if (p_event->type == NRFX_SAADC_EVT_DONE) {
        nrfx_saadc_buffer_convert(p_event->data.done.p_buffer, ADC_SAMPLE_NUM);
        convertedSamples[0] = p_event->data.done.p_buffer[0];
        NRF_LOG_INFO("Called adc callback");
    }
    
}

uint32_t adcGetVcc() {
    nrfx_saadc_channel_init(1, &adcChCfg_vddh);

    nrf_saadc_value_t adcVal;
    nrfx_saadc_sample_convert(1, &adcVal); // blocking, TODO: check if other conversion is running
    uint32_t batVolt = ((uint32_t)adcVal) * 1000 / ADC_COUNTS_PER_VOLT;
    NRF_LOG_INFO("ADC: %d -> %d mV", adcVal, batVolt);

    nrfx_saadc_channel_uninit(1);

    return batVolt;
}

void adcInit() {
    nrfx_saadc_init(&adcCfg_sensor, adcCallback);
    nrfx_saadc_channel_init(0, &adcChCfg_sensor);
    nrfx_saadc_channel_init(1, &adcChCfg_vddh);
    // nrfx_saadc_buffer_convert(adcBuf[0], ADC_SAMPLE_NUM); // currently messes with vcc sampling, TODO: find out why
    // nrfx_saadc_buffer_convert(adcBuf[1], ADC_SAMPLE_NUM);
}