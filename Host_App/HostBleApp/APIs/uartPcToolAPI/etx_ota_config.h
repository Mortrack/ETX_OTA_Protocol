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

#define ENABLE_APP_ETX_OTA_CONFIG              (0)          /**< @brief Flag used to enable the use of @ref app_etx_ota_config with a 1 or, otherwise to disable it with a 0. */
#if ENABLE_APP_ETX_OTA_CONFIG
#include "app_etx_ota_config.h" // This is the user custom ETX OTA configuration file that is recommended to use whenever it is desired to edit default configuration settings as defined in this @ref etx_ota_config file.
#endif

#ifndef PAYLOAD_PATH_OR_DATA_MAX_SIZE
#define PAYLOAD_PATH_OR_DATA_MAX_SIZE        (20*1024)          /**< @brief Designated maximum File Path length in bytes for the Payload that the user wants the @ref etx_ota_protocol_host program to send to the external device with which the Serial Port communication has been established with. */
#endif

#endif /* ETX_OTA_CONFIG_H_ */

/** @} */

/** @} */
