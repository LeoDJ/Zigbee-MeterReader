#pragma once

#include <zboss_api.h>
#include <zboss_api_addons.h>

#include "zigbeeCustom.h"

/* Basic cluster attributes initial values. For more information, see section 3.2.2.2 of the ZCL specification. */
#define SENSOR_INIT_BASIC_APP_VERSION       01                                  /**< Version of the application software (1 byte). */
#define SENSOR_INIT_BASIC_STACK_VERSION     10                                  /**< Version of the implementation of the Zigbee stack (1 byte). */
#define SENSOR_INIT_BASIC_HW_VERSION        11                                  /**< Version of the hardware of the device (1 byte). */
#define SENSOR_INIT_BASIC_MANUF_NAME        "Gingerlabs"                        /**< Manufacturer name (32 bytes). */
#define SENSOR_INIT_BASIC_MODEL_ID          "Pulsesensor"                       /**< Model number assigned by the manufacturer (32-bytes long string). */
#define SENSOR_INIT_BASIC_DATE_CODE         "20210306"                          /**< Date provided by the manufacturer of the device in ISO 8601 format (YYYYMMDD), for the first 8 bytes. The remaining 8 bytes are manufacturer-specific. */
#define SENSOR_INIT_BASIC_POWER_SOURCE      ZB_ZCL_BASIC_POWER_SOURCE_BATTERY   /**< Type of power source or sources available for the device. For possible values, see section 3.2.2.2.8 of the ZCL specification. */
#define SENSOR_INIT_BASIC_LOCATION_DESC     "Unspecified"                       /**< Description of the physical location of the device (16 bytes). You can modify it during the commisioning process. */
#define SENSOR_INIT_BASIC_PH_ENV            ZB_ZCL_BASIC_ENV_UNSPECIFIED        /**< Description of the type of physical environment. For possible values, see section 3.2.2.2.10 of the ZCL specification. */

#define MULTI_SENSOR_ENDPOINT               10                                  /**< Device endpoint. Used to receive light controlling commands. */
#define ZIGBEE_NETWORK_STATE_LED            BSP_BOARD_LED_2                     /**< LED indicating that light switch successfully joind Zigbee network. */


#define ZB_DEVICE_VER_MULTI_SENSOR          0                                    /**< Multisensor device version. */
#define ZB_MULTI_SENSOR_REPORT_ATTR_COUNT   10  // dunno, just put high number   /**< Number of attributes mandatory for reporting in the Temperature and Pressure Measurement cluster. */
#define ZB_MULTI_SENSOR_IN_CLUSTER_NUM      4                                    /**< Number of the input (server) clusters in the multisensor device. (multi_sensor_clusters) */
#define ZB_MULTI_SENSOR_OUT_CLUSTER_NUM     1                                    /**< Number of the output (client) clusters in the multisensor device. */

#define IEEE_CHANNEL_MASK                   ZB_TRANSCEIVER_ALL_CHANNELS_MASK

void zigbeeInit();
void zigbeeLoop();

/* Main application customizable context. Stores all settings and static values. */
typedef struct {
    zb_zcl_basic_attrs_ext_t            basic_attr;
    zb_zcl_identify_attrs_t             identify_attr;
    zbc_power_simplified_attr_t         power_attr;
    zbc_metering_extended_attr_t        metering_attr;
} sensor_device_ctx_t;

/** @brief Declares simple descriptor for the "Device_name" device.
 *  
 *  @param ep_name          Endpoint variable name.
 *  @param ep_id            Endpoint ID.
 *  @param in_clust_num     Number of the supported input clusters.
 *  @param out_clust_num    Number of the supported output clusters.
 */
#define ZB_ZCL_DECLARE_MULTI_SENSOR_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =         \
  {                                                                                   \
    ep_id,                                                                            \
    ZB_AF_HA_PROFILE_ID,                                                              \
    ZB_HA_METER_INTERFACE_DEVICE_ID,                                                  \
    ZB_DEVICE_VER_MULTI_SENSOR,                                                       \
    0,                                                                                \
    in_clust_num,                                                                     \
    out_clust_num,                                                                    \
    {                                                                                 \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                        \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                     \
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                                                 \
      ZB_ZCL_CLUSTER_ID_METERING,                                                     \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                     \
    }                                                                                 \
  }

/** @brief Declares endpoint for the multisensor device.
 *   
 *  @param ep_name          Endpoint variable name.
 *  @param ep_id            Endpoint ID.
 *  @param cluster_list     Endpoint cluster list.
 */
#define ZB_ZCL_DECLARE_MULTI_SENSOR_EP(ep_name, ep_id, cluster_list)                \
    ZB_ZCL_DECLARE_MULTI_SENSOR_DESC(ep_name,                                       \
        ep_id,                                                                      \
        ZB_MULTI_SENSOR_IN_CLUSTER_NUM,                                             \
        ZB_MULTI_SENSOR_OUT_CLUSTER_NUM);                                           \
    ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,            \
                                        ZB_MULTI_SENSOR_REPORT_ATTR_COUNT);         \
    ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id,                                     \
        ZB_AF_HA_PROFILE_ID,                                                        \
        0,                                                                          \
        NULL,                                                                       \
        ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                     \
        cluster_list,                                                               \
        (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                           \
        ZB_MULTI_SENSOR_REPORT_ATTR_COUNT, reporting_info## device_ctx_name, 0, NULL)
