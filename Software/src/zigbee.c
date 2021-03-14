#include <zigbee_helpers.h>
#include <zb_error_handler.h>

#include "zigbee.h"

#if !defined ZB_ED_ROLE
#error Define ZB_ED_ROLE to compile End Device source code.
#endif


static zb_bool_t joined;

// initialize device state 
static sensor_device_ctx_t m_dev_ctx = {
    .basic_attr = (zb_zcl_basic_attrs_ext_t) {
        .zcl_version    = ZB_ZCL_VERSION,
        .app_version    = SENSOR_INIT_BASIC_APP_VERSION,
        .stack_version  = SENSOR_INIT_BASIC_STACK_VERSION,
        .hw_version     = SENSOR_INIT_BASIC_HW_VERSION,
        // .mf_name        = SENSOR_INIT_BASIC_MANUF_NAME, // need to set separately, because of stupid "encoding"
        // .model_id       = SENSOR_INIT_BASIC_MODEL_ID,
        // .date_code      = SENSOR_INIT_BASIC_DATE_CODE,
        .power_source   = SENSOR_INIT_BASIC_POWER_SOURCE,
        // .location_id    = SENSOR_INIT_BASIC_LOCATION_DESC,
        .ph_env         = SENSOR_INIT_BASIC_PH_ENV
    },
    .identify_attr = (zb_zcl_identify_attrs_t) {
        .identify_time  = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE
    },
    .power_attr = (zbc_power_simplified_attr_t) {
        .battery_voltage                = ZB_ZCL_POWER_CONFIG_BATTERY_VOLTAGE_INVALID,
        .battery_remaining_percentage   = ZB_ZCL_POWER_CONFIG_BATTERY_REMAINING_UNKNOWN,
        .alarm_state                    = ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_DEFAULT_VALUE
    },
    .metering_attr = (zbc_metering_extended_attr_t) {
        .curr_summ_delivered                = 0,
        .status                             = 0,
        .unit_of_measure                    = ZB_ZCL_METERING_UNIT_UNITLESS_BINARY,
        .summation_formatting               = 0,
        .device_type                        = ZB_ZCL_METERING_ELECTRIC_METERING,
        .instantaneous_demand               = 0,
        .demand_formatting                  = 0,
        .historical_consumption_formatting  = 0,
        .multiplier                         = 1,
        .divisor                            = 1000
    }
};


void initialize_sensor_device_ctx_strings() {
    // need to set strings with helper function because zboss puts the length of the string at the beginning
    ZIGBEE_SET_STRING(m_dev_ctx.basic_attr.mf_name, SENSOR_INIT_BASIC_MANUF_NAME);
    ZIGBEE_SET_STRING(m_dev_ctx.basic_attr.model_id, SENSOR_INIT_BASIC_MODEL_ID);
    ZIGBEE_SET_STRING(m_dev_ctx.basic_attr.date_code, SENSOR_INIT_BASIC_DATE_CODE);
    ZIGBEE_SET_STRING(m_dev_ctx.basic_attr.location_id, SENSOR_INIT_BASIC_LOCATION_DESC);
} 

ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &m_dev_ctx.identify_attr.identify_time);

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list,
                                     &m_dev_ctx.basic_attr.zcl_version,
                                     &m_dev_ctx.basic_attr.app_version,
                                     &m_dev_ctx.basic_attr.stack_version,
                                     &m_dev_ctx.basic_attr.hw_version,
                                     m_dev_ctx.basic_attr.mf_name,
                                     m_dev_ctx.basic_attr.model_id,
                                     m_dev_ctx.basic_attr.date_code,
                                     &m_dev_ctx.basic_attr.power_source,
                                     m_dev_ctx.basic_attr.location_id,
                                     &m_dev_ctx.basic_attr.ph_env,
                                     m_dev_ctx.basic_attr.sw_ver);

ZB_ZCL_DECLARE_POWER_CONFIG_SIMPLIFIED_ATTRIB_LIST(power_attr_list,
    &m_dev_ctx.power_attr.battery_voltage,
    &m_dev_ctx.power_attr.battery_remaining_percentage,
    &m_dev_ctx.power_attr.alarm_state);

// ZB_ZCL_DECLARE_METERING_ATTRIB_LIST_EXT replaced with direct definition for better readability
ZB_ZCL_START_DECLARE_ATTRIB_LIST(meter_attr_list)
    ZBC_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_WRITEABLE_ID(&m_dev_ctx.metering_attr.curr_summ_delivered),
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_STATUS_ID, (&m_dev_ctx.metering_attr.status))
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID, (&m_dev_ctx.metering_attr.unit_of_measure))
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID, (&m_dev_ctx.metering_attr.summation_formatting))
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID, (&m_dev_ctx.metering_attr.device_type))
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID, (&m_dev_ctx.metering_attr.instantaneous_demand))
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID, (&m_dev_ctx.metering_attr.demand_formatting))
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_HISTORICAL_CONSUMPTION_FORMATTING_ID, (&m_dev_ctx.metering_attr.historical_consumption_formatting))
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_MULTIPLIER_ID, (&m_dev_ctx.metering_attr.multiplier))
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_DIVISOR_ID, (&m_dev_ctx.metering_attr.divisor))
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST;

// ZB_DECLARE_MULTI_SENSOR_CLUSTER_LIST replaced with direct array declaration for better readability
zb_zcl_cluster_desc_t multi_sensor_clusters[] = {
    ZB_ZCL_CLUSTER_DESC(
        ZB_ZCL_CLUSTER_ID_IDENTIFY,
        ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),
        (identify_attr_list),
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_MANUF_CODE_INVALID
    ),
    ZB_ZCL_CLUSTER_DESC(
        ZB_ZCL_CLUSTER_ID_BASIC,
        ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),
        (basic_attr_list),
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_MANUF_CODE_INVALID
    ),
    ZB_ZCL_CLUSTER_DESC(
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
        ZB_ZCL_ARRAY_SIZE(power_attr_list, zb_zcl_attr_t),
        (power_attr_list),
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_MANUF_CODE_INVALID
    ),
    ZB_ZCL_CLUSTER_DESC(
        ZB_ZCL_CLUSTER_ID_METERING,
        ZB_ZCL_ARRAY_SIZE(meter_attr_list, zb_zcl_attr_t),
        (meter_attr_list),
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_MANUF_CODE_INVALID
    ),
    ZB_ZCL_CLUSTER_DESC(
        ZB_ZCL_CLUSTER_ID_IDENTIFY,
        0,
        NULL,
        ZB_ZCL_CLUSTER_CLIENT_ROLE,
        ZB_ZCL_MANUF_CODE_INVALID
    )
};

ZB_ZCL_DECLARE_MULTI_SENSOR_EP(multi_sensor_ep,
                               MULTI_SENSOR_ENDPOINT,
                               multi_sensor_clusters);
 
ZBOSS_DECLARE_DEVICE_CTX_1_EP(multi_sensor_ctx, multi_sensor_ep);

APP_TIMER_DEF(zb_app_timer);

zb_uint8_t batVoltage = 0;
zb_uint64_t powerUsage = 0;

/**@brief Function for handling nrf app timer.
 * 
 * @param[IN]   context   Void pointer to context function is called with.
 * 
 * @details Function is called with pointer to sensor_device_ep_ctx_t as argument.
 */
static void zb_app_timer_handler(void * context)
{
    zb_zcl_status_t zcl_status;

    zcl_status = zb_zcl_set_attr_val(MULTI_SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID,
        &batVoltage,
        ZB_FALSE );
    if(zcl_status != ZB_ZCL_STATUS_SUCCESS) {
        NRF_LOG_INFO("Set battery voltage value fail. zcl_status: %d", zcl_status);
    }
    zcl_status = zb_zcl_set_attr_val(MULTI_SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID,
        &batVoltage,
        ZB_FALSE );
    if(zcl_status != ZB_ZCL_STATUS_SUCCESS) {
        NRF_LOG_INFO("Set battery percentage value fail. zcl_status: %d", zcl_status);
    }
    batVoltage = (batVoltage + 1) % 200;
    // NRF_LOG_INFO("Battery Voltage: %d", m_dev_ctx.power_attr.battery_voltage);

    zb_uint48_t curSum = m_dev_ctx.metering_attr.curr_summ_delivered;

    NRF_LOG_INFO("current summation delivered: %d", curSum.low); // can't really print any values over 32bits, %lld doesn't work

    zcl_status = zb_zcl_set_attr_val(MULTI_SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_METERING,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID,
        (zb_uint8_t *)&powerUsage,
        ZB_FALSE);
    if(zcl_status != ZB_ZCL_STATUS_SUCCESS) {
        NRF_LOG_INFO("Set current summation delivered value fail. zcl_status: %d", zcl_status);
    }

    zcl_status = zb_zcl_set_attr_val(MULTI_SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_METERING,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID,
        (zb_uint8_t *)&powerUsage,
        ZB_FALSE);
    if(zcl_status != ZB_ZCL_STATUS_SUCCESS) {
        NRF_LOG_INFO("Set instantaneous demand value fail. zcl_status: %d", zcl_status);
    }

    powerUsage += 10;

    
}

// zigbee handler from lmahmutov, for later reference:

// /**@brief ZigBee stack event handler.
//  *
//  * @param[in]   param   Reference to ZigBee stack buffer used to pass arguments (signal).
//  */
// void zboss_signal_handler(zb_uint8_t param)
// {
//     zb_zdo_app_signal_hdr_t  * p_sg_p      = NULL;
//     zb_zdo_app_signal_type_t   sig         = zb_get_app_signal(param, &p_sg_p);
//     zb_ret_t                   status      = ZB_GET_APP_SIGNAL_STATUS(param);
//     zb_bool_t                  comm_status;

//     switch (sig)
//     {
//         case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
//         case ZB_BDB_SIGNAL_DEVICE_REBOOT:
//             if (status == RET_OK)
//             {
//                 NRF_LOG_INFO("Joined network successfully");
//                 joined = ZB_TRUE;
//                 // led_blink(4);
//                 /* timeout for receiving data from sensor and voltage from battery */
//                 ret_code_t err_code = app_timer_start(zb_app_timer, APP_TIMER_TICKS(30000), NULL);
//                 APP_ERROR_CHECK(err_code);
//                 /* change data request timeout */
//                 zb_zdo_pim_set_long_poll_interval(60000);
//             }
//             else
//             {
//                 // led_blink(6);
//                 NRF_LOG_ERROR("Failed to join network. Status: %d", status);
//                 joined = ZB_FALSE;
//             }
//             break;

//         case ZB_ZDO_SIGNAL_LEAVE:
//             if (status == RET_OK)
//             {
//                 joined = ZB_FALSE;
//                 // led_blink(8);
//                 ret_code_t err_code = app_timer_stop(zb_app_timer);
//                 APP_ERROR_CHECK(err_code);
//                 zb_zdo_signal_leave_params_t *p_leave_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_leave_params_t);
//                 NRF_LOG_INFO("Network left. Leave type: %d", p_leave_params->leave_type);
//             }
//             else
//             {
//                 NRF_LOG_ERROR("Unable to leave network. Status: %d", status);
//             }
//             break;


//         case ZB_COMMON_SIGNAL_CAN_SLEEP:
//             // zb_sleep_now();
//             break;

//         case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
//             if (status != RET_OK)
//             {
//                 NRF_LOG_WARNING("Production config is not present or invalid");
//             }
//             break;

//         default:
//             /* Unhandled signal. For more information see: zb_zdo_app_signal_type_e and zb_ret_e */
//             NRF_LOG_INFO("Unhandled signal %d. Status: %d", sig, status);
//             break;
//     }

//     if (param)
//     {
//         zb_buf_free(param);
//     }
// }


void zboss_signal_handler(zb_bufid_t bufid)
{
    zb_zdo_app_signal_hdr_t  * p_sg_p      = NULL;
    zb_zdo_app_signal_type_t   sig         = zb_get_app_signal(bufid, &p_sg_p);
    zb_ret_t                   status      = ZB_GET_APP_SIGNAL_STATUS(bufid);

    /* Update network status LED */
    // zigbee_led_status_update(bufid, ZIGBEE_NETWORK_STATE_LED);

    switch (sig)
    {
        case ZB_BDB_SIGNAL_DEVICE_REBOOT:
            /* fall-through */
        case ZB_BDB_SIGNAL_STEERING:
            /* Call default signal handler. */
            ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
            if (status == RET_OK)
            {
                ret_code_t err_code = app_timer_start(zb_app_timer, APP_TIMER_TICKS(1000), NULL);
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            /* Call default signal handler. */
            ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
            break;
    }

    if (bufid)
    {
        zb_buf_free(bufid);
    }
}

/**@brief Callback function for handling ZCL commands.
 *
 * @param[in]   bufid   Reference to Zigbee stack buffer used to pass received data.
 */
static zb_void_t zcl_device_cb(zb_bufid_t bufid)
{
    zb_uint8_t                       cluster_id;
    zb_uint8_t                       attr_id;
    zb_zcl_device_callback_param_t * p_device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

    NRF_LOG_INFO("zcl_device_cb id %hd", p_device_cb_param->device_cb_id);

    switch (p_device_cb_param->device_cb_id)
    {
        case ZB_ZCL_SET_ATTR_VALUE_CB_ID: {
            cluster_id = p_device_cb_param->cb_param.set_attr_value_param.cluster_id;
            attr_id    = p_device_cb_param->cb_param.set_attr_value_param.attr_id;
            // zb_uint48_t value = p_device_cb_param->cb_param.set_attr_value_param.values.data48;
            // switch(cluster_id) {
            //     case ZB_ZCL_CLUSTER_ID_METERING:
            //     break;
            // }
            // NRF_LOG_INFO("Unhandled cluster id: %d, attribute: %d, value: %d, %d", cluster_id, attr_id, value.low, value.high);
            break;
        }
    }
}


void zigbeeInit() {
    initialize_sensor_device_ctx_strings();

    /* Create Timer for reporting attribute */
    ret_code_t err_code = app_timer_create(&zb_app_timer, APP_TIMER_MODE_REPEATED, zb_app_timer_handler);
    APP_ERROR_CHECK(err_code);

    /* Set ZigBee stack logging level and traffic dump subsystem. */
    ZB_SET_TRACE_LEVEL(ZIGBEE_TRACE_LEVEL);
    ZB_SET_TRACE_MASK(ZIGBEE_TRACE_MASK);
    ZB_SET_TRAF_DUMP_OFF();

    /* Initialize ZigBee stack. */
    ZB_INIT("zigbee");

    /* Set device address to the value read from FICR registers. */
    zb_ieee_addr_t ieee_addr;
    zb_osif_get_ieee_eui64(ieee_addr);
    // ieee_addr[0] += 1;
    zb_set_long_address(ieee_addr);

    /* Set static long IEEE address. */
    zb_set_network_ed_role(IEEE_CHANNEL_MASK);
    zigbee_erase_persistent_storage(ZB_FALSE);

    zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(60000));
    zb_set_rx_on_when_idle(ZB_FALSE);

    // Initialize application context structure.
    // ZB_MEMSET(&m_dev_ctx, 0, sizeof(m_dev_ctx));

    /* Register callback for handling ZCL commands. */
    ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);

    // Register sensor device context (endpoints).
    ZB_AF_REGISTER_DEVICE_CTX(&multi_sensor_ctx);

    /* Initialize sensor device attibutes */
    // multi_sensor_clusters_attr_init();

    /** Start Zigbee Stack. */
    zb_ret_t zb_err_code = zboss_start_no_autostart();
    ZB_ERROR_CHECK(zb_err_code);

    // NRF_LOG_INFO("Sanity check that the m_dev_ctx didn't get overwritten and strings work: zcl_version: %d, Manufacturer: %s", m_dev_ctx.basic_attr.zcl_version, ((uint8_t *)m_dev_ctx.basic_attr.mf_name) + 1);
}

void zigbeeLoop() {
    zboss_main_loop_iteration();
}