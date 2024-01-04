/**@file
 *
 * @defgroup etx_ota_protocol_host ETX OTA Protocol Library for host machines
 * @{
 *
 * @brief	This module provides the functions required to enable the application to be able to send and handle Payload
 *          requests via ETX OTA Protocol, which also includes sending and requesting installation of Firmware Images,
 *          to our external device with which the Serial Port communication of this module has been established with.
 *
 * @details	The ETX OTA Protocol sends/receives data through Packets.
 * @details There 4 types of Packets:
 *          <ol>
 *              <li>Command Type Packets</li>
 *              <li>Header Type Packets</li>
 *              <li>Data Type Packets</li>
 *              <li>Response Type Packets</li>
 *          </ol>
 *          where it is suggested to see @ref ETX_OTA_Packet_t for more details.
 * @details The General Data Format for all types of Packets is the following set of bytes:
 *          <ol>
 *              <li>Start of Frame (SOF): 1 byte</li>
 *              <li>Packet Type: 1 byte</li>
 *              <li>Data Length: 2 bytes</li>
 *              <li>Data: 1 up to 1024 bytes (size must be perfectly divisible by 4 bytes)</li>
 *              <li>CRC32: 4 bytes</li>
 *              <li>End of Frame (EOF): 1 byte</li>
 *          </ol>
 *          but for finer details on the Data Format for each type of ETX OTA Packet, see @ref ETX_OTA_Command_Packet_t ,
 *          @ref ETX_OTA_Header_Packet_t , @ref ETX_OTA_Data_Packet_t and @ref ETX_OTA_Response_Packet_t .
 * @details On the other hand, a whole ETX OTA Transaction has 5 different states, where each of them can indicate in
 *          what part of the whole transaction we current are at, which are given by the following in that order:
 *          <ol>
 *              <li>ETX OTA Idle State</li>
 *              <li>ETX OTA Start State</li>
 *              <li>ETX OTA Header State</li>
 *              <li>ETX OTA Data State</li>
 *              <li>ETX OTA End State</li>
 *          </ol>
 *          where it is suggested to see @ref ETX_OTA_State for more details.
 * @details Finally, the way our external device will interact with our host machine is that the host will send a single
 *          packet for each ETX OTA State, except maybe in the ETX OTA Data State since multiple packets are allowed
 *          there. Nonetheless, our external device will expect the ETX OTA States to transition in the order previously
 *          shown and our external device will also both validate the received data and, if it is successful on that, it
 *          will always respond back to the host with an ETX OTA Response Type Packet to let it know whether or not our
 *          external device was able to successfully get and process the data of each Packet by sending an ACK, or
 *          otherwise with a NACK.
 *
 * @note    To learn more details about how to setup this library and how to compile this program, see the Readme.md
 *          File of the project that is using this library.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @date	November 24, 2023.
 */

#include <stdint.h>
#include "etx_ota_config.h" // This is the Default ETX OTA Protocol Configurations File.

#ifndef INC_ETX_OTA_PROTOCOL_HOST_H_
#define INC_ETX_OTA_PROTOCOL_HOST_H_

#define ETX_OTA_SOF  				(0xAA)    		/**< @brief Designated Start Of Frame (SOF) byte to indicate the start of an ETX OTA Packet. */
#define ETX_OTA_EOF  				(0xBB)    		/**< @brief Designated End Of Frame (EOF) byte to indicate the end of an ETX OTA Packet. */
#define ETX_OTA_SOF_SIZE			(1U)			/**< @brief	Designated SOF field size in bytes in a ETX OTA Packet. */
#define ETX_OTA_PACKET_TYPE_SIZE	(1U)			/**< @brief	Designated Packet Type field size in bytes in a ETX OTA Packet. */
#define ETX_OTA_DATA_LENGTH_SIZE	(2U)			/**< @brief	Designated Data Length field size in bytes in a ETX OTA Packet. */
#define ETX_OTA_DATA_MAX_SIZE 		(1024U)  		/**< @brief Designated maximum expected "Data" field's size in the General Data Format of an ETX OTA Packet. @note This definition's value does not stand for the size of the entire ETX OTA Packet. Instead, it represents the size of the "Data" field that is inside the General Data Format of an ETX OTA Packet. */
#define ETX_OTA_CRC32_SIZE			(4U)			/**< @brief	Designated 32-bit CRC field size in bytes in a ETX OTA Packet. */
#define ETX_OTA_EOF_SIZE			(1U)			/**< @brief	Designated EOF field size in bytes in a ETX OTA Packet. */
#define ETX_OTA_32BITS_RESET_VALUE  (0xFFFFFFFF)    /**< @brief Designated value to represent a 32-bit value in reset mode on the Flash Memory of the external device (connected to it via @ref COMPORT_NUMBER ). */
#define ETX_OTA_16BITS_RESET_VALUE  (0xFFFF)        /**< @brief Designated value to represent a 16-bit value in reset mode on the Flash Memory of the external device (connected to it via @ref COMPORT_NUMBER ). */
#define ETX_OTA_8BITS_RESET_VALUE   (0xFF)          /**< @brief Designated value to represent a 8-bit value in reset mode on the Flash Memory of the external device (connected to it via @ref COMPORT_NUMBER ). */

/**@brief	ETX OTA Exception codes.
 *
 * @details	These Exception Codes are returned by the functions of the @ref etx_ota_protocol_host module to indicate the
 *          resulting status of having executed the process contained in each of those functions. For example, to
 *          indicate that the process executed by a certain function was successful or that it has failed.
 */
typedef enum
{
    ETX_OTA_EC_OK       = 0U,    //!< ETX OTA Protocol was successful.
    ETX_OTA_EC_STOP     = 1U,    //!< ETX OTA Protocol process or transaction has been stopped.
    ETX_OTA_EC_NR		= 2U,	 //!< ETX OTA Protocol has concluded with no response from Host.
    ETX_OTA_EC_NA       = 3U,    //!< ETX OTA Payload received Not Applicable.
    ETX_OTA_EC_ERR      = 4U     //!< ETX OTA Protocol has failed.
} ETX_OTA_Status;

/**@brief	Command Line Arguments definitions.
 *
 * @details	These definitions are used as identifiers for the Command Line Arguments that should be received right after
 *          the resulting compiled file from this @ref etx_ota_protocol_host is executed via the terminal window.
 *
 * @note	For more details see @ref main and the Readme.md file of the project that uses the
 *          @ref etx_ota_protocol_host .
 */
typedef enum
{
    TERMINAL_WINDOW_EXECUTION_COMMAND   = 0U,   //!< Command Line Argument Index 0, which should contain the string of the literal terminal window command used by the user to execute the @ref etx_ota_protocol_host program.
    COMPORT_NUMBER                      = 1U,   //!< Command Line Argument Index 1, which should contain the Comport with which the user wants the @ref etx_ota_protocol_host program to establish a connection with via RS232 protocol.
    PAYLOAD_PATH                        = 2U,   //!< Command Line Argument Index 2, which should contain the File Path, with respect to the File Location of the executed compiled file of the @ref etx_ota_protocol_host program, to the Payload file that the user wants this program to load and send towards the desired external device that is chosen via the @ref COMPORT_NUMBER .
    ETX_OTA_PAYLOAD_TYPE                = 3U    //!< Command Line Argument Index 3, which should contain the ETX OTA Payload Type to indicate to the @ref etx_ota_protocol_host program the type of Payload data that will be given. @note To see the available ETX OTA Payload Types, see @ref ETX_OTA_Payload_t .
} Command_Line_Arguments;

/**@brief	Payload Type definitions available in the ETX OTA Protocol.
 *
 * @details	Whenever the host sends to the external device (connected to it via @ref COMPORT_NUMBER ) a
 *          @ref ETX_OTA_PACKET_TYPE_DATA Packet Type (i.e., a Data Type Packet), there are several possible Payload
 *          Types for this Data Type Packet that indicate the type of data that the external device should expect to
 *          receive from the whole Data Type Packets that it will receive (this is because a whole Payload can be
 *          received from one or more Data Type Packets). These Payload Types are the following:
 */
typedef enum
{
    ETX_OTA_Application_Firmware_Image  = 0U,   	//!< ETX OTA Application Firmware Image Data Packet Type.
    ETX_OTA_Bootloader_Firmware_Image   = 1U,  		//!< ETX OTA Bootloader Firmware Image Data Packet Type.
    ETX_OTA_Custom_Data                 = 2U   		//!< ETX OTA Custom Data Packet Type.
} ETX_OTA_Payload_t;

/**@brief   Sends some desired ETX OTA Payload Data to a specified device by using the ETX OTA Protocol.
 *
 * @param comport               The actual comport that wants to be used for the RS232 protocol to connect to a desired
 *                              external device.
 * @param[in] payload_path      File Path towards the Payload File that is desired to load and send to the external
 *                              device (i.e., the device that is desired to connect to via the \p comport param) so that
 *                              it processes it correspondingly.
 * @param ETX_OTA_Payload_Type  The Payload Type.
 *
 * @retval  ETX_OTA_EC_OK
 * @retval 	ETX_OTA_EC_ERR
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 24, 2023.
 */
ETX_OTA_Status start_etx_ota_process(int comport, char payload_path[], ETX_OTA_Payload_t ETX_OTA_Payload_Type);

#endif /* INC_ETX_OTA_PROTOCOL_HOST_H_ */

/** @} */
