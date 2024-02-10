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
 * @date	October 14, 2023.
 */

#ifndef ETX_OTA_CONFIG_H_
#define ETX_OTA_CONFIG_H_

#define ENABLE_APP_ETX_OTA_CONFIG              (1)          /**< @brief Flag used to enable the use of @ref app_etx_ota_config with a 1 or, otherwise to disable it with a 0. */
#if ENABLE_APP_ETX_OTA_CONFIG
#include "app_etx_ota_config.h" // This is the user custom ETX OTA configuration file that is recommended to use whenever it is desired to edit default configuration settings as defined in this @ref etx_ota_config file.
#endif

#ifndef ETX_OTA_VERBOSE
#define ETX_OTA_VERBOSE 			        (0)   	        /**< @brief Flag value used to enable the compiler to take into account the code of the @ref etx_ota_protocol_host library that displays detailed information in the terminal window about the processes made in this library with a \c 1 . Otherwise, a \c 0 for only displaying minimum details of general processes only. */
#endif

#ifndef FLASH_PAGE_SIZE_IN_BYTES
#define FLASH_PAGE_SIZE_IN_BYTES	        (1024U)			/**< @brief Flash Memory page size in bytes as defined by the MCU/MPU with which the Serial Port communication is to be established with. */
#endif

#ifndef ETX_BL_PAGE_SIZE
#define ETX_BL_PAGE_SIZE                    (34)            /**< @brief Designated number of Flash Memory pages that have been designated for the Bootloader Firmware of the MCU/MPU with which the Serial Port communication has been established with. */
#endif

#ifndef ETX_APP_PAGE_SIZE
#define ETX_APP_PAGE_SIZE                   (86)            /**< @brief Designated number of Flash Memory pages that have been designated for the Application Firmware of the MCU/MPU with which the Serial Port communication has been established with. */
#endif

#ifndef PAYLOAD_MAX_FILE_PATH_LENGTH
#define PAYLOAD_MAX_FILE_PATH_LENGTH        (1024)          /**< @brief Designated maximum File Path length in bytes for the Payload that the user wants the @ref etx_ota_protocol_host program to send to the external device with which the Serial Port communication has been established with. */
#endif

#ifndef RS232_BAUDRATE
#define RS232_BAUDRATE                      (115200)        /**< @brief Chosen Baudrate with which we want the host to run the RS232 protocol. */
#endif

#ifndef RS232_MODE_DATA_BITS
#define RS232_MODE_DATA_BITS                ('8')           /**< @brief Chosen Data-bits string with which we want the host to run the RS232 protocol. @details The valid data-bits in the Teuniz RS232 Library are '5', '6', '7' and '8'. */
#endif

#ifndef RS232_MODE_PARITY
#define RS232_MODE_PARITY                   ('N')           /**< @brief Chosen Parity string with which we want the host to run the RS232 protocol. @details The valid parity values in the Teuniz RS232 Library are:<br> - 'N' = None (i.e., No parity bit sent at all).<br> - 'O' = Odd (i.e., Set to mark if the number of 1s in data bits is odd.<br> - 'E' = Even (i.e., Set to space if the number of 1s in data bits is even. */
#endif

#ifndef RS232_MODE_STOPBITS
#define RS232_MODE_STOPBITS                 ('1')           /**< @brief Chosen Stop-bit string with which we want the host to run the RS232 protocol. @note The possible stopbit values in RS232 are '1' or '2'. */
#endif

#ifndef RS232_IS_FLOW_CONTROL
#define RS232_IS_FLOW_CONTROL               (0)             /**< @brief Chosen Flow Control decimal value to indicate with a 1 that we want the host to run the RS232 protocol with Flow Control enabled, or otherwise with a decimal value of 0 to indicate to the host to not run the RS232 protocol with Flow Control. */
#endif

#ifndef SEND_PACKET_BYTES_DELAY
#define SEND_PACKET_BYTES_DELAY             (1000)          /**< @brief Designated delay in microseconds that is desired to request before having send a byte of data from a certain ETX OTA Packet that is in process of being send to the MCU. */
#endif

#ifndef TEUNIZ_LIB_POLL_COMPORT_DELAY
#define TEUNIZ_LIB_POLL_COMPORT_DELAY       (500000)        /**< @brief Designated delay in microseconds that it is to be requested to apply each time before calling the @ref RS232_PollComport function. @details For all the calls to the @ref RS232_PollComport function, this definition is called once, except in the @ref send_etx_ota_data function, where this definition is called twice.  @note The @ref teuniz_rs232_library suggests to place an interval of 100 milliseconds, but it did not worked for me that way. Instead, it worked for me with 500ms. */
#endif

#ifndef TRY_AGAIN_SENDING_FWI_DELAY
#define TRY_AGAIN_SENDING_FWI_DELAY         (9000000)       /**< @brief Designated delay in microseconds that it is to be requested to apply in case that starting an ETX OTA Transaction fails once only. @note The slave device sometimes does not get the start of an ETX OTA Transaction after its UART Timeout expires, which is expected since there is some code in the loop that the slave device has there that makes it do something else before waiting again for an ETX OTA Transaction, but that should be evaded by making a second attempt with the delay established in this variable. */
#endif

#ifndef CUSTOM_DATA_MAX_SIZE
#define CUSTOM_DATA_MAX_SIZE				(1024U)				/**< @brief	Designated maximum length in bytes for a possibly received ETX OTA Custom Data (i.e., @ref firmware_update_config_data_t::data ). */
#endif

#endif /* ETX_OTA_CONFIG_H_ */

/** @} */

/** @} */
