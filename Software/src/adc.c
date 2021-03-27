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
    .burst      = NRF_SAADC_BURST_DISABLED,
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
    .burst      = NRF_SAADC_BURST_DISABLED,
    .pin_p      = SAADC_CH_PSELP_PSELP_AnalogInput0,
    .pin_n      = NRF_SAADC_INPUT_DISABLED,
};

nrfx_saadc_config_t adcCfg_sensor = {
    .low_power_mode = true,
    .resolution = NRF_SAADC_RESOLUTION_12BIT,
    .oversample = NRF_SAADC_OVERSAMPLE_DISABLED,
    .interrupt_priority = APP_IRQ_PRIORITY_LOW
};

void (*_adcVccCallback)(uint32_t) = NULL;
void (*_adcSensorCallback)(int16_t) = NULL;

static nrf_saadc_value_t adcBuf[2][ADC_SAMPLE_NUM+1];

bool sampleVcc = false;
bool setupVccSampling = false;

uint8_t adcSamplingState = ADC_SampleSensor;


void adcCallback(nrfx_saadc_evt_t const * p_event) {
    if (p_event->type == NRFX_SAADC_EVT_DONE) {

        switch(adcSamplingState) {
            case ADC_SampleSensor:
                // NRF_LOG_INFO("Sensor callback: %d", p_event->data.done.p_buffer[0]);
                break;
            case ADC_SetupSampleVcc:
                nrfx_saadc_channel_init(1, &adcChCfg_vddh);
                nrfx_saadc_buffer_convert(p_event->data.done.p_buffer, 1 + ADC_SAMPLE_NUM);
                adcSamplingState = ADC_SampleVcc;   // set next conversion to include VCC
                nrfx_saadc_sample();                // immediately trigger next conversion
                return;                             // nothing else should be done for the setup
                break;
            case ADC_SampleVcc:
                nrfx_saadc_channel_uninit(1);           // deactivate battery measurement channel again
                adcSamplingState = ADC_SampleSensor;    // reset ADC sampling state to normal    
                if(_adcVccCallback) {
                    uint32_t batVolt = ((uint32_t)p_event->data.done.p_buffer[1]) * 1000 / ADC_COUNTS_PER_VOLT;
                    _adcVccCallback(batVolt);
                }
                break;
        }


        // setup next conversion
        ret_code_t err = nrfx_saadc_buffer_convert(p_event->data.done.p_buffer, ADC_SAMPLE_NUM);

        // I could technically do the sensor reading callback also for ADC_SampleVcc, as the VCC reading also contains a sample of channel 0, but I'm not sure of the timing
        if (adcSamplingState == ADC_SampleSensor) {
            if (_adcSensorCallback) {
                _adcSensorCallback(p_event->data.done.p_buffer[0]);
            }
        }
    }
    else {
        NRF_LOG_INFO("Other ADC event: %d", p_event->type);
    }
    
}

void adcTriggerVccReading() {
    // flush out waiting sensor conversion, actual ADC setup for VCC is done in callback
    // would've liked to use saadc_abort() here, but apparently app_timer counts as an interrupt context and thus it can't be used
    adcSamplingState = ADC_SetupSampleVcc;
    nrfx_saadc_sample(); 
}

void adcSetVccCallback(void (*callback)(uint32_t)) {
    _adcVccCallback = callback;
}

void adcInit() {
    nrfx_saadc_init(&adcCfg_sensor, adcCallback);
    nrfx_saadc_channel_init(0, &adcChCfg_sensor);
    // nrfx_saadc_channel_init(1, &adcChCfg_vddh);  // init channel only when needed
    nrfx_saadc_buffer_convert(adcBuf[0], ADC_SAMPLE_NUM);
    // nrfx_saadc_buffer_convert(adcBuf[1], ADC_SAMPLE_NUM); // I don't think I need double-buffering?
}

bool adcTriggerSample() {
    if (adcSamplingState == ADC_SampleSensor) {
        nrfx_saadc_sample();
        return true;
    }
    return false;
}

void adcSetSensorCallback(void (*callback)(int16_t)) {
    _adcSensorCallback = callback;
}
