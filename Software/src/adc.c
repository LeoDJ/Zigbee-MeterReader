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
    .burst      = NRF_SAADC_BURST_DISABLED,
    .pin_p      = SAADC_CH_PSELP_PSELP_VDDHDIV5,
    .pin_n      = NRF_SAADC_INPUT_DISABLED,
};

static nrf_saadc_value_t adcBuf[2][ADC_SAMPLE_NUM];

void adcCallback(nrf_drv_saadc_evt_t const * p_event) {
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE) {
        nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, 1);
    }

    NRF_LOG_INFO("ADC: %d", p_event->data.done.p_buffer[0]);
}

void adcDoSample() {
    nrf_drv_saadc_sample();
}

void adcInit() {

    nrf_drv_saadc_config_t saadc_config = {
        .low_power_mode = true,
        .resolution = NRF_SAADC_RESOLUTION_14BIT,
        .oversample = NRF_SAADC_OVERSAMPLE_DISABLED,
        .interrupt_priority = APP_IRQ_PRIORITY_LOW
    };

    nrf_drv_saadc_init(&saadc_config, adcCallback);
    nrf_drv_saadc_channel_init(0, &adc_vddh_cfg);
    nrf_drv_saadc_buffer_convert(adcBuf[0], ADC_SAMPLE_NUM);
    nrf_drv_saadc_buffer_convert(adcBuf[1], ADC_SAMPLE_NUM);
}