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
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 28, 2023.
 */

#include <stdint.h>
#include "etx_ota_config.h" // This is the Default ETX OTA Protocol Configurations File.
#include "HM10_ble_driver/Inc/hm10_ble_driver.h" // Custom Mortrack's Library to be able to send and/or receive data to/from the HM-10 BT Device.

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
    ETX_OTA_EC_OK                           = 0U,   //!< ETX OTA Protocol was successful.
    ETX_OTA_EC_STOP                         = 1U,   //!< ETX OTA Protocol process or transaction has been stopped.
    ETX_OTA_EC_NR		                    = 2U,	//!< ETX OTA Protocol has concluded with no response from Host.
    ETX_OTA_EC_NA                           = 3U,   //!< ETX OTA Payload received Not Applicable.
    ETX_OTA_EC_ERR                          = 4U,   //!< ETX OTA Protocol has failed. @note This field is not used in @ref etx_ota_protocol_host but it is kept since it is used in the ETX OTA Protocol Library for the slave devices. This way, Exception Codes compatibility between both libraries will match with respect to each other.
    ETX_OTA_EC_INV_CMD_LINE_ARG             = 5U,   //!< ETX OTA Protocol API side received invalid number of Command Line Arguments.
    ETX_OTA_EC_UNRECOG_PAYLOAD              = 6U,   //!< ETX OTA Payload Type not recognized.
    ETX_OTA_EC_OPEN_COM_ERR                 = 7U,   //!< ETX OTA Protocol has failed to Open requested RS232 Comport.
    ETX_OTA_EC_OPEN_FILE_ERR                = 8U,   //!< ETX OTA Protocol has failed to Open requested the File where the Payload lies in.
    ETX_OTA_EC_READ_FILE_ERR                = 9U,   //!< ETX OTA Protocol has failed to Read the requested File where the Payload lies in.
    ETX_OTA_EC_START_CMD_SEND_DATA_ERR      = 10U,  //!< ETX OTA Protocol has failed to send ETX OTA Start Command's bytes to the slave device.
    ETX_OTA_EC_START_CMD_NACK_RESP          = 11U,  //!< ETX OTA Protocol has received a NACK Response after sending an ETX OTA Start Command to the slave device.
    ETX_OTA_EC_HEADER_PCKT_SEND_DATA_ERR    = 12U,  //!< ETX OTA Protocol has failed to send ETX OTA Header Type Packet's bytes to the slave device.
    ETX_OTA_EC_HEADER_PCKT_NACK_RESP        = 13U,  //!< ETX OTA Protocol has received a NACK Response after sending an ETX OTA Header Type Packet to the slave device.
    ETX_OTA_EC_DATA_PCKT_SEND_DATA_ERR      = 14U,  //!< ETX OTA Protocol has failed to send ETX OTA Data Type Packet's bytes to the slave device.
    ETX_OTA_EC_DATA_PCKT_NACK_RESP          = 15U,  //!< ETX OTA Protocol has received a NACK Response after sending an ETX OTA Data Type Packet to the slave device.
    ETX_OTA_EC_END_CMD_SEND_DATA_ERR        = 16U,  //!< ETX OTA Protocol has failed to send ETX OTA End Command's bytes to the slave device.
    ETX_OTA_EC_END_CMD_NACK_RESP            = 17U,  //!< ETX OTA Protocol has received a NACK Response after sending an ETX OTA End Command to the slave device.
    ETX_OTA_EC_ABORT_CMD_SEND_DATA_ERR      = 18U,  //!< ETX OTA Protocol has failed to send ETX OTA Abort Command's bytes to the slave device.
    ETX_OTA_EC_ABORT_CMD_NACK_RESP          = 19U,  //!< ETX OTA Protocol has received a NACK Response after sending an ETX OTA Abort Command to the slave device.
    ETX_OTA_EC_ABORT_LOOP_ERR               = 20U,  //!< ETX OTA Protocol failed to successfully send an Abort Command during the loop-process of sending Abort Commands in an attempt to guarantee getting the slave device to leave any on-going ETX OTA Transaction before starting a new one.
    ETX_OTA_EC_BLE_INIT_ERR                 = 21U,  //!< ETX OTA Protocol Bluetooth Initialization has failed due to invalid comport value.
    ETX_OTA_EC_BLE_AT_CMD_ERR               = 22U,  //!< ETX OTA Protocol Bluetooth Disconnect Command has failed.
    ETX_OTA_EC_BLE_TYPE_CMD_ERR             = 23U,  //!< ETX OTA Protocol Bluetooth Set Pin Code Mode Command has failed.
    ETX_OTA_EC_BLE_RESET_CMD_ERR            = 24U,  //!< ETX OTA Protocol Bluetooth Reset Command has failed.
    ETX_OTA_EC_BLE_CONN_CMD_ERR             = 25U   //!< ETX OTA Protocol Bluetooth Connect-To-Address Command has failed.
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
    TERMINAL_WINDOW_EXECUTION_COMMAND   = 0U,    //!< Command Line Argument Index 0, which should contain the string of the literal terminal window command used by the user to execute the @ref etx_ota_protocol_host program.
    COMPORT_NUMBER                      = 1U,    //!< Command Line Argument Index 1, which should contain the Comport with which the user wants the @ref etx_ota_protocol_host program to establish a connection with via RS232 protocol.
    PAYLOAD_SIZE                        = 2U,    //!< Command Line Argument Index 2, which should contain the length in bytes of specifically an ETX OTA Custom Data Payload. @note This field can have any positive numeric value whenever not receiving an ETX OTA Custom Data Payload since the program will ignore this value in that case.
    PAYLOAD_PATH_OR_DATA                = 3U,    //!< Command Line Argument Index 3, which should either contain the desired ETX OTA Custom Data to be sent in case of having that type of ETX OTA Transaction, or should contain the File Path to the Payload file (with respect to the File Location of the executed compiled file of the @ref main_program program) that the user wants this program to load and send towards the desired external device that is chosen via the @ref COMPORT_NUMBER .
    ETX_OTA_PAYLOAD_TYPE                = 4U,    //!< Command Line Argument Index 4, which should contain the ETX OTA Payload Type to indicate to the @ref etx_ota_protocol_host program the type of Payload data that will be given. @note To see the available ETX OTA Payload Types, see @ref ETX_OTA_Payload_t .
    FLASH_PAGE_SIZE_IN_BYTES            = 5U,    //!< Command Line Argument Index 5, which should contain the size in bytes of each Flash Memory Page of the slave MCU/MPU to communicate with via the ETX OTA Protocol.
    ETX_BL_PAGE_SIZE                    = 6U,    //!< Command Line Argument Index 6, which should contain the Number of Pages designated to the Bootloader Firmware of the slave MCU/MPU to communicate with via the ETX OTA Protocol.
    ETX_APP_PAGE_SIZE                   = 7U,    //!< Command Line Argument Index 7, which should contain the Number of Pages designated to the Application Firmware of the slave MCU/MPU to communicate with via the ETX OTA Protocol.
    RS232_BAUDRATE                      = 8U,    //!< Command Line Argument Index 8, which should contain the Baud rate that wants to be used on the UART with which a communication with the slave device will be made.
    RS232_MODE_DATA_BITS                = 9U,    //!< Command Line Argument Index 9, which should contain the Data-bits with which we want the host to run the RS232 protocol.
    RS232_MODE_PARITY                   = 10U,   //!< Command Line Argument Index 10, which should contain the Parity with which we want the host to run the RS232 protocol.
    RS232_MODE_STOPBITS                 = 11U,   //!< Command Line Argument Index 11, which should contain the Stop-bits with which we want the host to run the RS232 protocol.
    RS232_IS_FLOW_CONTROL               = 12U,   //!< Command Line Argument Index 12, which should contain the Flag value to indicate whether we want the host to run the RS232 protocol with Flow Control with a \c "1" or otherwise with a \c "0" .
    SEND_PACKET_BYTES_DELAY             = 13U,   //!< Command Line Argument Index 13, which should contain the desired delay in microseconds to be requested after having send a byte of data from a certain ETX OTA Packet that is in process of being sent to the slave device.
    TEUNIZ_LIB_POLL_COMPORT_DELAY       = 14U,   //!< Command Line Argument Index 14, which should contain the desired delay in microseconds that is to be requested to apply each time before calling the @ref RS232_PollComport function.
    TRY_AGAIN_SENDING_FWI_DELAY         = 15U,   //!< Command Line Argument Index 15, which should contain the desired delay in microseconds that it is to be requested to apply in case that starting an ETX OTA Transaction fails once only.
    HM10_CONNECT_TO_ADDRESS_TIMEOUT     = 16U,   //!< Command Line Argument Index 16, which should contain the desired time in microseconds that is to be requested for waiting for establishing a Bluetooth Connection between our HM-10 BT Device and the desired BT Device. @note For more details, see @ref ETX_OTA_API_t::hm10_connect_to_address_timeout .
    BLUETOOTH_ADDRESS                   = 17U    //!< Command Line Argument Index 17, which should contain the Bluetooth Address, without the colon characters, of the Remote Bluetooth Device with which it is desired to establish a Bluetooth Connection with. @note As an example, if we wanted to connect to the Bluetooth Address 00:17:EA:09:09:09, then the Command Line Argument value for this parameter should be "0017EA090909".
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

// TODO: Although not currently needed for using this API, it is still pending to add some logic to both accept via the API and to process inside its code the "payload_max_file_path_length" field:
/**@brief	ETX OTA API parameters structure.
 *
 * @details	This structure contains all the fields that are transacted with the API part of the @ref main_program
 *          program and whatever external program with which there is some data interaction with the API provided by
 *          @ref main_program .
 */
typedef struct {
    int comport;		                                       //!< The actual comport that wants to be used for the RS232 protocol to connect to a desired external device.
    ETX_OTA_Payload_t ETX_OTA_Payload_Type;			           //!< The Payload Type that the API received.
    uint32_t rs232_baudrate;                                   //!< Chosen Baudrate with which we want the host to run the RS232 protocol.
    uint32_t send_packet_bytes_delay;                          //!< Delay in microseconds that is desired to request before having send a byte of data from a certain ETX OTA Packet that is in process of being sent to the MCU.
    uint32_t teuniz_lib_poll_comport_delay;                    //!< Delay in microseconds that it is to be requested to apply each time before calling the @ref RS232_PollComport function. @details For all the calls to the @ref RS232_PollComport function, this definition is called once, except in the @ref send_etx_ota_data function, where this definition is called twice.  @note The @ref teuniz_rs232_library suggests to place an interval of 100 milliseconds, but it did not worked for me that way. Instead, it worked for me with 500ms.
    uint32_t try_again_sending_fwi_delay;                      //!< Delay in microseconds that it is to be requested to apply in case that starting an ETX OTA Transaction fails once only. @note The slave device sometimes does not get the start of an ETX OTA Transaction after its UART Timeout expires, which is expected since there is some code in the loop that the slave device has there that makes it do something else before waiting again for an ETX OTA Transaction, but that should be evaded by making a second attempt with the delay established in this variable.
    uint32_t hm10_connect_to_address_timeout;                  //!< Desired time in microseconds that is to be requested to our host machine for waiting for the HM-10 BT device's Connect-To-Address Response after sending a Connect-To-Address Command to it (i.e., for establishing a Bluetooth Connection between out HM-10 BT Device and a remote BT Device). @note The maximum time that a Bluetooth Connection can be made with an HM-10 BT Device is 11 seconds.
    uint32_t payload_size;                                     //!< Length in bytes of specifically an ETX OTA Custom Data Payload. @note This field can have any value whenever not receiving an ETX OTA Custom Data Payload since the program will ignore this value in that case.
    uint16_t flash_page_size_in_bytes;			               //!< Flash Memory page size in bytes as defined by the MCU/MPU with which the Serial Port communication is to be established with.
    uint16_t ETX_bl_page_size;                                 //!< Number of Flash Memory pages that have been designated for the Bootloader Firmware of the MCU/MPU with which the Serial Port communication is to be established with.
    uint16_t ETX_app_page_size;                                //!< Number of Flash Memory pages that have been designated for the Application Firmware of the MCU/MPU with which the Serial Port communication is to be established with.
    uint16_t payload_max_file_path_length;                     //!< Maximum File Path length in bytes for the Payload that the user wants the @ref etx_ota_protocol_host program to send to the external device with which the Serial Port communication has been established with.
    char payload_path_or_data[PAYLOAD_PATH_OR_DATA_MAX_SIZE];  //!< This field will stand for the Payload Path whenever a Firmware Image is given via the @ref ETX_OTA_API_t::ETX_OTA_Payload_Type parameter. However, this field will correspond to the actual data for whenever an ETX OTA Custom Data Payload is given instead in that parameter.
    char bt_addr[HM10_BT_ADDR_SIZE];                           //!< Bluetooth Address, without the colon characters, of the Remote Bluetooth Device with which it is desired to establish a Bluetooth Connection with.
    uint8_t rs232_mode_data_bits;                              //!< Chosen Data-bits string with which we want the host to run the RS232 protocol. @details The valid data-bits in the Teuniz RS232 Library are '5', '6', '7' and '8'.
    uint8_t rs232_mode_parity;                                 //!< Chosen Parity string with which we want the host to run the RS232 protocol. @details The valid parity values in the Teuniz RS232 Library are:<br> - 'N' = None (i.e., No parity bit sent at all).<br> - 'O' = Odd (i.e., Set to mark if the number of 1s in data bits is odd.<br> - 'E' = Even (i.e., Set to space if the number of 1s in data bits is even.
    uint8_t rs232_mode_stopbits;                               //!< Chosen Stop-bit string with which we want the host to run the RS232 protocol. @note The possible stopbit values in RS232 are '1' or '2'.
    uint8_t rs232_is_flow_control;                             //!< Chosen Flow Control decimal value to indicate with a 1 that we want the host to run the RS232 protocol with Flow Control enabled, or otherwise with a decimal value of 0 to indicate to the host to not run the RS232 protocol with Flow Control.
} ETX_OTA_API_t;

/**@brief   Sends some desired ETX OTA Payload Data to a specified device by using the ETX OTA Protocol.
 *
 * @param[in] p_ETX_OTA_api Should hold all the data received via the API of the @ref main_program .
 *
 * @retval  ETX_OTA_EC_OK
 * @retval  ETX_OTA_EC_NA
 * @retval  ETX_OTA_EC_INV_CMD_LINE_ARG
 * @retval  ETX_OTA_EC_UNRECOG_PAYLOAD
 * @retval  ETX_OTA_EC_OPEN_COM_ERR
 * @retval  ETX_OTA_EC_OPEN_FILE_ERR
 * @retval  ETX_OTA_EC_READ_FILE_ERR
 * @retval  ETX_OTA_EC_START_CMD_SEND_DATA_ERR
 * @retval  ETX_OTA_EC_START_CMD_NACK_RESP
 * @retval  ETX_OTA_EC_HEADER_PCKT_SEND_DATA_ERR
 * @retval  ETX_OTA_EC_HEADER_PCKT_NACK_RESP
 * @retval  ETX_OTA_EC_DATA_PCKT_SEND_DATA_ERR
 * @retval  ETX_OTA_EC_DATA_PCKT_NACK_RESP
 * @retval  ETX_OTA_EC_END_CMD_SEND_DATA_ERR
 * @retval  ETX_OTA_EC_END_CMD_NACK_RESP
 * @retval  ETX_OTA_EC_ABORT_CMD_SEND_DATA_ERR
 * @retval  ETX_OTA_EC_ABORT_CMD_NACK_RESP
 * @retval  ETX_OTA_EC_ABORT_LOOP_ERR
 * @retval  ETX_OTA_EC_BLE_INIT_ERR
 * @retval  ETX_OTA_EC_BLE_AT_CMD_ERR
 * @retval  ETX_OTA_EC_BLE_TYPE_CMD_ERR
 * @retval  ETX_OTA_EC_BLE_RESET_CMD_ERR
 * @retval  ETX_OTA_EC_BLE_CONN_CMD_ERR
 *
 * @note    For more details on the returned values, see @ref ETX_OTA_Status .
 *
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @date    November 28, 2023.
 */
ETX_OTA_Status start_etx_ota_process(ETX_OTA_API_t *p_ETX_OTA_api);

#endif /* INC_ETX_OTA_PROTOCOL_HOST_H_ */

/** @} */
