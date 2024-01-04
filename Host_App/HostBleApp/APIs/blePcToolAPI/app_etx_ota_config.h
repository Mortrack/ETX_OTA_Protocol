/** @addtogroup etx_ota_protocol_host
 * @{
 */

/**@file
 *
 * @defgroup app_etx_ota_config Application ETX OTA Protocol Configuration File
 * @{
 *
 * @brief   This file should contain all the ETX OTA Protocol Configurations that wish to be changed from their default
 *          values, which are defined as such in the @ref etx_ota_protocol_host file.
 *
 * @note    It is highly suggested not to directly edit the Configurations Settings defined in the @ref etx_ota_config
 *          file. Instead of doing that whenever requiring different Configuration Settings, it is suggested to do that
 *          instead in this additional header file named as "app_etx_ota_config.h". However, to enable the use of this
 *          additional header file, you will also need to change the @ref ENABLE_APP_ETX_OTA_CONFIG define, that is
 *          located at @ref etx_ota_config, to a value of 1.
 * @note    For more details on how to use this @ref app_etx_ota_config , see the Readme.md file of the project on which
 *          the ETX OTA Protocol library is being used at.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	October 08, 2023.
 */

#ifndef APP_ETX_OTA_CONFIG
#define APP_ETX_OTA_CONFIG

#define PAYLOAD_PATH_OR_DATA_MAX_SIZE        (120*1024)          /**< @brief Designated maximum File Path length in bytes for the Payload that the user wants the @ref etx_ota_protocol_host program to send to the external device with which the Serial Port communication has been established with. */

#endif /* APP_ETX_OTA_CONFIG_H_ */

/** @} */

/** @} */
