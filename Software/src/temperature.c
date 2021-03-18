#include "temperature.h"
#include <nrf_temp.h>

void tempInit() {
    nrf_temp_init();
}

// returns temperature in 1/100°C
int32_t tempGet() {
    NRF_TEMP->TASKS_START = 1; // Start the temperature measurement.
    while (NRF_TEMP->EVENTS_DATARDY == 0) {  // Wait for conversion to finish, should only take about 36us
    }
    NRF_TEMP->EVENTS_DATARDY = 0; // Reset ready flag
    int32_t temp = nrf_temp_read() * 25; // read temperature (1/4 °C) and convert to 1/100°C steps
    NRF_TEMP->TASKS_STOP = 1; // Stop the temperature measurement
    return temp;
}