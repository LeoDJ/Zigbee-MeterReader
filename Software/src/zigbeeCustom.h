
#pragma once

#include "zb_zcl_power_config.h"

// simplified power declaration copied from https://github.com/lmahmutov/nrf52_multisensor/blob/master/zigbee_lacking.h
/**@brief power Measurement cluster attributes according to ZCL Specification 4.5.2.1.1. */
typedef struct {
    zb_uint8_t battery_voltage;
    zb_uint8_t battery_remaining_percentage;
    zb_uint32_t alarm_state;
} power_simplified_attr_t;

#define ZB_ZCL_DECLARE_POWER_CONFIG_SIMPLIFIED_ATTRIB_LIST(attr_list,voltage, remaining, alarm_state)  \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                             \
    ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID(voltage,),         \
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

