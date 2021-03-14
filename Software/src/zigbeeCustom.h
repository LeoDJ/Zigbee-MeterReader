#pragma once

// This file contains custom data structures that didn't exist in the ZCL libraries
// "zbc" prefix = Zigbee Custom

#include "zb_zcl_power_config.h"


// simplified power declaration copied from https://github.com/lmahmutov/nrf52_multisensor/blob/master/zigbee_lacking.h
/**@brief power Measurement cluster attributes according to ZCL Specification 4.5.2.1.1. */
typedef struct {
    zb_uint8_t battery_voltage;
    zb_uint8_t battery_remaining_percentage;
    zb_uint32_t alarm_state;
} zbc_power_simplified_attr_t;

#define ZB_ZCL_DECLARE_POWER_CONFIG_SIMPLIFIED_ATTRIB_LIST(attr_list,voltage, remaining, alarm_state)  \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                             \
    ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_REPORTING_ID(voltage,),         \
    ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_REMAINING_REPORTING_ID(remaining,),    \
    ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_STATE_ID(alarm_state,), \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_REMAINING_REPORTING_ID(data_ptr, bat_num) \
{                                                               \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_PERCENTAGE_REMAINING_ID,     \
  ZB_ZCL_ATTR_TYPE_U8,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,  \
  (zb_voidp_t) data_ptr                                         \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_REPORTING_ID(data_ptr, bat_num) \
{                                                               \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_VOLTAGE_ID,       \
  ZB_ZCL_ATTR_TYPE_U8,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,  \
  (zb_voidp_t) data_ptr                                         \
}

#define ZBC_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_WRITEABLE_ID(data_ptr) \
{                                                                                              \
    ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID,                                       \
    ZB_ZCL_ATTR_TYPE_U48,                                                                      \
    ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING,                              \
    (zb_voidp_t) data_ptr                                                                      \
}

typedef union {
    struct {
        zb_uint8_t digits_right : 3;    // Nubmer of digits to the right of the decimal point
        zb_uint8_t digits_left : 4;     // Nubmer of digits to the left of the decimal point
        zb_uint8_t suppress_zeros : 1;  // If set, suppress leading zeros
        };
    zb_uint8_t raw;
} zbc_formatting_t;

// expanded from zb_zcl_metering_attrs_t
typedef struct {
    zb_uint48_t curr_summ_delivered;                    // ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID    
    zb_uint8_t status;                                  // ZB_ZCL_ATTR_METERING_STATUS_ID    
    zb_uint8_t unit_of_measure;                         // ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID    
    zbc_formatting_t summation_formatting;              // ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID    
    zb_uint8_t device_type;                             // ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID    
    zb_int24_t instantaneous_demand;                    // ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID    
    zbc_formatting_t demand_formatting;                 // ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID
    zbc_formatting_t historical_consumption_formatting; // ZB_ZCL_ATTR_METERING_HISTORICAL_CONSUMPTION_FORMATTING_ID
    zb_uint24_t multiplier;                             // ZB_ZCL_ATTR_METERING_MULTIPLIER_ID
    zb_uint24_t divisor;                                // ZB_ZCL_ATTR_METERING_DIVISOR_ID
} zbc_metering_extended_attr_t;

/* Use ZB_ZCL_SET_STRING_VAL to set strings, because the first byte should  
* contain string length without trailing zero.
*
* For example "test" string wil be encoded as:
*   [(0x4), 't', 'e', 's', 't']
*/
#define ZIGBEE_SET_STRING(stringToSet, value) \
    ZB_ZCL_SET_STRING_VAL(stringToSet, \
    value, \
    ZB_ZCL_STRING_CONST_SIZE(value)) 

