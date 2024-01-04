/** @addtogroup bl_side_etx_ota
 * @{
 */

#include "bl_side_etx_ota.h"
#include <stdio.h>	// Library from which "printf()" is located at.
#include <string.h>	// Library from which "memset()" is located at.
#include <stdbool.h> // Library from which the "bool" type is located at.

#define ETX_OTA_SOF  				(0xAA)    		/**< @brief Designated Start Of Frame (SOF) byte to indicate the start of an ETX OTA Packet. */
#define ETX_OTA_EOF  				(0xBB)    		/**< @brief Designated End Of Frame (EOF) byte to indicate the end of an ETX OTA Packet. */
#define ETX_OTA_SOF_SIZE			(1U)			/**< @brief	Designated SOF field size in bytes in a ETX OTA Packet. */
#define ETX_OTA_PACKET_TYPE_SIZE	(1U)			/**< @brief	Designated Packet Type field size in bytes in a ETX OTA Packet. */
#define ETX_OTA_DATA_LENGTH_SIZE	(2U)			/**< @brief	Designated Data Length field size in bytes in a ETX OTA Packet. */
#define ETX_OTA_DATA_MAX_SIZE 		(1024U)  		/**< @brief Designated maximum expected "Data" field's size in the General Data Format of an ETX OTA Packet. @note This definition's value does not stand for the size of the entire ETX OTA Packet. Instead, it represents the size of the "Data" field that is inside the General Data Format of an ETX OTA Packet. */
#define ETX_OTA_CRC32_SIZE			(4U)			/**< @brief	Designated 32-bit CRC field size in bytes in a ETX OTA Packet. */
#define ETX_OTA_EOF_SIZE			(1U)			/**< @brief	Designated EOF field size in bytes in a ETX OTA Packet. */
#define ETX_OTA_DATA_OVERHEAD 		(ETX_OTA_SOF_SIZE + ETX_OTA_PACKET_TYPE_SIZE + ETX_OTA_DATA_LENGTH_SIZE + ETX_OTA_CRC32_SIZE + ETX_OTA_EOF_SIZE)  	/**< @brief Data overhead in bytes of an ETX OTA Packet, which represents the bytes of an ETX OTA Packet except for the ones that it has at the Data field. */
#define ETX_OTA_PACKET_MAX_SIZE 	(ETX_OTA_DATA_MAX_SIZE + ETX_OTA_DATA_OVERHEAD)																		/**< @brief Total bytes in an ETX OTA Packet. */
#define ETX_OTA_DATA_FIELD_INDEX	(ETX_OTA_SOF_SIZE + ETX_OTA_PACKET_TYPE_SIZE + ETX_OTA_DATA_LENGTH_SIZE) 											/**< @brief Index position of where the Data field bytes of a ETX OTA Packet starts at. */
#define ETX_OTA_BL_FW_SIZE          (FLASH_PAGE_SIZE_IN_BYTES * ETX_BL_FLASH_PAGES_SIZE)   	/**< @brief Maximum size allowable for a Bootloader Firmware Image to have. */
#define ETX_OTA_APP_FW_SIZE         (FLASH_PAGE_SIZE_IN_BYTES * ETX_APP_FLASH_PAGES_SIZE)   /**< @brief Maximum size allowable for an Application Firmware Image to have. */

/**@brief	ETX OTA process states.
 *
 * @details	The ETX OTA process states are used in the functions of the @ref bl_side_etx_ota module to either indicate
 * 			or identify in what part of the whole ETX OTA process is our MCU/MPU currently at.
 * @details	The ETX OTA process consists of several sub-processes or states that must be given place in the following
 *          orderly fashion:
 *			<ol>
 *				<li>Idle State</li>
 *				<li>Start State</li>
 *				<li>Header State</li>
 *				<li>Data State</li>
 *				<li>End State</li>
 *			</ol>
 *
 * @note	If the ETX OTA process gives place in a different order as shown above (e.g., Idle State --> Start State -->
 *          Data State ..., where instead, the order of the states expected were Idle State --> Start State --> Header
 *          State ...), then the ETX OTA process will be terminated with the corresponding @ref ETX_OTA_Status Exception
 *          code.
 */
typedef enum
{
	ETX_OTA_STATE_IDLE    = 0U,   	//!< ETX OTA Idle State. @details This state stands for when our MCU/MPU is not in a ETX OTA Firmware Update.
	ETX_OTA_STATE_START   = 1U,   	//!< ETX OTA Start State. @details This state gives place when our MCU/MPU receives a Command Type Packet from the host right after leaving the ETX OTA Idle State. Our MCU/MPU will expect that Packet to contain the Start Command so that our MCU/MPU starts an ETX OTA Firmware Update.
	ETX_OTA_STATE_HEADER  = 2U,   	//!< ETX OTA Header State. @details This state starts right after the Command Type Packet of the ETX OTA Start State is processed. In the ETX OTA Header State, our MCU/MPU will expect to receive a Header Type Packet in which our MCU/MPU will obtain the size in bytes of the Firmware Image to be received, its recorded 32-bits CRC and the sub-type of the ETX OTA Data Type Packets to be received (i.e., @ref ETX_OTA_Payload_t ).
	ETX_OTA_STATE_DATA    = 3U,   	//!< ETX OTA Data State. @details This state starts right after the Header Type Packet of the ETX OTA Header State is processed. In the ETX OTA Data State, our MCU/MPU will expect to receive one or more Data Type Packets from which our MCU/MPU will receive a Firmware Image from the host. It is also in this State where our MCU/MPU will apply that Firmware Image to itself.
	ETX_OTA_STATE_END     = 4U		//!< ETX OTA End State. @details This state starts right after the Data Type Packet(s) of the ETX OTA Data State is/are processed. In the ETX OTA End State, our MCU/MPU will expect to receive a Command Type Packet containing the End Command so that our MCU/MPU confirms the conclusion of the ETX OTA Firmware Update.
} ETX_OTA_State;

/**@brief	Packet Type definitions available in the ETX OTA Firmware Update process.
 */
typedef enum
{
	ETX_OTA_PACKET_TYPE_CMD       = 0U,   	//!< ETX OTA Command Type Packet. @details This Packet Type is expected to be send by the host to our MCU/MPU to request a certain ETX OTA Command to our MCU/MPU (see @ref ETX_OTA_Command ).
	ETX_OTA_PACKET_TYPE_DATA      = 1U,   	//!< ETX OTA Data Type Packet. @details This Packet Type will contain either the full or a part/chunk of a Firmware Image being send from the host to our MCU/MPU.
	ETX_OTA_PACKET_TYPE_HEADER    = 2U,   	//!< ETX OTA Header Type Packet. @details This Packet Type will provide the size in bytes of the Firmware Image that our MCU/MPU will receive, its recorded 32-bits CRC and the sub-type of the ETX OTA Data Type Packets to be received (i.e., @ref ETX_OTA_Payload_t ).
	ETX_OTA_PACKET_TYPE_RESPONSE  = 3U		//!< ETX OTA Response Type Packet. @details This Packet Type contains a response from our MCU/MPU that is given to the host to indicate to it whether or not our MCU/MPU was able to successfully process the latest request or Packet from the host.
} ETX_OTA_Packet_t;

/**@brief	ETX OTA Commands definitions.
 *
 * @details	These are the different Commands that the host can request to our MCU/MPU whenever the host sends an ETX OTA
 *          Command Type Packet to that external device.
 */
typedef enum
{
	ETX_OTA_CMD_START = 0U,		    //!< ETX OTA Firmware Update Start Command. @details This command indicates to the MCU/MPU that the host is connected to (via @ref COMPORT_NUMBER ) to start an ETX OTA Process.
	ETX_OTA_CMD_END   = 1U,    		//!< ETX OTA Firmware Update End command. @details This command indicates to the MCU/MPU that the host is connected to (via @ref COMPORT_NUMBER ) to end the current ETX OTA Process.
	ETX_OTA_CMD_ABORT = 2U    		//!< ETX OTA Abort Command. @details This command is used by the host to request to our MCU/MPU to abort whatever ETX OTA Process that our MCU/MPU is working on. @note Unlike the other Commands, this one can be legally requested to our MCU/MPU at any time and as many times as the host wants to.
} ETX_OTA_Command;

/**@brief	Payload Type definitions available in the ETX OTA Firmware Update process.
 *
 * @details	Whenever the host sends to our MCU/MPU a @ref ETX_OTA_PACKET_TYPE_DATA Packet Type (i.e., a Data Type
 * 			Packet), there are several possible sub-types for this and any other Data Type Packet that indicate the type
 * 			of data that our MCU/MPU should expect to receive from the whole Data Type Packets that it will receive.
 * 			These sub-types are given by @ref ETX_OTA_Payload_t .
 */
typedef enum
{
    ETX_OTA_Application_Firmware_Image  = 0U,   	//!< ETX OTA Application Firmware Image Data Packet Type.
	ETX_OTA_Bootloader_Firmware_Image   = 1U,  		//!< ETX OTA Bootloader Firmware Image Data Packet Type.
	ETX_OTA_Custom_Data                 = 2U   		//!< ETX OTA Custom Data Packet Type.
} ETX_OTA_Payload_t;

/**@brief	Response Status definitions available in the ETX OTA Firmware Update process.
 *
 * @details	Whenever the host sends to our MCU/MPU a Packet, depending on whether our MCU/MPU was able to successfully
 * 			process the data of that Packet of not, our MCU/MPU will respond back to the host with a Response Type
 * 			Packet containing the value of a Response Status correspondingly.
 */
typedef enum
{
	ETX_OTA_ACK  = 0U,   		//!< Acknowledge (ACK) data byte used in an ETX OTA Response Type Packet to indicate to the host that the latest ETX OTA Packet has been processed successfully by our MCU/MPU.
	ETX_OTA_NACK   = 1U   		//!< Not Acknowledge (NACK) data byte used in an ETX OTA Response Type Packet to indicate to the host that the latest ETX OTA Packet has not been processed successfully by our MCU/MPU.
} ETX_OTA_Response_Status;

static uint8_t Rx_Buffer[ETX_OTA_PACKET_MAX_SIZE];			    /**< @brief Global buffer that will be used by our MCU/MPU to hold the whole data of a received ETX OTA Packet from the host. */
static ETX_OTA_State etx_ota_state = ETX_OTA_STATE_IDLE;	    /**< @brief Global variable used to hold the ETX OTA Process State at which our MCU/MPU is currently at. */
static uint32_t etx_ota_fw_received_size = 0;				    /**< @brief Global variable used to indicate the Total Size in bytes of the whole ETX OTA Payload that our MCU/MPU has received and written into the Flash Memory designated to the ETX OTA Protocol. */
static firmware_update_config_data_t *p_fw_config;			    /**< @brief Global pointer to the latest data of the @ref firmware_update_config sub-module. */
static UART_HandleTypeDef *p_huart;							    /**< @brief Our MCU/MPU's Hardware Protocol UART Handle from which the ETX OTA Protocol will be used on. */
static ETX_OTA_hw_Protocol ETX_OTA_hardware_protocol;           /**< @brief Hardware Protocol into which the ETX OTA Protocol will be used for sending/receiving data to/from the host. */
static HM10_GPIO_def_t *p_GPIO_is_hm10_default_settings = NULL; /**< @brief Pointer to the GPIO Definition Type of the GPIO Pin from which it can be requested to reset the Configuration Settings of the HM-10 BT Device to its default settings. @details This Input Mode GPIO will be used so that our MCU can know whether the user wants it to set the default configuration settings in the HM-10 BT Device. @note The following are the possible values of the GPIO Pin designated here:<br><br>* 0 (i.e., Low State) = Do not reset/change the configuration settings of the HM-10 BT Device.<br>* 1 (i.e., High State) = User requests to reset the configuration settings of the HM-10 BT Device to its default settings. */

/**@brief	ETX OTA Command Type Packet's parameters structure.
 *
 * @details	This structure contains all the fields of an ETX OTA Packet of @ref ETX_OTA_PACKET_TYPE_CMD Type.
 * @details	The following illustrates the ETX OTA format of this Type of Packets:
 *
 * <table style="border-collapse:collapse;border-spacing:0;background-color:#ffffff;text-align:center;vertical-align:top;">
 * <thead>
 *   <tr>
 *     <th style="border-color:black;border-style:solid;border-width:1px">SOF</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Packet<br>Type</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Len</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Command</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">CRC</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">EOF</th>
 *   </tr>
 * </thead>
 * <tbody>
 *   <tr>
 *     <td style="border-color:#ffffff">1B</td>
 *     <td style="border-color:#ffffff">1B</td>
 *     <td style="border-color:#ffffff">2B</td>
 *     <td style="border-color:#ffffff">1B</td>
 *     <td style="border-color:#ffffff">4B</td>
 *     <td style="border-color:#ffffff">1B</td>
 *   </tr>
 * </tbody>
 * </table>
 *
 * where B = byte(s).
 */
typedef struct __attribute__ ((__packed__)) {
	uint8_t   sof;				//!< Start of Frame (SOF). @details All ETX OTA Packets must start with a SOF byte, whose value is @ref ETX_OTA_SOF .
	uint8_t   packet_type;		//!< Packet Type. @details The value of this parameter has to be @ref ETX_OTA_PACKET_TYPE_CMD because this is meant to be used for Command Type Packets.
	uint16_t  data_len;			//!< Length of the Command value in bytes. @note The length of any @ref ETX_OTA_Command value is 1 byte.
	uint8_t   cmd;				//!< Command value. @note See @ref ETX_OTA_Command for more details.
	uint32_t  crc;				//!< 32-bit CRC of the \c cmd parameter of this structure.
	uint8_t   eof;				//!< Start of Frame (EOF). @details All ETX OTA Packets must end with an EOF byte, whose value is given by @ref ETX_OTA_EOF .
} ETX_OTA_Command_Packet_t;

/**@brief	Header Data parameters structure.
 *
 * @details	This structure contains all the fields of the Header data that is expected to be received by our MCU/MPU
 * 			in a @ref ETX_OTA_PACKET_TYPE_HEADER Type Packet (i.e., in a Header Type Packet).
 */
typedef struct __attribute__ ((__packed__)) {
	uint32_t 	package_size;		//!< Total length/size in bytes of the data expected to be received by our MCU/MPU from the host via all the @ref ETX_OTA_PACKET_TYPE_DATA Type Packet(s) (i.e., in a Data Type Packet or Packets) to be received.
	uint32_t 	package_crc;		//!< 32-bit CRC of the whole data to be obtained from all the @ref ETX_OTA_PACKET_TYPE_DATA Type Packets (i.e., in a Data Type Packets).
	uint32_t 	reserved1;			//!< 32-bits reserved for future changes on this firmware.
	uint16_t 	reserved2;			//!< 16-bits reserved for future changes on this firmware.
	uint8_t 	reserved3;			//!< 8-bits reserved for future changes on this firmware.
	uint8_t		payload_type;	    //!< Expected payload type to be received from the @ref ETX_OTA_PACKET_TYPE_DATA Type Packets (i.e., in a Data Type Packets). @note see @ref ETX_OTA_Payload_t to learn about the available Payload Types.
} header_data_t;

/**@brief	ETX OTA Header Type Packet's parameters structure.
 *
 * @details	This structure contains all the fields of an ETX OTA Packet of @ref ETX_OTA_STATE_HEADER Type.
 * @details	The following illustrates the ETX OTA format of this Type of Packets:
 *
 * <table style="border-collapse:collapse;border-spacing:0;background-color:#ffffff;text-align:center;vertical-align:top;">
 * <thead>
 *   <tr>
 *     <th style="border-color:black;border-style:solid;border-width:1px">SOF</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Packet<br>Type</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Len</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Header<br>Data</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">CRC</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">EOF</th>
 *   </tr>
 * </thead>
 * <tbody>
 *   <tr>
 *     <td style="border-color:#ffffff">1B</td>
 *     <td style="border-color:#ffffff">1B</td>
 *     <td style="border-color:#ffffff">2B</td>
 *     <td style="border-color:#ffffff">16B</td>
 *     <td style="border-color:#ffffff">4B</td>
 *     <td style="border-color:#ffffff">1B</td>
 *   </tr>
 * </tbody>
 * </table>
 *
 * where B = byte(s).
 */
typedef struct __attribute__ ((__packed__)) {
	uint8_t     	sof;			//!< Start of Frame (SOF). @details All ETX OTA Packets must start with a SOF byte, whose value is @ref ETX_OTA_SOF .
	uint8_t     	packet_type;	//!< Packet Type. @details The value of this parameter has to be @ref ETX_OTA_PACKET_TYPE_HEADER because this is meant to be used for Header Type Packets.
	uint16_t    	data_len;		//!< Length of the \c meta_data parameter in bytes. @note The length of the \c meta_data parameter value must be 16 bytes.
	header_data_t	meta_data;		//!< Header data. @note See @ref header_data_t for more details.
	uint32_t    	crc;			//!< 32-bit CRC of the \c meta_data parameter of this structure.
	uint8_t     	eof;			//!< Start of Frame (EOF). @details All ETX OTA Packets must end with an EOF byte, whose value is given by @ref ETX_OTA_EOF .
} ETX_OTA_Header_Packet_t;

/**@brief	ETX OTA Data Type Packet's parameters structure.
 *
 * @details	This structure contains all the fields of an ETX OTA Packet of @ref ETX_OTA_STATE_DATA Type.
 * @details	The following illustrates the ETX OTA format of this Type of Packets:
 *
 * <table style="border-collapse:collapse;border-spacing:0;background-color:#ffffff;text-align:center;vertical-align:top;">
 * <thead>
 *   <tr>
 *     <th style="border-color:black;border-style:solid;border-width:1px">SOF</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Packet<br>Type</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Len</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Payload Data</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">CRC</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">EOF</th>
 *   </tr>
 * </thead>
 * <tbody>
 *   <tr>
 *     <td style="border-color:#ffffff">1B</td>
 *     <td style="border-color:#ffffff">1B</td>
 *     <td style="border-color:#ffffff">2B</td>
 *     <td style="border-color:#ffffff">@ref data_len B</td>
 *     <td style="border-color:#ffffff">4B</td>
 *     <td style="border-color:#ffffff">1B</td>
 *   </tr>
 * </tbody>
 * </table>
 *
 * where B = byte(s) and where the value of the "Payload Data" field must be perfectly divisible by 4 bytes.
 *
 * @note	The CRC is not within the fields of this structure because this structure was formulated in such a way that
 *          it could represent any possible ETX OTA Data Type Packet in consideration that they may vary in size and
 *          because it is desired to make it compatible with the General Data Format explained in @ref bl_side_etx_ota .
 *          Therefore, the strategy that has been applied in the @ref bl_side_etx_ota module is to append all the data
 *          contained in the @ref data field from all the received ETX OTA Data Type Packets within a single ETX OTA
 *          Process to then calculate the CRC of that whole data and validate it with the one received for that whole
 *          data via the @ref header_data_t::package_crc field.
 * @note	Another consequence of what was just explained is that the EOF is also not within the fields of this
 *          structure but it is always validated at the end of each ETX OTA Data Type Packet received.
 */
typedef struct __attribute__ ((__packed__)) {
	uint8_t     sof;			//!< Start of Frame (SOF). @details All ETX OTA Packets must start with a SOF byte, whose value is @ref ETX_OTA_SOF .
	uint8_t     packet_type;	//!< Packet Type. @details The value of this parameter must be @ref ETX_OTA_PACKET_TYPE_DATA because this is meant to be used for Data Type Packets.
	uint16_t    data_len;		//!< Length of the actual "Data" field contained inside the \c data parameter in bytes (i.e., This does not take into account the "CRC32" and "EOF" fields length). @note This length is defined by the host, but its maximum value can be @ref ETX_OTA_DATA_MAX_SIZE (i.e., 1024 bytes).
	uint8_t     *data;			//!< Payload data, 32-bit CRC and EOF. @details the Data bytes may have a length of 1 byte up to a maximum of @ref ETX_OTA_DATA_MAX_SIZE (i.e., 1024 bytes). Next to them, the 32-bit CRC will be appended and then the EOF byte will be appended subsequently. @details The appended 32-bit CRC is calculated with respect to the Data bytes received in for this single/particular ETX OTA Data Type Packet (i.e., Not for the whole data previously explained).
} ETX_OTA_Data_Packet_t;

/**@brief	ETX OTA Response Type Packet's parameters structure.
 *
 * @details	This structure contains all the fields of an ETX OTA Packet of @ref ETX_OTA_PACKET_TYPE_RESPONSE Type.
 * @details	The following illustrates the ETX OTA format of this Type of Packets:
 *
 * <table style="border-collapse:collapse;border-spacing:0;background-color:#ffffff;text-align:center;vertical-align:top;">
 * <thead>
 *   <tr>
 *     <th style="border-color:black;border-style:solid;border-width:1px">SOF</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Packet<br>Type</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Len</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">Status</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">CRC</th>
 *     <th style="border-color:black;border-style:solid;border-width:1px">EOF</th>
 *   </tr>
 * </thead>
 * <tbody>
 *   <tr>
 *     <td style="border-color:#ffffff">1B</td>
 *     <td style="border-color:#ffffff">1B</td>
 *     <td style="border-color:#ffffff">2B</td>
 *     <td style="border-color:#ffffff">1B</td>
 *     <td style="border-color:#ffffff">4B</td>
 *     <td style="border-color:#ffffff">1B</td>
 *   </tr>
 * </tbody>
 * </table>
 *
 * where B = byte(s).
 */
typedef struct __attribute__ ((__packed__)) {
	uint8_t   sof;				//!< Start of Frame (SOF). @details All ETX OTA Packets must start with a SOF byte, whose value is @ref ETX_OTA_SOF .
	uint8_t   packet_type;		//!< Packet Type. @details The value of this parameter must be @ref ETX_OTA_PACKET_TYPE_RESPONSE because this is meant to be used for Response Type Packets.
	uint16_t  data_len;			//!< Length of the \c status parameter in bytes. @note The length of any valid value for the \c status parameter is 1 byte.
	uint8_t   status;			//!< Response Status value. @note See @ref ETX_OTA_Response_Status for more details.
	uint32_t  crc;				//!< 32-bit CRC of the \c status parameter of this structure.
	uint8_t   eof;				//!< Start of Frame (EOF). @details All ETX OTA Packets must end with an EOF byte, whose value is given by @ref ETX_OTA_EOF .
} ETX_OTA_Response_Packet_t;

/**
 * @brief   Gets one Packet from the ETX OTA process, if any is given.
 *
 * @details The General Data Format that is expected to be received is as explained in the Doxygen documentation of the
 *          @ref ETX_OTA_Data_Packet_t parameters structure, which starts with a SOF byte that is followed up with the
 *          Packet Type field, the Data Length field, the Data field, the CRC32 field and ends up with an EOF byte.
 *
 * @note	The HAL Timeout for each reception of data is given by @ref ETX_CUSTOM_HAL_TIMEOUT .
 *
 * @param[in, out] buf 		Buffer pointer into which the bytes of the received packet from the host, will be stored.
 * @param max_len 			Maximum length of bytes that are expected to be received from the "Data" field of the next
 * 							ETX OTA packet.
 *
 * @retval					ETX_OTA_EC_OK
 * @retval					ETX_OTA_EC_NR
 * @retval					ETX_OTA_EC_ERR
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date October 01, 2023.
 */
static ETX_OTA_Status etx_ota_receive_packet(uint8_t *buf, uint16_t max_len);

/**@brief	Processes and validates the latest received ETX OTA Packet.
 *
 * @details	This function will read the current value of the @ref etx_ota_state global variable to determine at which
 *          ETX OTA State the process is currently at in order to process the latest ETX OTA Packet correspondingly. In
 *          addition, this function will modify the value of that variable to the next ETX OTA State whenever it
 *          corresponds.
 *
 * @details In addition, this function will do the following depending on the current ETX OTA State:
 *          <ul>
 *              <li><b>ETX OTA Idle State:</b> Do nothing.</li>
 *              <li><b>ETX OTA Start State:</b> Validate the latest ETX OTA Packet to be that of an ETX OTA Command Type
 *                                              Packet containing the Start Command in it and, if successful, then
 *                                              change current ETX OTA State to ETX OTA Header State.</li>
 *              <li><b>ETX OTA Header State:</b> Validate the latest ETX OTA Packet to be that of an ETX OTA Header
 *                                               Type Packet and, if successful, then validate the incoming Firmware
 *                                               Image to be that of an Application Firmware Image. If successful, then
 *                                               write that data into the @ref firmware_update_config sub-module. If
 *                                               everything goes well up to this point, then change the current ETX OTA
 *                                               State to ETX OTA Data State.</li>
 *              <li><b>ETX OTA Data State:</b> Validate the latest ETX OTA Packet to be that of an ETX OTA Data Type
 *                                             Packet and, if successful, then write the contents of the "Data" field
 *                                             of that Packet into the corresponding Flash Memory location address of
 *                                             our MCU/MPU's Application Firmware. Repeat this explained process for all
 *                                             the ETX OTA Data Type Packets to be received by our MCU/MPU. After the
 *                                             last ETX OTA Data Packet has been successfully, change the current ETX
 *                                             OTA State to ETX OTA End State.</li>
 *              <li><b>ETX OTA End State:</b> Validate the latest ETX OTA Packet to be that of an ETX OTA Command Type
 *                                            Packet containing the End Command in it and, if successful, then validate
 *                                            the CRC of the Application Firmware Image that has just been installed. If
 *                                            this is successful, then change the current ETX OTA State to ETX OTA Idle
 *                                            State.</li>
 *          </ul>
 *
 * @note	Each time the implementer/programmer calls this function, the @ref etx_ota_receive_packet function must to
 * 			be called once before this @ref etx_ota_process_data function.
 *
 * @param[in] buf	Buffer pointer to the data of the latest ETX OTA Packet.
 *
 * @retval						ETX_OTA_EC_OK
 * @retval						ETX_OTA_EC_STOP
 * @retval						ETX_OTA_EC_NR
 * @retval						ETX_OTA_EC_NA
 * @retval						ETX_OTA_EC_ERR
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date October 02, 2023.
 */
static ETX_OTA_Status etx_ota_process_data(uint8_t *buf);

/**@brief	Sends an ETX OTA Response Type Packet with a desired Response Status (i.e., ACK or NACK) to the host either
 *          via the UART or the BT Hardware Protocol correspondingly.
 *
 * @note    This function decides on sending the data on a certain Hardware Protocol according to the current value of
 *          @ref ETX_OTA_hardware_protocol , which should be set only via the @ref init_firmware_update_module function.
 *
 * @param response_status	\c ETX_OTA_ACK or \c ETX_OTA_NACK .
 *
 * @retval  ETX_OTA_EC_OK
 * @retval 	ETX_OTA_EC_NR
 * @retval 	ETX_OTA_EC_ERR
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date October 30, 2023.
 */
static ETX_OTA_Status etx_ota_send_resp(ETX_OTA_Response_Status response_status);

/**@brief	Write the contents of the "Data" field contained in a given ETX OTA Data Type Packet into the Flash Memory
 *          of our MCU/MPU's Application Firmware.
 *
 * @param[in] data			Pointer to the "Data" field of a given ETX OTA Data Type Packet, whose data wants to be
 *                          written into the Flash Memory of our MCU/MPU's Application Firmware.
 * @param data_len			Length in bytes of the "Data" field of the ETX Data Type Packet that is being pointed
 *                          towards to, via the \p data param.
 * @param is_first_block	Flag used to indicate whether the current ETX OTA Data Type Packet contains the first Flash
 *                          Memory block with a \c true or otherwise with a \c false .
 *
 * @retval 					ETX_OTA_EC_OK
 * @retval 					ETX_OTA_EC_NR
 * @retval 					ETX_OTA_EC_ERR
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @date October 11, 2023.
 */
static ETX_OTA_Status write_data_to_flash_app(uint8_t *data, uint16_t data_len, bool is_full_image);

/**@brief	Gets the corresponding @ref ETX_OTA_Status value depending on the given @ref HAL_StatusTypeDef value.
 *
 * @param HAL_status	HAL Status value (see @ref HAL_StatusTypeDef ) that wants to be converted into its equivalent
 * 						of a @ref ETX_OTA_Status value.
 *
 * @retval				ETX_OTA_EC_NR if \p HAL_status param equals \c HAL_BUSY or \c ETX_CUSTOM_HAL_TIMEOUT .
 * @retval				ETX_OTA_EC_ERR if \p HAL_status param equals \c HAL_ERROR .
 * @retval				HAL_status param otherwise.
 *
 * @note	For more details on the returned values listed, see @ref ETX_OTA_Status and @ref HAL_StatusTypeDef .
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date September 26, 2023.
 */
static ETX_OTA_Status HAL_ret_handler(HAL_StatusTypeDef HAL_status);

ETX_OTA_Status init_firmware_update_module(ETX_OTA_hw_Protocol hardware_protocol,
											UART_HandleTypeDef *huart,
											firmware_update_config_data_t *fw_config,
											HM10_GPIO_def_t *GPIO_is_hm10_default_settings_Pin)
{
    /** <b>Local variable ret:</b> Used to hold the exception code value returned by either a @ref FirmUpdConf_Status , a @ref ETX_OTA_Status or a @ref HM10_Status function type. */
    uint16_t ret;
    #if ETX_OTA_VERBOSE
        printf("Initializing the Firmware Update Module...\r\n");
    #endif

    /* Persist the requested hardware protocol into which the ETX OTA Protocol will be used on. */
    ETX_OTA_hardware_protocol = hardware_protocol;

    /* Persist the pointer to the UART into which the ETX OTA Protocol will be used on. */
    p_huart = huart;

    /* Persist the pointer to the Firmware Update Configurations sub-module to the one that was given. */
    p_fw_config = fw_config;

    /* Persist the pointer to the GPIO Definition Type of the GPIO Pin from which it can be requested to reset the Configuration Settings of the HM-10 BT Device to its default settings. */
    p_GPIO_is_hm10_default_settings = GPIO_is_hm10_default_settings_Pin;

    /* Validate the requested hardware protocol to be used and, if required, initialized it. */
    switch (hardware_protocol)
    {
        case ETX_OTA_hw_Protocol_UART:
			#if ETX_OTA_VERBOSE
				printf("The UART Hardware Protocol has been selected by the Firmware Update Module.\r\n");
			#endif
            break;
        case ETX_OTA_hw_Protocol_BT:
			#if ETX_OTA_VERBOSE
				printf("The BT Hardware Protocol has been selected by the Firmware Update Module.\r\n");
			#endif

            /* Initializing the HM-10 Bluetooth module. */
			#if ETX_OTA_VERBOSE
				printf("Initializing the HM-10 Bluetooth module...\r\n");
			#endif
            init_hm10_module(p_huart);

            /* Resetting the Configuration Settings of the HM-10 BT Device to its Default Settings, but only if user is requesting it. */
            if (HAL_GPIO_ReadPin(p_GPIO_is_hm10_default_settings->GPIO_Port, p_GPIO_is_hm10_default_settings->GPIO_Pin) == GPIO_PIN_SET)
            {
				#if ETX_OTA_VERBOSE
					printf("MCU has been requested to reset the configuration settings of the HM-10 BT Device to its default settings.\r\n");
					printf("Resetting configuration settings of the HM-10 BT Device...\r\n");
				#endif

				/* Sending test command to HM-10 BT Device to make sure that it is not in a Bluetooth Connection for the next steps to be made and to make sure that it is currently in working condition. */
				ret = disconnect_hm10_from_bt_address();
				if (ret == HM10_BT_Connection_Status_Unknown)
				{
					#if ETX_OTA_VERBOSE
						printf("ERROR: Something went wrong whenever sending the initial Test Command sent to the HM-10 BT Device(Bluetooth Connection Status Code returned = %d).\r\n", ret);
					#endif
					return ETX_OTA_EC_ERR;
				}

                /* Restore all the HM-10 Setup values to factory setup. */
                ret = send_hm10_renew_cmd();
                if (ret != HM10_EC_OK)
                {
					#if ETX_OTA_VERBOSE
						printf("ERROR: The HM-10 BT device could not be restored to its factory setup via the AT+RENEW Command (Exception Code = %d).\r\n", ret);
					#endif
                    return ETX_OTA_EC_ERR;
                }

                /* Setting BT Name in HM-10 BT Device. */
                /** <b>Local variable default_ble_name:</b> Used to hold the Default BT Name of the HM-10 BT Device as given by @ref HM10_DEFAULT_BLE_NAME . */
                uint8_t default_ble_name[] = {HM10_DEFAULT_BLE_NAME};
                ret = set_hm10_name(default_ble_name, sizeof(default_ble_name));
                if (ret != HM10_EC_OK)
                {
					#if ETX_OTA_VERBOSE
						printf("ERROR: The BT Name of the HM-10 BT device could not be set to its default value (Exception Code = %d).\r\n", ret);
					#endif
                    return ETX_OTA_EC_ERR;
                }

                /* Setting Role in BT Device. */
                ret = set_hm10_role(HM10_DEFAULT_ROLE);
                if (ret != HM10_EC_OK)
                {
					#if ETX_OTA_VERBOSE
						printf("ERROR: The Role of the HM-10 BT device could not be set to its default value (Exception Code = %d).\r\n", ret);
					#endif
                    return ETX_OTA_EC_ERR;
                }

                /* Setting a Pin in the BT Device. */
                /** <b>Local variable default_pin_code:</b> Used to hold the Default Pin Code of the HM-10 BT Device as given by @ref HM10_DEFAULT_PIN . */
                uint8_t default_pin_code[HM10_PIN_VALUE_SIZE] = {HM10_DEFAULT_PIN};
                ret = set_hm10_pin(default_pin_code);
                if (ret != HM10_EC_OK)
                {
					#if ETX_OTA_VERBOSE
						printf("ERROR: The Pin of the HM-10 BT device could not be set to its default value (Exception Code = %d).\r\n", ret);
					#endif
                    return ETX_OTA_EC_ERR;
                }

                /* Setting a Pin Code Mode in the BT Device. */
                ret = set_hm10_pin_code_mode(HM10_DEFAULT_PIN_CODE_MODE);
                if (ret != HM10_EC_OK)
                {
					#if ETX_OTA_VERBOSE
						printf("ERROR: The Pin Code Mode of the HM-10 BT device could not be set to its default value (Exception Code = %d).\r\n", ret);
					#endif
                    return ETX_OTA_EC_ERR;
                }

                /* Setting the Module Work Mode in the BT Device. */
                ret = set_hm10_module_work_mode(HM10_DEFAULT_MODULE_WORK_MODE);
                if (ret != HM10_EC_OK)
                {
					#if ETX_OTA_VERBOSE
						printf("ERROR: The Module Work Mode of the HM-10 BT device could not be set to its default value (Exception Code = %d).\r\n", ret);
					#endif
                    return ETX_OTA_EC_ERR;
                }

                /* Resetting the BT Device. */
                ret = send_hm10_reset_cmd();
                if (ret != HM10_EC_OK)
                {
					#if ETX_OTA_VERBOSE
						printf("ERROR: Could not reset the HM-10 BT device (Exception Code = %d).\r\n", ret);
					#endif
                    return ETX_OTA_EC_ERR;
                }
				#if ETX_OTA_VERBOSE
					printf("The reset of the configuration settings of the HM-10 BT Device has been successfully completed.\r\n");
				#endif
            }
			#if ETX_OTA_VERBOSE
				printf("Initialization of the HM-10 Bluetooth module has been completed successfully.\r\n");
			#endif
            break;
        default:
			#if ETX_OTA_VERBOSE
				printf("ERROR: The requested Hardware Protocol %d is not recognized by the ETX OTA Protocol.\r\n", hardware_protocol);
			#endif
            return ETX_OTA_EC_ERR;
    }

    return ETX_OTA_EC_OK;
}

ETX_OTA_Status firmware_image_download_and_install()
{
	/** <b>Local variable ret:</b> Used to hold the exception code value returned by a @ref FirmUpdConf_Status or a @ref ETX_OTA_Status function type. */
	ETX_OTA_Status ret;

	/* Reset the global variables related to: 1) The Header data of a received Firmware Image and 2) The ETX OTA Process State. */
	etx_ota_fw_received_size = 0U;
	etx_ota_state            = ETX_OTA_STATE_START;

	/* Attempt to receive a Firmware Image from the host and, if applicable, install it. */
	#if ETX_OTA_VERBOSE
		printf("Waiting for a Firmware Image from the host...\r\n");
	#endif
	do
	{
		#if ETX_OTA_VERBOSE
			printf("Waiting for an ETX OTA Packet from the host...\r\n");
		#endif
		ret = etx_ota_receive_packet(Rx_Buffer, ETX_OTA_PACKET_MAX_SIZE);
		switch (ret)
		{
		  case ETX_OTA_EC_OK:
			/* Since the ETX OTA Packet was received successfully, proceed into processing that data correspondingly. */
			ret = etx_ota_process_data(Rx_Buffer);
			switch (ret)
			{
			  case ETX_OTA_EC_OK:
				  #if ETX_OTA_VERBOSE
				  	  printf("DONE: The current ETX OTA Packet was processed successfully. Therefore, sending ACK...\r\n");
				  #endif
				  etx_ota_send_resp(ETX_OTA_ACK);
				  break;
			  case ETX_OTA_EC_STOP:
				  #if ETX_OTA_VERBOSE
				  	  printf("DONE: The ETX OTA process has been requested to be stopped by the host. Therefore, sending ACK...\r\n");
				  #endif
				  etx_ota_send_resp(ETX_OTA_ACK);
				  return ETX_OTA_EC_STOP;
			  case ETX_OTA_EC_NR:
				  #if ETX_OTA_VERBOSE
				  	  printf("ERROR: Our MCU/MPU's HAL responded with a No Response ETX OTA Exception code during a part of the process where this was not expected.\r\n");
				  #endif
				  return ETX_OTA_EC_ERR;
			  case ETX_OTA_EC_NA:
				  #if ETX_OTA_VERBOSE
				  	  printf("WARNING: The host has requested to start a Bootloader Firmware Update. Therefore, sending NACK...\r\n");
				  #endif
				  etx_ota_send_resp(ETX_OTA_NACK);
				  return ETX_OTA_EC_NA;
			  case ETX_OTA_EC_ERR:
				  #if ETX_OTA_VERBOSE
				  	  printf("ERROR: An Error Exception Code has been generated during the ETX OTA process. Therefore, sending NACK...\r\n");
				  #endif
				  etx_ota_send_resp(ETX_OTA_NACK);
				  return ETX_OTA_EC_ERR;
			  default:
				  /* This should not happen. */
				  #if ETX_OTA_VERBOSE
				  	  printf("ERROR: The ETX OTA Exception code %d that has been generated is unrecognized by our MCU/MPU. Therefore, sending NACK...\r\n", ret);
				  #endif
				  return ret;
			}
			break;

		  case ETX_OTA_EC_NR:
			  #if ETX_OTA_VERBOSE
			  	  printf("DONE: No response from host.\r\n");
			  #endif
			  return ETX_OTA_EC_NR;

		  case ETX_OTA_EC_ERR:
			  #if ETX_OTA_VERBOSE
			  	  printf("ERROR: An Error Exception Code has been generated during the ETX OTA process. Therefore, sending NACK...\r\n");
			  #endif
			  etx_ota_send_resp(ETX_OTA_NACK);
			  return ETX_OTA_EC_ERR;

		  default:
			  /* The "default" case should not be called. */
			  #if ETX_OTA_VERBOSE
			  	  printf("ERROR: The ETX OTA Exception code %d that has been generated either should not have been generated or is unrecognized by our MCU/MPU. Therefore, sending NACK...\r\n", ret);
			  #endif
			  etx_ota_send_resp(ETX_OTA_NACK);
			  return ret;
		}
	}
	while (etx_ota_state != ETX_OTA_STATE_IDLE);

	return ETX_OTA_EC_OK;
}

static ETX_OTA_Status etx_ota_receive_packet(uint8_t *buf, uint16_t max_len)
{
	/** <b>Local variable ret:</b> Used to hold the exception code value returned by either a @ref FirmUpdConf_Status , a @ref ETX_OTA_Status or a @ref HM10_Status function type. */
	int16_t  ret;
	/** <b>Local variable len:</b> Current index against which the bytes of the current ETX OTA Packet have been fetched. */
	uint16_t len = 0;
	/** <b>Local variable data_len:</b> "Data Length" field value of the ETX OTA Packet that is currently being received via the chosen Hardware Protocol. @details	The value of this field should stand for the length in bytes of the current ETX OTA Packet's "Data" field. */
	uint16_t data_len;
	/** <b>Local variable cal_data_crc:</b> Our MCU/MPU's calculated CRC of the ETX OTA Packet received via the chosen Hardware Protocol. */
	uint32_t cal_data_crc;
	/** <b>Local variable rec_data_crc:</b> Value holder of the "Recorded CRC" contained in the ETX OTA Packet received via the chosen Hardware Protocol. */
	uint32_t rec_data_crc;

	#if ETX_OTA_VERBOSE
		printf("Waiting to receive an ETX OTA Packet from the host...\r\n");
	#endif
	switch (ETX_OTA_hardware_protocol)
	{
		case ETX_OTA_hw_Protocol_UART:
			/* Wait to receive the first byte of data from the host and validate it to be the SOF byte of an ETX OTA Packet. */
			ret = HAL_UART_Receive(p_huart, &buf[len], ETX_OTA_SOF_SIZE, ETX_CUSTOM_HAL_TIMEOUT);
			ret = HAL_ret_handler(ret);
			if (ret != HAL_OK)
			{
				return ret;
			}
			if (buf[len++] != ETX_OTA_SOF)
			{
				#if ETX_OTA_VERBOSE
					printf("ERROR: Expected to receive the SOF field value from the current ETX OTA Packet.\r\n");
				#endif
				return ETX_OTA_EC_ERR;
			}

			/* Wait to receive the next 1-byte of data from the host and validate it to be a "Packet Type" field value of an ETX OTA Packet. */
			ret = HAL_UART_Receive(p_huart, &buf[len], ETX_OTA_PACKET_TYPE_SIZE, ETX_CUSTOM_HAL_TIMEOUT);
			ret = HAL_ret_handler(ret);
			if (ret != HAL_OK)
			{
				return ret;
			}
			switch (buf[len++])
			{
				case ETX_OTA_PACKET_TYPE_CMD:
				case ETX_OTA_PACKET_TYPE_DATA:
				case ETX_OTA_PACKET_TYPE_HEADER:
				case ETX_OTA_PACKET_TYPE_RESPONSE:
					break;
				default:
					#if ETX_OTA_VERBOSE
						printf("ERROR: The data received from the Packet Type field of the currently received ETX OTA Packet contains a value not recognized by our MCU/MPU.\r\n");
					#endif
					return ETX_OTA_EC_ERR;
			}

			/* Wait to receive the next 2-bytes of data from the host, which our MCU/MPU will interpret as the "Data Length" field value of an ETX OTA Packet. */
			ret = HAL_UART_Receive(p_huart, &buf[len], ETX_OTA_DATA_LENGTH_SIZE, ETX_CUSTOM_HAL_TIMEOUT);
			ret = HAL_ret_handler(ret);
			if (ret != HAL_OK)
			{
				return ret;
			}
			data_len = *(uint16_t *)&buf[len];
			len += ETX_OTA_DATA_LENGTH_SIZE;

			/* Wait to receive the next \c data_len bytes of data from the host, which our MCU/MPU will interpret as the "Data" field value of an ETX OTA Packet. */
			for (uint16_t i=0; i<data_len; i++)
			{
				ret = HAL_UART_Receive(p_huart, &buf[len++], 1, ETX_CUSTOM_HAL_TIMEOUT);
				ret = HAL_ret_handler(ret);
				if (ret != HAL_OK)
				{
					return ret;
				}
			}

			/* Wait to receive the next 4-bytes of data from the host, which our MCU/MPU will interpret as the "CRC32" field value of an ETX OTA Packet. */
			ret = HAL_UART_Receive(p_huart, &buf[len], ETX_OTA_CRC32_SIZE, ETX_CUSTOM_HAL_TIMEOUT);
			ret = HAL_ret_handler(ret);
			if (ret != HAL_OK)
			{
				return ret;
			}
			rec_data_crc = *(uint32_t *) &buf[len];
			len += ETX_OTA_CRC32_SIZE;

			/* Wait to receive the next 1-byte of data from the host and validate it to be a "EOF" field value of an ETX OTA Packet. */
			ret = HAL_UART_Receive(p_huart, &buf[len], ETX_OTA_EOF_SIZE, ETX_CUSTOM_HAL_TIMEOUT);
			ret = HAL_ret_handler(ret);
			if (ret != HAL_OK)
			{
				return ret;
			}
			break;
		case ETX_OTA_hw_Protocol_BT:
			/* Wait to receive the first byte of data from the host and validate it to be the SOF byte of an ETX OTA Packet. */
			ret = get_hm10_ota_data(&buf[len], ETX_OTA_SOF_SIZE, ETX_CUSTOM_HAL_TIMEOUT);
			if (ret != HAL_OK)
			{
				return ret;
			}
			if (buf[len++] != ETX_OTA_SOF)
			{
				#if ETX_OTA_VERBOSE
					printf("ERROR: Expected to receive the SOF field value from the current ETX OTA Packet.\r\n");
				#endif
				return ETX_OTA_EC_ERR;
			}

			/* Wait to receive the next 1-byte of data from the host and validate it to be a "Packet Type" field value of an ETX OTA Packet. */
			ret = get_hm10_ota_data(&buf[len], ETX_OTA_PACKET_TYPE_SIZE, ETX_CUSTOM_HAL_TIMEOUT);
			if (ret != HAL_OK)
			{
				return ret;
			}
			switch (buf[len++])
			{
				case ETX_OTA_PACKET_TYPE_CMD:
				case ETX_OTA_PACKET_TYPE_DATA:
				case ETX_OTA_PACKET_TYPE_HEADER:
				case ETX_OTA_PACKET_TYPE_RESPONSE:
					break;
				default:
					#if ETX_OTA_VERBOSE
						printf("ERROR: The data received from the Packet Type field of the currently received ETX OTA Packet contains a value not recognized by our MCU/MPU.\r\n");
					#endif
					return ETX_OTA_EC_ERR;
			}

			/* Wait to receive the next 2-bytes of data from the host, which our MCU/MPU will interpret as the "Data Length" field value of an ETX OTA Packet. */
			ret = get_hm10_ota_data(&buf[len], ETX_OTA_DATA_LENGTH_SIZE, ETX_CUSTOM_HAL_TIMEOUT);
			if (ret != HAL_OK)
			{
				return ret;
			}
			data_len = *(uint16_t *)&buf[len];
			len += ETX_OTA_DATA_LENGTH_SIZE;

			/* Wait to receive the next \c data_len bytes of data from the host, which our MCU/MPU will interpret as the "Data" field value of an ETX OTA Packet. */
			for (uint16_t i=0; i<data_len; i++)
			{
				ret = get_hm10_ota_data(&buf[len++], 1, ETX_CUSTOM_HAL_TIMEOUT);
				if (ret != HAL_OK)
				{
					return ret;
				}
			}

			/* Wait to receive the next 4-bytes of data from the host, which our MCU/MPU will interpret as the "CRC32" field value of an ETX OTA Packet. */
			ret = get_hm10_ota_data(&buf[len], ETX_OTA_CRC32_SIZE, ETX_CUSTOM_HAL_TIMEOUT);
			if (ret != HAL_OK)
			{
				return ret;
			}
			rec_data_crc = *(uint32_t *) &buf[len];
			len += ETX_OTA_CRC32_SIZE;

			/* Wait to receive the next 1-byte of data from the host and validate it to be a "EOF" field value of an ETX OTA Packet. */
			ret = get_hm10_ota_data(&buf[len], ETX_OTA_EOF_SIZE, ETX_CUSTOM_HAL_TIMEOUT);
			if (ret != HAL_OK)
			{
				return ret;
			}
			break;
		default:
			/* This should not happen since it should have been previously validated. */
			#if ETX_OTA_VERBOSE
				printf("ERROR: Expected a Hardware Protocol value, but received something else: %d.\r\n", ETX_OTA_hardware_protocol);
			#endif
			return ETX_OTA_EC_ERR;
	}

	/* Validate that the latest byte received corresponds to an ETX OTA End of Frame (EOF) byte. */
	if (buf[len++] != ETX_OTA_EOF)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: Expected to receive the EOF field value from the current ETX OTA Packet.\r\n");
		#endif
		return ETX_OTA_EC_ERR;
	}

	/* Calculate the 32-bit CRC only with respect to the contents of the "Data" field from the current ETX OTA Packet that has just been received. */
	cal_data_crc = crc32_mpeg2(&buf[ETX_OTA_DATA_FIELD_INDEX], data_len);

	/* Validate that the Calculated CRC matches the Recorded CRC. */
	if (cal_data_crc != rec_data_crc)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: CRC mismatch with current ETX OTA Packet [Calculated CRC = 0x%08X] [Recorded CRC = 0x%08X]\r\n",
													   (unsigned int) cal_data_crc, (unsigned int) rec_data_crc);
		#endif
		return ETX_OTA_EC_ERR;
	}

	if (max_len < len)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: Received more data than expected (Expected = %d, Received = %d)\r\n", max_len, len);
		#endif
		return ETX_OTA_EC_ERR;
	}

	#if ETX_OTA_VERBOSE
		printf("ETX OTA Packet has been successfully received.\r\n");
	#endif
	return ETX_OTA_EC_OK;
}

static ETX_OTA_Status etx_ota_process_data(uint8_t *buf)
{
	/** <b>Local pointer cmd:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Command_Packet_t type. */
	ETX_OTA_Command_Packet_t *cmd = (ETX_OTA_Command_Packet_t *) buf;

	/* Check if we received the ETX OTA Abort Command and, if true, stop the ETX OTA process. */
	#if ETX_OTA_VERBOSE
		printf("Processing data of the latest ETX OTA Packet...\r\n");
	#endif
	if (cmd->packet_type == ETX_OTA_PACKET_TYPE_CMD)
	{
		if (cmd->cmd == ETX_OTA_CMD_ABORT)
		{
			#if ETX_OTA_VERBOSE
				printf("DONE: ETX OTA Abort command received. Stopping the process...\r\n");
			#endif
			return ETX_OTA_EC_STOP;
		}
	}

	switch (etx_ota_state)
	{
		case ETX_OTA_STATE_IDLE:
			#if ETX_OTA_VERBOSE
				printf("DONE: ETX OTA Process is in Idle State.\r\n");
			#endif
			return ETX_OTA_EC_OK;

		case ETX_OTA_STATE_START:
			if ((cmd->packet_type==ETX_OTA_PACKET_TYPE_CMD) && (cmd->cmd == ETX_OTA_CMD_START))
			{
				#if ETX_OTA_VERBOSE
					printf("DONE: Received ETX OTA Start Command.\r\n");
				#endif
				etx_ota_state = ETX_OTA_STATE_HEADER;
				return ETX_OTA_EC_OK;
			}
			#if ETX_OTA_VERBOSE
				printf("ERROR: Expected ETX OTA Command Type Packet containing an ETX OTA Start Command, but something else was received instead.\r\n");
			#endif
			return ETX_OTA_EC_ERR;

		case ETX_OTA_STATE_HEADER:
			/** <b>Local pointer header:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Header_Packet_t type. */
			ETX_OTA_Header_Packet_t *header = (ETX_OTA_Header_Packet_t *) buf;

			if (header->packet_type == ETX_OTA_PACKET_TYPE_HEADER)
			{
				/** <b>Local variable header_ret:</b> Return value of a @ref FirmUpdConf_Status function function type. */
				int16_t  header_ret;

				/* We validate that the Firmware Image to be received is either a Bootloader or an Application Firmware Image. */
				switch (header->meta_data.payload_type)
				{
					case ETX_OTA_Application_Firmware_Image:
						/* We validate the size of the Application Firmware Image to be received. */
						if (header->meta_data.package_size > ETX_OTA_APP_FW_SIZE)
						{
							#if ETX_OTA_VERBOSE
								printf("ERROR: The given Application Firmware Image (of size %ld) exceeds the maximum bytes allowed (which is %d).\r\n", header->meta_data.package_size, ETX_OTA_APP_FW_SIZE);
							#endif
							return ETX_OTA_EC_NA;
						}
						p_fw_config->is_bl_fw_stored_in_app_fw = BT_FW_NOT_STORED_IN_APP_FW;
						p_fw_config->is_bl_fw_install_pending = NOT_PENDING;
						break;
					case ETX_OTA_Bootloader_Firmware_Image:
						/* We validate the size of the Bootloader Firmware Image to be received. */
						if (header->meta_data.package_size > ETX_OTA_BL_FW_SIZE)
						{
							#if ETX_OTA_VERBOSE
								printf("ERROR: The given Bootloader Firmware Image (of size %ld) exceeds the maximum bytes allowed (which is %d).\r\n", header->meta_data.package_size, ETX_OTA_BL_FW_SIZE);
							#endif
							return ETX_OTA_EC_NA;
						}
						p_fw_config->is_bl_fw_stored_in_app_fw = BT_FW_STORED_IN_APP_FW;
						p_fw_config->is_bl_fw_install_pending = IS_PENDING;
						break;
					case ETX_OTA_Custom_Data:
						#if ETX_OTA_VERBOSE
							printf("WARNING: Received an ETX OTA Custom Data request.\r\n");
						#endif
						return ETX_OTA_EC_NA;
					default:
						#if ETX_OTA_VERBOSE
							printf("WARNING: A Firmware Image was expected to be received from the host, but a different request was received instead.\r\n");
						#endif
						return ETX_OTA_EC_NA;
				}

				/* We write the newly received Firmware Image Header data into a new data block of the Flash Memory designated to the @ref firmware_update_config sub-module. */
				p_fw_config->App_fw_size = header->meta_data.package_size;
				p_fw_config->App_fw_rec_crc = header->meta_data.package_crc;
				header_ret = firmware_update_configurations_write(p_fw_config);
				if (header_ret != FIRM_UPDT_CONF_EC_OK)
				{
					#if ETX_OTA_VERBOSE
						printf("EXCEPTION CODE %d: The data was not written into the Firmware Update Configurations sub-module.\r\n", header_ret);
					#endif
					return header_ret;
				}

				#if ETX_OTA_VERBOSE
					printf("Received ETX OTA Header with a Firmware Size of %ld bytes.\r\n", p_fw_config->App_fw_size);
				#endif
				etx_ota_state = ETX_OTA_STATE_DATA;
				return ETX_OTA_EC_OK;
			}
			#if ETX_OTA_VERBOSE
				printf("ERROR: Expected ETX OTA Header Type Packet, but something else was received instead.\r\n");
			#endif
			return ETX_OTA_EC_ERR;

		case ETX_OTA_STATE_DATA:
			/** <b>Local pointer data:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Data_Packet_t type. */
			ETX_OTA_Data_Packet_t *data = (ETX_OTA_Data_Packet_t *) buf;
			/** <b>Local variable data_ret:</b> Return value of a @ref ETX_OTA_Status function. */
			uint16_t data_ret;

			if (data->packet_type == ETX_OTA_PACKET_TYPE_DATA)
			{
				/* Validate that the Payload received from the current ETX OTA Packet is perfectly divisible by 4 bytes (i.e., one word). */
				if ((data->data_len)%4 != 0)
				{
					#if ETX_OTA_VERBOSE
						printf("ERROR: The size of the currently received Payload is not perfectly divisible by 4 bytes (i.e., one word).\r\n");
					#endif
					return ETX_OTA_EC_ERR;
				}

				/* Write the ETX OTA Data Type Packet to the Flash Memory location of the Application Firmware. */
				data_ret = write_data_to_flash_app(buf+ETX_OTA_DATA_FIELD_INDEX, data->data_len, etx_ota_fw_received_size==0);
				data_ret = HAL_ret_handler(data_ret);

				if (data_ret != HAL_OK)
				{
					return data_ret;
				}
				#if ETX_OTA_VERBOSE
					if (p_fw_config->is_bl_fw_install_pending == IS_PENDING)
					{
						if (p_fw_config->App_fw_size%ETX_OTA_DATA_MAX_SIZE == 0)
						{
							printf("[%ld/%ld] parts of the Bootloader Firmware Image are now stored into the Flash Memory designated to the Application Firmware Image...\r\n", etx_ota_fw_received_size/ETX_OTA_DATA_MAX_SIZE, p_fw_config->App_fw_size/ETX_OTA_DATA_MAX_SIZE);
						}
						else
						{
							if (etx_ota_fw_received_size%ETX_OTA_DATA_MAX_SIZE == 0)
							{
								printf("[%ld/%ld] parts of the Bootloader Firmware Image are now stored into the Flash Memory designated to the Application Firmware Image...\r\n", etx_ota_fw_received_size/ETX_OTA_DATA_MAX_SIZE, p_fw_config->App_fw_size/ETX_OTA_DATA_MAX_SIZE+1);
							}
							else
							{
								printf("[%ld/%ld] parts of the Bootloader Firmware Image are now stored into the Flash Memory designated to the Application Firmware Image...\r\n", (etx_ota_fw_received_size/ETX_OTA_DATA_MAX_SIZE+1), p_fw_config->App_fw_size/ETX_OTA_DATA_MAX_SIZE+1);
							}
						}
					}
					else
					{
						if (p_fw_config->App_fw_size%ETX_OTA_DATA_MAX_SIZE == 0)
						{
							printf("[%ld/%ld] parts of the Application Firmware Image are now installed into our MCU/MPU...\r\n", etx_ota_fw_received_size/ETX_OTA_DATA_MAX_SIZE, p_fw_config->App_fw_size/ETX_OTA_DATA_MAX_SIZE);
						}
						else
						{
							if (etx_ota_fw_received_size%ETX_OTA_DATA_MAX_SIZE==0)
							{
								printf("[%ld/%ld] parts of the Application Firmware Image are now installed into our MCU/MPU...\r\n", etx_ota_fw_received_size/ETX_OTA_DATA_MAX_SIZE, p_fw_config->App_fw_size/ETX_OTA_DATA_MAX_SIZE+1);
							}
							else
							{
								printf("[%ld/%ld] parts of the Application Firmware Image are now installed into our MCU/MPU...\r\n", (etx_ota_fw_received_size/ETX_OTA_DATA_MAX_SIZE+1), p_fw_config->App_fw_size/ETX_OTA_DATA_MAX_SIZE+1);
							}
						}
					}
				#endif
				if (etx_ota_fw_received_size >= p_fw_config->App_fw_size)
				{
					/* received the full data. Therefore, move to the End State of the ETX OTA Process. */
					etx_ota_state = ETX_OTA_STATE_END;
				}
				return ETX_OTA_EC_OK;
			}
			#if ETX_OTA_VERBOSE
				printf("ERROR: Expected ETX OTA Data Type Packet, but something else was received instead.\r\n");
			#endif
			return ETX_OTA_EC_ERR;

		case ETX_OTA_STATE_END:
			if ((cmd->packet_type==ETX_OTA_PACKET_TYPE_CMD) && (cmd->cmd==ETX_OTA_CMD_END))
			{
				/** <b>Local variable cal_crc:</b> Value holder for the calculated 32-bit CRC of the Application Firmware Image that has just been installed into our MCU/MPU. */
				uint32_t cal_crc = crc32_mpeg2((uint8_t *) ETX_APP_FLASH_ADDR, p_fw_config->App_fw_size);

				/* Validate the 32-bit CRC of the whole Application Firmware Image. */
				#if ETX_OTA_VERBOSE
					printf("Received ETX OTA END Command.\r\n");
					if (p_fw_config->is_bl_fw_install_pending == IS_PENDING)
					{
						printf("Validating the received Bootloader Firmware Image...\r\n");
					}
					else
					{
						printf("Validating the received Application Firmware Image...\r\n");
					}
				#endif
				if (cal_crc != p_fw_config->App_fw_rec_crc)
				{
					#if ETX_OTA_VERBOSE
						if (p_fw_config->is_bl_fw_install_pending == IS_PENDING)
						{
							printf("The 32-bit CRC of the received Bootloader Firmware Image mismatches with the calculated one: [Calculated CRC = 0x%08X] [Recorded CRC = 0x%08X]\r\n",
															(unsigned int) cal_crc, (unsigned int) p_fw_config->App_fw_rec_crc);
						}
						else
						{
							printf("The 32-bit CRC of the installed Application Firmware Image mismatches with the calculated one: [Calculated CRC = 0x%08X] [Recorded CRC = 0x%08X]\r\n",
															(unsigned int) cal_crc, (unsigned int) p_fw_config->App_fw_rec_crc);
						}
					#endif
					return ETX_OTA_EC_ERR;
				}
				#if ETX_OTA_VERBOSE
					if (p_fw_config->is_bl_fw_install_pending == IS_PENDING)
					{
						printf("DONE: 32-bit CRC of the received Bootloader Firmware Image has been successfully validated.\r\n");
					}
					else
					{
						printf("DONE: 32-bit CRC of the installed Application Firmware Image has been successfully validated.\r\n");
					}
				#endif
				etx_ota_state = ETX_OTA_STATE_IDLE;
				return ETX_OTA_EC_OK;
			}
			#if ETX_OTA_VERBOSE
				printf("ERROR: Expected ETX OTA Command Type Packet containing an ETX OTA End Command, but something else was received instead.\r\n");
			#endif
			return ETX_OTA_EC_ERR;

		default:
			/* Should not come here */
			#if ETX_OTA_VERBOSE
				printf("ERROR: The current ETX OTA State %d is unrecognized by our MCU/MPU.\r\n", etx_ota_state);
			#endif
			return ETX_OTA_EC_ERR;
			break;
	}
}

//#pragma GCC diagnostic ignored "-Wstringop-overflow=" // This pragma definition will tell the compiler to ignore an expected Compilation Warning (due to a code functionality that it is strictly needed to work that way) that gives using the HAL_CRC_Calculate() function inside the etx_ota_send_resp() function,. which states the following: 'HAL_CRC_Calculate' accessing 4 bytes in a region of size 1.
static ETX_OTA_Status etx_ota_send_resp(ETX_OTA_Response_Status response_status)
{
	/** <b>Local variable ret:</b> Return value of a @ref ETX_OTA_Status function function type. */
	ETX_OTA_Status  ret;

	ETX_OTA_Response_Packet_t response =
	{
		.sof         	= ETX_OTA_SOF,
		.packet_type 	= ETX_OTA_PACKET_TYPE_RESPONSE,
		.data_len    	= 1U,
		.status      	= response_status,
		.crc			= 0U,
		.eof         	= ETX_OTA_EOF
	};
	response.crc = crc32_mpeg2((uint8_t *) &response.status, 1);

	switch (ETX_OTA_hardware_protocol)
	{
		case ETX_OTA_hw_Protocol_UART:
			ret = HAL_UART_Transmit(p_huart, (uint8_t *) &response, sizeof(ETX_OTA_Response_Packet_t), ETX_CUSTOM_HAL_TIMEOUT);
			ret = HAL_ret_handler(ret);
			break;
		case ETX_OTA_hw_Protocol_BT:
			ret = send_hm10_ota_data((uint8_t *) &response, sizeof(ETX_OTA_Response_Packet_t), ETX_CUSTOM_HAL_TIMEOUT);
			break;
		default:
			/* This should not happen since it should have been previously validated. */
			#if ETX_OTA_VERBOSE
				printf("ERROR: Expected a Hardware Protocol value, but received something else: %d.\r\n", ETX_OTA_hardware_protocol);
			#endif
			return ETX_OTA_EC_ERR;
	}

	return ret;
}

static ETX_OTA_Status write_data_to_flash_app(uint8_t *data, uint16_t data_len, bool is_first_block)
{
	/** <b>Local variable ret:</b> Return value of a @ref ETX_OTA_Status function function type. */
	uint8_t  ret;
	/**	<b>Local variable p_data:</b> Pointer to the data at which the \p data param points to but in \c uint32_t Type. */
	uint32_t *p_data = (uint32_t *) data;

	/* Unlock the Flash Memory of our MCU/MPU. */
	ret = HAL_FLASH_Unlock();
	ret = HAL_ret_handler(ret);
	if(ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: HAL Flash could not be unlocked; ETX OTA Exception code %d.\r\n", ret);
		#endif
		return ret;
	}

	/* Erase Flash Memory dedicated to our MCU/MPU's Application Firmware but only if the current ETX OTA Data Type Packet being processed is the first one. */
	if (is_first_block)
	{
		#if ETX_OTA_VERBOSE
			printf("Erasing the Flash Memory pages designated to the Application Firmware of our MCU/MPU...\r\n");
		#endif
		FLASH_EraseInitTypeDef EraseInitStruct;
		uint32_t page_error;

		EraseInitStruct.TypeErase    = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.Banks        = FLASH_BANK_1;
		EraseInitStruct.PageAddress  = ETX_APP_FLASH_ADDR;
		EraseInitStruct.NbPages      = ETX_APP_FLASH_PAGES_SIZE;

		ret = HAL_FLASHEx_Erase(&EraseInitStruct, &page_error);
		ret = HAL_ret_handler(ret);
		if (ret != HAL_OK)
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: Flash Memory pages of the Application Firmware of our MCU/MPU could not be erased; ETX OTA Exception code %d.\r\n", ret);
			#endif
			return ret;
		}
		#if ETX_OTA_VERBOSE
			printf("DONE: Flash Memory pages designated to the Application Firmware of our MCU/MPU have been successfully erased.\r\n");
		#endif
	}

	/**	<b>Local variable word_data:</b> Array of 4 bytes (i.e., 1 word) initialized with the zeros (i.e., 0x00 in each byte) to then overwrite them if needed with the remaining bytes of the last word from the Application Firmware Image. */
	uint8_t word_data[4] = {0x00, 0x00, 0x00, 0x00};
	/** <b>Local variable data_len_minus_one_word:</b> Holds the length in bytes of the "Data" field from the current ETX OTA Data Type Packet, except for the last four bytes. */
	uint16_t data_len_minus_one_word = data_len - 4;
	/** <b>Local variable remaining_data_len_of_last_word:</b> Should contain the remaining bytes of the last word from the Application Firmware Image that are pending to be written into our MCU/MPU's Flash Memory. */
	uint8_t remaining_data_len_of_last_word;
	/**	<b>Local variable bytes_flashed:</b> Indicator of how many bytes of the current ETX OTA Packet's Payload have been written into the designated Flash Memory of the Application Firmware. */
	uint16_t bytes_flashed = 0;

	if (data_len > 4)
	{
		/* Write the entire Application Firmware Image into our MCU/MPU's Flash Memory, except for the last word (i.e., the last four bytes). */
		for ( ; bytes_flashed<data_len_minus_one_word; bytes_flashed+=4)
		{
			ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
									(ETX_APP_FLASH_ADDR + etx_ota_fw_received_size),
									p_data[bytes_flashed/4]);
			ret = HAL_ret_handler(ret);
			if (ret == HAL_OK)
			{
				etx_ota_fw_received_size += 4;
			}
			else
			{
				#if ETX_OTA_VERBOSE
					printf("EXCEPTION CODE %d: The Firmware Image data was not successfully written into our MCU/MPU.\r\n", ret);
				#endif
				return ret;
			}
		}

		/* Populate the remaining bytes of the Application Firmware Image that is currently being written into our MCU/MPU's Flash Memory via the \c word_data variable. */
		// NOTE: This way, if there are unused bytes in the \c word _data variable after filling it with the remaining bytes of the Application Firmware Image, these will be left with the values that the Flash Memory recognizes as Reset Values.
		remaining_data_len_of_last_word = data_len - bytes_flashed;
		for (uint8_t i=0; i<remaining_data_len_of_last_word; i++)
		{
			word_data[i] = data[bytes_flashed + i];
		}
	}
	else
	{
		/* Populate the remaining bytes of the Application Firmware Image that is currently being written into our MCU/MPU's Flash Memory via the \c word_data variable. */
		// NOTE: This way, if there are unused bytes in the \c word _data variable after filling it with the remaining bytes of the Application Firmware Image, these will be left with the values that the Flash Memory recognizes as Reset Values.
		remaining_data_len_of_last_word = data_len;
		for (uint8_t i=0; i<remaining_data_len_of_last_word; i++)
		{
			word_data[i] = data[i];
		}
	}

	/* Write the remaining bytes of the Application Firmware Image into the Flash Memory designated pages to our MCU/MPU's Application Firmware. */
	ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
							(ETX_APP_FLASH_ADDR + etx_ota_fw_received_size),
							*((uint32_t *) word_data));
	ret = HAL_ret_handler(ret);
	if (ret == HAL_OK)
	{
		bytes_flashed += remaining_data_len_of_last_word;
		etx_ota_fw_received_size += remaining_data_len_of_last_word;
	}
	else
	{
		#if ETX_OTA_VERBOSE
			printf("EXCEPTION CODE %d: The Firmware Image data was not successfully written into our MCU/MPU.\r\n", ret);
		#endif
		return ret;
	}

	/* Lock the Flash Memory, just like it originally was before calling this @ref write_data_to_flash_app function. */
	ret = HAL_FLASH_Lock();
	ret = HAL_ret_handler(ret);
	if(ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: HAL Flash could not be locked; ETX OTA Exception code %d.\r\n", ret);
		#endif
	}

	return ret;
}

static ETX_OTA_Status HAL_ret_handler(HAL_StatusTypeDef HAL_status)
{
  switch (HAL_status)
    {
  	  case HAL_BUSY:
	  case HAL_TIMEOUT:
		return ETX_OTA_EC_NR;
	  case HAL_ERROR:
		return ETX_OTA_EC_ERR;
	  default:
		return HAL_status;
    }
}

/** @} */
