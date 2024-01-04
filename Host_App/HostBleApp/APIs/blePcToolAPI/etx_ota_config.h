/** @addtogroup etx_ota_protocol_host
 * @{
 */

/**@file
 *
 * @defgroup etx_ota_config Default ETX OTA Protocol Configuration File
 * @{
 *
 * @brief   This file contains all the Default ETX OTA Protocol Configurations.
 *
 * @note    It is highly suggested not to directly edit the Configurations Settings defined in this file. Instead of
 *          doing that whenever requiring different Configuration Settings, it is suggested to do that instead in an
 *          additional header file named as "app_etx_ota_config.h" whose File Path should be located as a sibbling of
 *          this @ref etx_ota_config header file. However, to enable the use of that additional header file, you will
 *          also need to change the @ref ENABLE_APP_ETX_OTA_CONFIG define, that is located at @ref etx_ota_config, to a
 *          value of 1.
 * @note    For more details on how to use this @ref etx_ota_config , see the Readme.md file of the project on which the
 *          ETX OTA Protocol library is being used at.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	November 02, 2023.
 */

#ifndef ETX_OTA_CONFIG_H_
#define ETX_OTA_CONFIG_H_

#define ENABLE_APP_ETX_OTA_CONFIG           (0)          /**< @brief Flag used to enable the use of @ref app_etx_ota_config with a 1 or, otherwise to disable it with a 0. */
#if ENABLE_APP_ETX_OTA_CONFIG
#include "app_etx_ota_config.h" // This is the user custom ETX OTA configuration file that is recommended to use whenever it is desired to edit default configuration settings as defined in this @ref etx_ota_config file.
#endif

#ifndef PAYLOAD_PATH_OR_DATA_MAX_SIZE
#define PAYLOAD_PATH_OR_DATA_MAX_SIZE       (20*1024)    /**< @brief Designated maximum File Path length in bytes for the Payload that the user wants the @ref etx_ota_protocol_host program to send to the external device with which the Serial Port communication has been established with. */
#endif

#ifndef HM10_RESET_AND_RENEW_CMDS_DELAY
#define HM10_RESET_AND_RENEW_CMDS_DELAY     (1000000U)   /**< @brief Designated time in microseconds for the Delay to be requested each time after either the Reset or Renew Command is solicited to the HM-10 BT Device. @details In order to guarantee that any other AT Command will work as expected after Resetting the HM-10 BT Device, a Delay is needed in order to wait for the Device to complete the Reset Process. This is particularly necessary if a Bluetooth Connection is requested to the HM-10 BT Device after applying a Reset to itself. @note On a validation test made with only one HM-10 BT Device, a Delay of 500 milliseconds worked fine, but repeating that test with more units in the future would help to learn the right value for this Definition. Therefore, a higher value than the one mentioned is suggested in order to guarantee that the HM-10 BT Device will work properly. */
#endif

#endif /* ETX_OTA_CONFIG_H_ */

/** @} */

/** @} */
