/** @file
 *	@brief	ETX OTA Protocol source file for Pre-Bootloader Firmwares.
 *
 * @defgroup pre_bl_side_etx_ota Firmware Update module
 * @{
 *
 * @brief	This module provides the functions required to enable the application to be able to install a Firmware Image
 *          located at the Flash Memory location address of the Application Firmware (i.e., @ref ETX_APP_FLASH_ADDR )
 *          into the Flash Memory location address of the Bootloader Firmware (i.e., @ref ETX_BL_FLASH_ADDR ).
 *
 * @note    This module expects the Firmware Image to have been previously stored with the ETX OTA Protocol made by
 *          Mortrack.
 *
 * @details The way in which this Firmware Image installation process is made is via the ETX OTA Protocol, which is a
 *          protocol that serves for the purpose of transferring data from a certain host device to a slave device
 *          (i.e., our MCU/MPU) and, in particular, for data whose integrity is considered to be critically important
 *          since this protocol prioritizes data transfer reliability over data transfer speed.
 *
 * @note    Another thing to highlight is that this @ref pre_bl_side_etx_ota has included the "stm32f1xx_hal.h" header
 *          file in its .c file counter part (i.e., firmware_update_config.c ) to be able to use the UART in this
 *          module. However, this header file is specifically meant for the STM32F1 series devices. If yours is from a
 *          different type, then you will have to substitute the right one here for your particular STMicroelectronics
 *          device. However, if you cant figure out what the name of that header file is, then simply substitute that
 *          line of code from this firmware_update_config.c File by: #include "main.h"
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @date	November 18, 2023.
 */
#include <stdint.h>
#include "firmware_update_config.h" // We call the library that holds the Firmware Update Configurations sub-module.

#ifndef PRE_BL_SIDE_ETX_OTA_H_
#define PRE_BL_SIDE_ETX_OTA_H_

/**@brief	ETX OTA Exception codes.
 *
 * @details	These Exception Codes are returned by the functions of the @ref pre_bl_side_firmware_update module to
 *          indicate the resulting status of having executed the process contained in each of those functions. For
 *          example, to indicate that the process executed by a certain function was successful or that it has failed.
 */
typedef enum
{
	ETX_OTA_EC_OK       = 0U,    //!< ETX OTA Protocol was successful. @note The code from the @ref HAL_ret_handler function contemplates that this value will match the one given for \c HAL_OK from @ref HAL_StatusTypeDef .
	ETX_OTA_EC_STOP     = 1U,    //!< ETX OTA Protocol process or transaction has been stopped.
	ETX_OTA_EC_NR		= 2U,	 //!< ETX OTA Protocol has concluded with no response from Host.
	ETX_OTA_EC_NA       = 3U,    //!< ETX OTA Payload received or to be received Not Applicable.
	ETX_OTA_EC_ERR      = 4U     //!< ETX OTA Protocol has failed.
} ETX_OTA_Status;

/**@brief   Attempts to install a Firmware Image located at the Flash Memory location address of the Application
 *          Firmware (i.e., @ref ETX_APP_FLASH_ADDR ) into the Flash Memory location address of the Bootloader Firmware
 *          (i.e., @ref ETX_BL_FLASH_ADDR ).
 *
 * @details This function will first check if the size in bytes of the Firmware Image located at the Flash Memory
 *          location address of the Application firmware (i.e., @ref ETX_APP_FLASH_ADDR ) is perfectly divisible by 4
 *          bytes. Only in the case that this is true, will then this function install that Firmware Image. Otherwise,
 *          this function will terminate.
 * @details Also, in the case that the Firmware Image is successfully installed, then this function will update the
 *          Configuration Settings from the @ref firmware_update_config correspondingly and will persist them into its
 *          designated Flash Memory (@ref firmware_update_config_data_t::BL_fw_size ,
 *          @ref firmware_update_config_data_t::BL_fw_rec_crc and
 *          @ref firmware_update_config_data_t::is_bl_fw_install_pending will be updated only).
 *
 * @note    This function expects the implementer to have correctly previously validated that the Firmware Image located
 *          at the Flash Memory location address of the Application Firmware (i.e., @ref ETX_APP_FLASH_ADDR )
 *          corresponds to a Bootloader Firmware Image. If that image ends up not being a Bootloader Firmware Image
 *          type, then our MCU/MPU's program/software will most probably not work.
 *
 * @param[in] p_fw_config   Pointer to the struct that should already contain the latest data of the
 *                          @ref firmware_update_config .
 *
 * @retval  ETX_OTA_EC_OK   if the Firmware Image located at the Flash Memory location address of the Application
 *                          Firmware (i.e., @ref ETX_APP_FLASH_ADDR ) is correctly installed into the Flash Memory
 *                          location address of the Bootlaoder Firmware (i.e., @ref ETX_BL_FLASH_ADDR ).
 * @retval	ETX_OTA_EC_NR   if there was no response from the HAL of our MCU/MPU whenever either attempting to
 *                          unlock/lock the HAL FLASH or; to erase/write in the Flash Memory designated to the
 *                          Bootloader Firmware Image of our MCU/MPU.
 * @retval	ETX_OTA_EC_ERR  otherwise.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 18, 2023.
 */
ETX_OTA_Status install_bl_stored_in_app_fw(firmware_update_config_data_t *p_fw_config);

#endif /* PRE_BL_SIDE_ETX_OTA_H_ */

/** @} */