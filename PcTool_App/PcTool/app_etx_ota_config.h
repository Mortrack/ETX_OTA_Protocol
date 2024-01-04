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

#define ETX_OTA_VERBOSE 			        (1)   	        /**< @brief Flag value used to enable the compiler to take into account the code of the @ref etx_ota_protocol_host library that displays detailed information in the terminal window about the processes made in this library with a \c 1 . Otherwise, a \c 0 for only displaying minimum details of general processes only. */
#define CUSTOM_DATA_MAX_SIZE				(2048U)			/**< @brief	Designated maximum length in bytes for a possibly received ETX OTA Custom Data (i.e., @ref firmware_update_config_data_t::data ). */

#endif /* APP_ETX_OTA_CONFIG_H_ */

/** @} */

/** @} */
