/** @addtogroup pre_bl_side_etx_ota
 * @{
 */

/** @addtogroup main_etx_ota_config
 * @{
 */

/**@file
 * @brief	ETX OTA Protocol personalized configuration file for Pre-Bootloader Firmwares (edit ETX OTA Configurations here).
 *
 * @defgroup app_etx_ota_config Application ETX OTA Protocol Configuration File
 * @{
 *
 * @brief   This file should contain all the Pre-Bootloader Firmware Configurations made by the user/implementor.
 *
 * @note    @note    It is highly suggested not to directly edit the Configurations Settings defined in the @ref etx_ota_config
 *          file. Instead of doing that whenever requiring different Configuration Settings, it is suggested to do that
 *          instead in this additional header file named as "app_etx_ota_config.h". However, to enable the use of this
 *          additional header file, you will also need to change the @ref ENABLE_APP_ETX_OTA_CONFIG define, that is
 *          located at @ref etx_ota_config, to a value of 1.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	November 16, 2023.
 */

#ifndef APP_ETX_OTA_CONFIG
#define APP_ETX_OTA_CONFIG

#define ETX_BL_FLASH_ADDR					(0x08002000)	/**< @brief Designated Flash Memory start address for the Bootloader Firmware of our MCU/MPU. */

#endif /* APP_ETX_OTA_CONFIG_H_ */

/** @} */ // app_etx_ota_config

/** @} */ // main_etx_ota_config

/** @} */ // pre_bl_side_etx_ota
