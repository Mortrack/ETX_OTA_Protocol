/** @addtogroup etx_ota_protocol_host
 * @{
 */

#include "etx_ota_protocol_host.h"
#include "RS232/rs232.h" // Library for using RS232 protocol.
#include "etx_ota_config.h" // Custom Library used for configuring the ETX OTA protocol.
#include <stdlib.h>
#include <stdio.h>	// Library from which "printf()" is located at.
#include <stdbool.h> // Library from which the "bool" type is located at.
#include <unistd.h> // Library for using the "usleep()" function.
#include <stdarg.h>



/**@brief	LOG Messages Types Definitions.
 */
typedef enum
{
    DEBUG_t     = 0U,   		//!< Debug Log Message Type.
    INFO_t      = 1U,   		//!< Information Log Message Type.
    DONE_t      = 2U,           //!< Done Log Message Type.
    WARNING_t   = 3U,           //!< Warning Log Message Type.
    ERROR_t     = 4U            //!< Error Log Message Type.
} LOG_t;

/**@brief	ETX OTA process states.
 *
 * @details	The ETX OTA process states are used in the functions of the "ETX OTA Protocol for the external device"
 *          (connected to it via @ref COMPORT_NUMBER ) to either indicate or identify in what part of the whole ETX OTA
 *          process is that external device currently at.
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
	ETX_OTA_STATE_IDLE    = 0U,   	//!< ETX OTA Idle State. @details This state stands for when the external device (connected to it via @ref COMPORT_NUMBER ) is not in a ETX OTA Process.
	ETX_OTA_STATE_START   = 1U,   	//!< ETX OTA Start State. @details This state gives place when the external device (connected to it via @ref COMPORT_NUMBER ) receives a Command Type Packet from the host right after leaving the ETX OTA Idle State. That external device will expect that Packet to contain the Start Command so that this external device starts an ETX OTA Process.
	ETX_OTA_STATE_HEADER  = 2U,   	//!< ETX OTA Header State. @details This state starts right after the Command Type Packet of the ETX OTA Start State is processed. In the ETX OTA Header State, the external device (connected to it via @ref COMPORT_NUMBER ) will expect to receive a Header Type Packet in which this external device will obtain the size in bytes of the Payload to be received, its recorded 32-bits CRC and the Type of Payload to be received (i.e., @ref ETX_OTA_Payload_t ).
	ETX_OTA_STATE_DATA    = 3U,   	//!< ETX OTA Data State. @details This state starts right after the Header Type Packet of the ETX OTA Header State is processed. In the ETX OTA Data State, the external device (connected to it via @ref COMPORT_NUMBER ) will expect to receive one or more Data Type Packets from which this external device will receive a Payload from the host. In the case of receiving a Firmware Update Image, it is also in this State where this external device will install that Firmware Image to itself.
	ETX_OTA_STATE_END     = 4U		//!< ETX OTA End State. @details This state starts right after the Data Type Packet(s) of the ETX OTA Data State is/are processed. In the ETX OTA End State, the external device (connected to it via @ref COMPORT_NUMBER ) will expect to receive a Command Type Packet containing the End Command so that this external device confirms the conclusion of the ETX OTA Process.
} ETX_OTA_State;

/**@brief	Packet Type definitions available in the ETX OTA Protocol.
 */
typedef enum
{
    ETX_OTA_PACKET_TYPE_CMD       = 0U,   	//!< ETX OTA Command Type Packet. @details This Packet Type is expected to be send by the host to the external device (connected to it via @ref COMPORT_NUMBER ) to request a certain ETX OTA Command to that external device (see @ref ETX_OTA_Command ).
    ETX_OTA_PACKET_TYPE_DATA      = 1U,   	//!< ETX OTA Data Type Packet. @details This Packet Type will contain either the full or a part/chunk of a Payload being send from the host to our external device (connected to it via @ref COMPORT_NUMBER ).
    ETX_OTA_PACKET_TYPE_HEADER    = 2U,   	//!< ETX OTA Header Type Packet. @details This Packet Type will provide the size in bytes of the Payload that the external device (connected to it via @ref COMPORT_NUMBER ) will receive, its recorded 32-bits CRC and the Type of Pyaload data to be received (i.e., @ref ETX_OTA_Payload_t ).
    ETX_OTA_PACKET_TYPE_RESPONSE  = 3U		//!< ETX OTA Response Type Packet. @details This Packet Type contains a response from the external device (connected to it via @ref COMPORT_NUMBER ) that is given to the host to indicate to it whether or not that external device was able to successfully process the latest request or Packet from the host.
} ETX_OTA_Packet_t;

/**@brief	ETX OTA Commands definitions.
 *
 * @details	These are the different Commands that the host can request to the external device that it is connected to
 *          (via @ref COMPORT_NUMBER) whenever the host sends an ETX OTA Command Type Packet to that external device.
 */
typedef enum
{
    ETX_OTA_CMD_START = 0U,		    //!< ETX OTA Firmware Update Start Command. @details This command indicates to the MCU/MPU that the host is connected to (connected to to it via @ref COMPORT_NUMBER ) to start an ETX OTA Process.
    ETX_OTA_CMD_END   = 1U,    		//!< ETX OTA Firmware Update End command. @details This command indicates to the MCU/MPU that the host is connected to (connected to to it via @ref COMPORT_NUMBER ) to end the current ETX OTA Process.
    ETX_OTA_CMD_ABORT = 2U    		//!< ETX OTA Abort Command. @details This command is used by the host to request to the external device that the host is connected to (connected to to it via @ref COMPORT_NUMBER ) to abort whatever ETX OTA Process that external device is working on. @note Unlike the other Commands, this one can be legally requested to the external device at any time and as many times as the host wants to.
} ETX_OTA_Command;

/**@brief	Response Status definitions available in the ETX OTA Protocol.
 *
 * @details	Whenever the host sends to the external device (connected to it via @ref COMPORT_NUMBER ) a Packet,
 *          depending on whether that external device was able to successfully process the data of that Packet of not,
 *          this external device will respond back to the host with a Response Type Packet containing the value of a
 *          Response Status correspondingly.
 */
typedef enum
{
    ETX_OTA_ACK  = 0U,   		//!< Acknowledge (ACK) data byte used in an ETX OTA Response Type Packet to indicate to the host that the latest ETX OTA Packet has been processed successfully by the external device (connected to it via @ref COMPORT_NUMBER ).
    ETX_OTA_NACK   = 1U   		//!< Not Acknowledge (NACK) data byte used in an ETX OTA Response Type Packet to indicate to the host that the latest ETX OTA Packet has not been processed successfully by the external device (connected to it via @ref COMPORT_NUMBER ).
} ETX_OTA_Response_Status;

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
 * @details	This structure contains all the fields of the Header data that is expected to be received by our external
 *          device (connected to to it via @ref COMPORT_NUMBER ) in a @ref ETX_OTA_PACKET_TYPE_HEADER Type Packet (i.e.,
 *          in a Header Type Packet).
 */
typedef struct __attribute__ ((__packed__)) {
    uint32_t 	package_size;		//!< Total length/size in bytes of the data expected to be received by our external device (connected to it via @ref COMPORT_NUMBER ) from the host via all the @ref ETX_OTA_PACKET_TYPE_DATA Type Packet(s) (i.e., in a Data Type Packet or Packets) to be received.
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
 *          because it is desired to make it compatible with the General Data Format explained in @ref firmware_update .
 *          Therefore, the strategy that has been applied in the @ref firmware_update module is to append all the data
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
    uint8_t     *data;			//!< Paylaod data, 32-bit CRC and EOF. @details the Data bytes may have a length of 1 byte up to a maximum of @ref ETX_OTA_DATA_MAX_SIZE (i.e., 1024 bytes). Next to them, the 32-bit CRC will be appended and then the EOF byte will be appended subsequently. @details The appended 32-bit CRC is calculated with respect to the Data bytes received in for this single/particular ETX OTA Data Type Packet (i.e., Not for the whole data previously explained).
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

#define ETX_OTA_DATA_OVERHEAD 		    (ETX_OTA_SOF_SIZE + ETX_OTA_PACKET_TYPE_SIZE + ETX_OTA_DATA_LENGTH_SIZE + ETX_OTA_CRC32_SIZE + ETX_OTA_EOF_SIZE)  	/**< @brief Data overhead in bytes of an ETX OTA Packet, which represents the bytes of an ETX OTA Packet except for the ones that it has at the Data field. */
#define ETX_OTA_PACKET_MAX_SIZE 	    (ETX_OTA_DATA_MAX_SIZE + ETX_OTA_DATA_OVERHEAD)	                                                                    /**< @brief Total bytes in an ETX OTA Packet. */
#define ETX_OTA_DATA_FIELD_INDEX	    (ETX_OTA_SOF_SIZE + ETX_OTA_PACKET_TYPE_SIZE + ETX_OTA_DATA_LENGTH_SIZE)                                            /**< @brief Index position of where the Data field bytes of a ETX OTA Packet starts at. */
#define ETX_OTA_BL_FW_SIZE              (FLASH_PAGE_SIZE_IN_BYTES * ETX_BL_PAGE_SIZE)   /**< @brief Maximum size allowable for a Bootloader Firmware Image to have. */
#define ETX_OTA_APP_FW_SIZE             (FLASH_PAGE_SIZE_IN_BYTES * ETX_APP_PAGE_SIZE)  /**< @brief Maximum size allowable for an Application Firmware Image to have. */
#define ETX_OTA_MAX_FW_SIZE             (ETX_OTA_APP_FW_SIZE)                           /**< @brief Maximum size allowable for a Firmware Image to have. */
#define ETX_OTA_CMD_PACKET_T_SIZE       (sizeof(ETX_OTA_Command_Packet_t))              /**< @brief Length in bytes of the @ref ETX_OTA_Command_Packet_t struct. */
#define ETX_OTA_HEADER_DATA_T_SIZE      (sizeof(header_data_t))                         /**< @brief Length in bytes of the @ref header_data_t struct. */
#define ETX_OTA_HEADER_PACKET_T_SIZE    (sizeof(ETX_OTA_Header_Packet_t))               /**< @brief Length in bytes of the @ref ETX_OTA_Header_Packet_t struct. */
uint8_t payload_send_attempts = 0;                                                      /**< @brief Attempts that have been made to send a Payload to the external device (connected to it via @ref COMPORT_NUMBER ). @note This variable is used only to count the attempts of sending that but only whenever receiving a NACK Response Status from sending an ETX OTA Packet Type Packet to that external device after having sent either an ETX OTA Start Command or an ETX OTA Header Type Command. The reason for this is because if that happens, it is highly possible that this is due to that the external device was doing something else aside waiting to receive an ETX OTA Request from the host at that moment to be able to receive the desired payload from the host. */

static uint8_t ETX_OTA_Packet_Buffer[ETX_OTA_PACKET_MAX_SIZE];        /**< @brief Global buffer that will be used by our host machine to hold the whole data of either a received ETX OTA Packet from the external device (connected to it via @ref COMPORT_NUMBER ) or to populate in it the Packet's bytes to be send to that external device. */
static uint8_t PAYLOAD_CONTENT[ETX_OTA_MAX_FW_SIZE];                  /**< @brief Global holder for the Firmware Update Image file contents. */
static const uint32_t crc_table[0x100] = {
        0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
        0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75, 0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
        0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
        0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95, 0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,
        0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072, 0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
        0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02, 0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
        0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692, 0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
        0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2, 0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,
        0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB, 0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
        0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B, 0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
        0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B, 0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
        0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B, 0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
        0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C, 0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
        0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC, 0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
        0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C, 0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
        0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C, 0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4,
};                                                                    /**< @brief Global 32-bit CRC (MPEG-2) Lookup Table. */

/**@brief   Logger Message function with Debug, Information, Done, Warning and Error types available.
 *
 * @param log_type  Logger type that wants to be assigned to the messaged to be logged.
 * @param msg       Actual message that wants to be logged.
 *
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @author 	<a href=https://www.geeksforgeeks.org/how-to-write-your-own-printf-in-c/>Geeks for Geeks Website</a>
 * @date    October 06, 2023.
 */
static void LOG(LOG_t log_type, const char* msg, ...);

/**@brief   Calculates the 32-bit CRC of a given data.
 *
 * @param[in] p_data    Pointer to the data from which it is desired to calculate the 32-bit CRC.
 * @param DataLength    Length in bytes of the \p p_data param.
 *
 * @return              The calculated 32-bit CRC Hash Function on the input data towards which the \p p_data param
 *                      points to.
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 */
static uint32_t crc32_mpeg2(uint8_t *p_data, uint32_t DataLength);

/**@brief   Indicates whether the external device (connected to it via @ref COMPORT_NUMBER ) responded to our host
 *          machine with an ACK or a NACK Response Status.
 *
 * @param teuniz_rs232_lib_comport  The converted value of the actual comport that was requested by the user but into
 *                                  its equivalent for the @ref teuniz_rs232_library (For more details, see the Table
 *                                  from @ref teuniz_rs232_library ).
 *
 * @return                          \c true if there was both some data received from the external device (connected to
 *                                  it via @ref COMPORT_NUMBER ) to our host machine and where also that data contained
 *                                  an ETX OTA Response Type Packet containing an ACK Response Status. Otherwise,
 *                                  \c false if there was either no data received from the that external device to the
 *                                  host machine or that data contained a NACK Response Status. In addition to this,
 *                                  regardless of the cases previously explained, if the CRC validation fails, then a
 *                                  \c false value will be given.
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    October 06, 2023.
 */
static bool is_ack_resp_received(int teuniz_rs232_lib_comport);

/**@brief   Sends an ETX OTA Command Type Packet containing the Abort Command in it to the external device (connected to
 *          it via @ref COMPORT_NUMBER ).
 *
 * @details Sending an Abort Command to that external device will request to stop any ongoing ETX OTA Protocol process.
 *
 * @param teuniz_rs232_lib_comport  The converted value of the actual comport that was requested by the user but into
 *                                  its equivalent for the @ref teuniz_rs232_library (For more details, see the Table
 *                                  from @ref teuniz_rs232_library ).
 *
 * @retval  ETX_OTA_EC_OK
 * @retval 	ETX_OTA_EC_ERR
 *
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @date    November 12, 2023.
 */
static ETX_OTA_Status send_etx_ota_abort(int teuniz_rs232_lib_comport);

/**@brief   Sends an ETX OTA Command Type Packet containing the Start Command in it to the external device (connected to
 *          it via @ref COMPORT_NUMBER ).
 *
 * @details Sending a Start Command to that external device will request to start an ETX OTA Protocol process.
 *
 * @param teuniz_rs232_lib_comport  The converted value of the actual comport that was requested by the user but into
 *                                  its equivalent for the @ref teuniz_rs232_library (For more details, see the Table
 *                                  from @ref teuniz_rs232_library ).
 *
 * @retval  ETX_OTA_EC_OK
 * @retval 	ETX_OTA_EC_ERR
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    October 06, 2023.
 */
static ETX_OTA_Status send_etx_ota_start(int teuniz_rs232_lib_comport);

/**@brief   Sends an ETX OTA Header Type Packet to the external device (connected to it via @ref COMPORT_NUMBER ) that
 *          contains the general information of the Payload to be sent to that external device.
 *
 * @param teuniz_rs232_lib_comport  The converted value of the actual comport that was requested by the user but into
 *                                  its equivalent for the @ref teuniz_rs232_library (For more details, see the Table
 *                                  from @ref teuniz_rs232_library ).
 *
 * @retval 					ETX_OTA_EC_OK
 * @retval 					ETX_OTA_EC_ERR
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    October 07, 2023.
 */
static ETX_OTA_Status send_etx_ota_header(int teuniz_rs232_lib_comport, header_data_t *etx_ota_header_info);

/**@brief   Sends an ETX OTA Data Type Packet to the external device (connected to it via @ref COMPORT_NUMBER ) that
 *          contains some desired Payload Data.
 *
 * @param teuniz_rs232_lib_comport  The converted value of the actual comport that was requested by the user but into
 *                                  its equivalent for the @ref teuniz_rs232_library (For more details, see the Table
 *                                  from @ref teuniz_rs232_library ).
 * @param[in] payload               Pointer to the Payload Data that wants to be send in the current ETX OTA Data Type
 *                                  Packet.
 * @param data_len                  Length in bytes of the Payload Data.
 *
 * @retval 					ETX_OTA_EC_OK
 * @retval 					ETX_OTA_EC_ERR
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    October 07, 2023.
 */
static ETX_OTA_Status send_etx_ota_data(int teuniz_rs232_lib_comport, uint8_t *payload, uint16_t data_len);

/**@brief   Sends an ETX OTA Command Type Packet containing the End Command in it to the external device (connected to
 *          it via @ref COMPORT_NUMBER ).
 *
 * @details Sending a End Command to that external device will allow the host to confirm with that device that the
 *          Packets for the current ETX OTA Process have been all send by now.
 *
 * @param teuniz_rs232_lib_comport  The converted value of the actual comport that was requested by the user but into
 *                                  its equivalent for the @ref teuniz_rs232_library (For more details, see the Table
 *                                  from @ref teuniz_rs232_library ).
 *
 * @retval 					ETX_OTA_EC_OK
 * @retval 					ETX_OTA_EC_ERR
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    October 07, 2023.
 */
static ETX_OTA_Status send_etx_ota_end(int teuniz_rs232_lib_comport);

#if !ETX_OTA_VERBOSE
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

static void LOG(LOG_t log_type, const char* msg, ...)
{
    #if ETX_OTA_VERBOSE
        if (log_type == DEBUG_t)
        {
            printf("DEBUG: ");
        }
        else if (log_type == INFO_t)
        {
            printf("INFO: ");
        }
        else if (log_type == DONE_t)
        {
            printf("DONE: ");
        }
        else if (log_type == WARNING_t)
        {
            printf("WARNING: ");
        }
        else if (log_type == ERROR_t)
        {
            printf("ERROR: ");
        }
        if (log_type==DEBUG_t || log_type==INFO_t || log_type==DONE_t || log_type==WARNING_t || log_type==ERROR_t)
        {
            // initializing list pointer
            va_list ptr;
            va_start(ptr, msg);

            // char array to store token
            char token[1000];
            // index of where to store the characters of msg in
            // token
            int k = 0;

            // parsing the formatted string
            for (int i = 0; msg[i] != '\0'; i++) {
                token[k++] = msg[i];

                if (msg[i + 1] == '%' || msg[i + 1] == '\0') {
                    token[k] = '\0';
                    k = 0;
                    if (token[0] != '%') {
                        fprintf(
                                stdout, "%s",
                                token); // printing the whole token if
                        // it is not a format specifier
                    }
                    else {
                        int j = 1;
                        char ch1 = 0;

                        // this loop is required when printing
                        // formatted value like 0.2f, when ch1='f'
                        // loop ends
                        while ((ch1 = token[j++]) < 58) {
                        }
                        // for integers
                        if (ch1 == 'i' || ch1 == 'd' || ch1 == 'u'
                            || ch1 == 'h') {
                            fprintf(stdout, token,
                                    va_arg(ptr, int));
                        }
                            // for characters
                        else if (ch1 == 'c') {
                            fprintf(stdout, token,
                                    va_arg(ptr, int));
                        }
                            // for float values
                        else if (ch1 == 'f') {
                            fprintf(stdout, token,
                                    va_arg(ptr, double));
                        }
                        else if (ch1 == 'l') {
                            char ch2 = token[2];

                            // for long int
                            if (ch2 == 'u' || ch2 == 'd'
                                || ch2 == 'i') {
                                fprintf(stdout, token,
                                        va_arg(ptr, long));
                            }

                                // for double
                            else if (ch2 == 'f') {
                                fprintf(stdout, token,
                                        va_arg(ptr, double));
                            }
                        }
                        else if (ch1 == 'L') {
                            char ch2 = token[2];

                            // for long long int
                            if (ch2 == 'u' || ch2 == 'd'
                                || ch2 == 'i') {
                                fprintf(stdout, token,
                                        va_arg(ptr, long long));
                            }

                                // for long double
                            else if (ch2 == 'f') {
                                fprintf(stdout, token,
                                        va_arg(ptr, long double));
                            }
                        }

                            // for strings
                        else if (ch1 == 's') {
                            fprintf(stdout, token,
                                    va_arg(ptr, char*));
                        }

                            // print the whole token
                            // if no case is matched
                        else {
                            fprintf(stdout, "%s", token);
                        }
                    }
                }
            }

            // ending traversal
            va_end(ptr);
        }
    printf("\n");
    #endif
}


static uint32_t crc32_mpeg2(uint8_t *p_data, uint32_t DataLength)
{
    /** <b>Local variable checksum:</b> Will hold the resulting checksum of the 32-bit CRC Hash Function to be calculated. @note A checksum is any sort of mathematical operation that it is performed on data to represent its number of bits in a transmission message. This is usually used by programmers to detect high-level errors within data transmissions. The way this is used is prior to transmission, such that every piece of data or file can be assigned a checksum value after running a cryptographic has function, which in this particular case, the has function is 32-bit CRC. */
    uint32_t checksum = 0xFFFFFFFF;

    /* Apply the 32-bit CRC Hash Function to the given input data (i.e., The data towards which the \p p_data pointer points to). */
    for (unsigned int i=0; i<DataLength; i++)
    {
        uint8_t top = (uint8_t) (checksum >> 24);
        top ^= p_data[i];
        checksum = (checksum << 8) ^ crc_table[top];
    }
    return checksum;
}

static bool is_ack_resp_received(int teuniz_rs232_lib_comport)
{
    /* Reset the data contained inside the ETX OTA Packet Buffer. */
    LOG(INFO_t, "Waiting for receiving an ETX OTA Response type Packet from Serial Port...");
    memset(ETX_OTA_Packet_Buffer, 0, ETX_OTA_PACKET_MAX_SIZE);

    /* Get the bytes available in the Serial Port if there is any. */
    usleep(TEUNIZ_LIB_POLL_COMPORT_DELAY);
    uint16_t len =  RS232_PollComport(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer, sizeof(ETX_OTA_Response_Packet_t));

    if (len > 0)
    {
        /** <b>Local pointer etx_ota_resp:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Response_Packet_t type. */
        ETX_OTA_Response_Packet_t *etx_ota_resp = (ETX_OTA_Response_Packet_t *) ETX_OTA_Packet_Buffer;
        if (etx_ota_resp->packet_type == ETX_OTA_PACKET_TYPE_RESPONSE)
        {
            if (etx_ota_resp->crc == crc32_mpeg2(&etx_ota_resp->status, 1))
            {
                LOG(DONE_t, "ETX OTA Response Type Packet successfully received and processed.");
                if (etx_ota_resp->status == ETX_OTA_ACK)
                {
                    LOG(INFO_t, "Received ACK Status Response.");
                    return true;
                }
                #if ETX_OTA_VERBOSE
                else
                {
                    LOG(INFO_t, "Received NACK Status Response.");
                }
                #endif
            }
            #if ETX_OTA_VERBOSE
            else
            {
                LOG(ERROR_t, "CRC mismatch: [Calculated CRC = 0x%08lX] [Recorded CRC = 0x%08lX]", crc32_mpeg2(&etx_ota_resp->status, 1), etx_ota_resp->crc);
            }
            #endif
        }
        #if ETX_OTA_VERBOSE
        else
        {
            LOG(ERROR_t, "Expected an ETX OTA Response Type Packet, but received something else.");
        }
        #endif
    }
    #if ETX_OTA_VERBOSE
    else
    {
        LOG(ERROR_t, "No data was received from the Serial Port.");
    }
    #endif

    return false;
}

static ETX_OTA_Status send_etx_ota_abort(int teuniz_rs232_lib_comport)
{
    /** <b>Local pointer etx_ota_abort:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Command_Packet_t type. */
    ETX_OTA_Command_Packet_t *etx_ota_abort = (ETX_OTA_Command_Packet_t *) ETX_OTA_Packet_Buffer;

    /* Reset and then Populate the ETX OTA Packet Buffer with a ETX OTA Command Type Packet carrying the Abort Command. */
    memset(ETX_OTA_Packet_Buffer, 0, ETX_OTA_PACKET_MAX_SIZE);
    etx_ota_abort->sof          = ETX_OTA_SOF;
    etx_ota_abort->packet_type  = ETX_OTA_PACKET_TYPE_CMD;
    etx_ota_abort->data_len     = 1;
    etx_ota_abort->cmd          = ETX_OTA_CMD_ABORT;
    etx_ota_abort->crc          = crc32_mpeg2(&etx_ota_abort->cmd, 1);
    etx_ota_abort->eof          = ETX_OTA_EOF;

    /* Send the ETX OTA Command Type Packet containing the Abort Command. */
    LOG(INFO_t, "Sending an ETX OTA Command Type Packet containing the Abort Command...");
    for (uint8_t i = 0; i<ETX_OTA_CMD_PACKET_T_SIZE; i++)
    {
        usleep(SEND_PACKET_BYTES_DELAY);
        if (RS232_SendByte(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer[i]))
        {
            LOG(ERROR_t, "A byte of the ETX OTA Command Type Packet containing the Abort Command could not be send over the Serial Port.");
            return ETX_OTA_EC_ERR;
        }
    }

    /* Validate receiving back an ACK Status Response from the MCU. */
    if (!is_ack_resp_received(teuniz_rs232_lib_comport))
    {
        LOG(ERROR_t, "The host machine has received a NACK from the external device.");
        return ETX_OTA_EC_ERR;
    }

    LOG(DONE_t, "ETX OTA Command Type Packet containing the Start Command was send successfully.");
    return ETX_OTA_EC_OK;
}

static ETX_OTA_Status send_etx_ota_start(int teuniz_rs232_lib_comport)
{
    /** <b>Local pointer etx_ota_start:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Command_Packet_t type. */
    ETX_OTA_Command_Packet_t *etx_ota_start = (ETX_OTA_Command_Packet_t *) ETX_OTA_Packet_Buffer;

    /* Reset and then Populate the ETX OTA Packet Buffer with a ETX OTA Command Type Packet carrying the Start Command. */
    memset(ETX_OTA_Packet_Buffer, 0, ETX_OTA_PACKET_MAX_SIZE);
    etx_ota_start->sof          = ETX_OTA_SOF;
    etx_ota_start->packet_type  = ETX_OTA_PACKET_TYPE_CMD;
    etx_ota_start->data_len     = 1;
    etx_ota_start->cmd          = ETX_OTA_CMD_START;
    etx_ota_start->crc          = crc32_mpeg2(&etx_ota_start->cmd, 1);
    etx_ota_start->eof          = ETX_OTA_EOF;

    /* Send the ETX OTA Command Type Packet containing the Start Command. */
    LOG(INFO_t, "Sending an ETX OTA Command Type Packet containing the Start Command...");
    for (uint8_t i = 0; i<ETX_OTA_CMD_PACKET_T_SIZE; i++)
    {
        usleep(SEND_PACKET_BYTES_DELAY);
        if (RS232_SendByte(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer[i]))
        {
            LOG(ERROR_t, "A byte of the ETX OTA Command Type Packet containing the Start Command START could not be send over the Serial Port.");
            return ETX_OTA_EC_ERR;
        }
    }

    /* Validate receiving back an ACK Status Response from the MCU. */
    if (!is_ack_resp_received(teuniz_rs232_lib_comport))
    {
        LOG(ERROR_t, "The host machine has received a NACK from the external device.");
        return ETX_OTA_EC_ERR;
    }

    LOG(DONE_t, "ETX OTA Command Type Packet containing the Start Command was send successfully.");
    return ETX_OTA_EC_OK;
}

static ETX_OTA_Status send_etx_ota_header(int teuniz_rs232_lib_comport, header_data_t *etx_ota_header_info)
{
    /** <b>Local pointer etx_ota_start:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Command_Packet_t type. */
    ETX_OTA_Header_Packet_t *etx_ota_header = (ETX_OTA_Header_Packet_t *) ETX_OTA_Packet_Buffer;

    /* Reset and then Populate the ETX OTA Packet Buffer with a ETX OTA Command Type Packet carrying the Start Command. */
    memset(ETX_OTA_Packet_Buffer, 0, ETX_OTA_PACKET_MAX_SIZE);
    etx_ota_header->sof          = ETX_OTA_SOF;
    etx_ota_header->packet_type  = ETX_OTA_PACKET_TYPE_HEADER;
    etx_ota_header->data_len     = ETX_OTA_HEADER_DATA_T_SIZE;
    memcpy(&etx_ota_header->meta_data, etx_ota_header_info, ETX_OTA_HEADER_DATA_T_SIZE);
    etx_ota_header->crc          = crc32_mpeg2((uint8_t *) &(etx_ota_header->meta_data), ETX_OTA_HEADER_DATA_T_SIZE);
    etx_ota_header->eof          = ETX_OTA_EOF;

    /* Send the ETX OTA Header Type Packet. */
    LOG(INFO_t, "Sending an ETX OTA Header Type Packet containing the general information of the Payload to be sent...");
    for (uint8_t i = 0; i<ETX_OTA_HEADER_PACKET_T_SIZE; i++)
    {
        usleep(SEND_PACKET_BYTES_DELAY);
        if (RS232_SendByte(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer[i]))
        {
            LOG(ERROR_t, "A byte of the ETX OTA Header Type Packet could not be send over the Serial Port.");
            return ETX_OTA_EC_ERR;
        }
    }

    /* Validate receiving back an ACK Status Response from the MCU. */
    if (!is_ack_resp_received(teuniz_rs232_lib_comport))
    {
        LOG(ERROR_t, "The host machine has received a NACK from the external device.");
        return ETX_OTA_EC_ERR;
    }

    LOG(DONE_t, "ETX OTA Header Type Packet has been sent successfully.");
    return ETX_OTA_EC_OK;
}

static ETX_OTA_Status send_etx_ota_data(int teuniz_rs232_lib_comport, uint8_t *payload, uint16_t data_len)
{
    /** <b>Local pointer etx_ota_data:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Data_Packet_t type. */
    ETX_OTA_Data_Packet_t *etx_ota_data = (ETX_OTA_Data_Packet_t *) ETX_OTA_Packet_Buffer;

    /* Reset and then Populate the ETX OTA Packet Buffer with a ETX OTA Data Type Packet carrying the requested Payload data. */
    memset(ETX_OTA_Packet_Buffer, 0, ETX_OTA_PACKET_MAX_SIZE);
    etx_ota_data->sof          = ETX_OTA_SOF;
    etx_ota_data->packet_type  = ETX_OTA_PACKET_TYPE_DATA;
    etx_ota_data->data_len     = data_len;
    memcpy(&ETX_OTA_Packet_Buffer[ETX_OTA_DATA_FIELD_INDEX], payload, data_len); // Populate Payload Data field.
    /** <b>Local variable offset_index:</b> Indicates the index value for a certain field contained in the current ETX OTA Data Type Packet. */
    uint16_t offset_index = ETX_OTA_DATA_FIELD_INDEX + data_len;
    /** <b>Local variable crc:</b> Holds the Calculated 32-bit CRC of the given Payload Data. */
    uint32_t crc = crc32_mpeg2(payload, data_len);
    memcpy(&ETX_OTA_Packet_Buffer[offset_index], (uint8_t *) &crc, ETX_OTA_CRC32_SIZE); // Populate CRC field.
    offset_index += ETX_OTA_CRC32_SIZE;
    ETX_OTA_Packet_Buffer[offset_index] = ETX_OTA_EOF; // Populate EOF field.
    offset_index += ETX_OTA_EOF_SIZE;

    /* Send an ETX OTA Data Type Packet. */
    LOG(INFO_t, "Sending an ETX OTA Data Type Packet containing %d bytes of Payload Data...", data_len);
    for (uint16_t i=0; i<offset_index; i++)
    {
        usleep(SEND_PACKET_BYTES_DELAY);
        if (RS232_SendByte(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer[i]))
        {
            LOG(ERROR_t, "A byte of the current ETX OTA Data Type Packet could not be send over the Serial Port.");
            return ETX_OTA_EC_ERR;
        }
    }

    /* Validate receiving back an ACK Status Response from the MCU. */
    /*
     NOTE:  For the cases where packs of 1024 bytes of payload data are send in this function, more delay time for the
            "RS232_PollComport()" function will be required so that we can guarantee that our external device receives
            the data and is able to respond back to our host machine within a time at which both programs (i.e., the
            one for our host machine and the one for the external device) work as expected.
            Therefore, the following additional line of code (i.e., "usleep(TEUNIZ_LIB_POLL_COMPORT_DELAY);") is used
            to address this problem.
     */
    usleep(TEUNIZ_LIB_POLL_COMPORT_DELAY);
    if (!is_ack_resp_received(teuniz_rs232_lib_comport))
    {
        LOG(ERROR_t, "The host machine has received a NACK from the external device.");
        return ETX_OTA_EC_ERR;
    }

    LOG(DONE_t, "ETX OTA Data Type Packet has been sent successfully.");
    return ETX_OTA_EC_OK;
}

static ETX_OTA_Status send_etx_ota_end(int teuniz_rs232_lib_comport)
{
    /** <b>Local pointer etx_ota_end:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Command_Packet_t type. */
    ETX_OTA_Command_Packet_t *etx_ota_end = (ETX_OTA_Command_Packet_t*)ETX_OTA_Packet_Buffer;

    /* Reset and then Populate the ETX OTA Packet Buffer with a ETX OTA Command Type Packet carrying the Start Command. */
    memset(ETX_OTA_Packet_Buffer, 0, ETX_OTA_PACKET_MAX_SIZE);
    etx_ota_end->sof          = ETX_OTA_SOF;
    etx_ota_end->packet_type  = ETX_OTA_PACKET_TYPE_CMD;
    etx_ota_end->data_len     = 1;
    etx_ota_end->cmd          = ETX_OTA_CMD_END;
    etx_ota_end->crc          = crc32_mpeg2(&etx_ota_end->cmd, 1);
    etx_ota_end->eof          = ETX_OTA_EOF;

    /* Send the ETX OTA Command Type Packet containing the End Command. */
    LOG(INFO_t, "Sending an ETX OTA Command Type Packet containing the End Command...");
    for (uint8_t i=0; i<ETX_OTA_CMD_PACKET_T_SIZE; i++)
    {
        usleep(SEND_PACKET_BYTES_DELAY);
        if (RS232_SendByte(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer[i]))
        {
            LOG(ERROR_t, "A byte of the ETX OTA Command Type Packet containing the End Command START could not be send over the Serial Port.");
            return ETX_OTA_EC_ERR;
        }
    }

    /* Validate receiving back an ACK Status Response from the MCU. */
    /*
     NOTE:  Apparently, after the ETX OTA Data Type Packets, here it is also required more delay time so that the
     "RS232_PollComport()" function works as expected.
     */
    usleep(TEUNIZ_LIB_POLL_COMPORT_DELAY);
    if (!is_ack_resp_received(teuniz_rs232_lib_comport))
    {
        LOG(ERROR_t, "The host machine has received a NACK from the external device.");
        return ETX_OTA_EC_ERR;
    }

    LOG(DONE_t, "ETX OTA Command Type Packet containing the End Command was send successfully.");
    return ETX_OTA_EC_OK;
}

ETX_OTA_Status start_etx_ota_process(int comport, char payload_path[], ETX_OTA_Payload_t ETX_OTA_Payload_Type)
{
    /** <b>Local variable teuniz_rs232_lib_comport:</b> Should hold the converted value of the actual comport that was requested by the user but into its equivalent for the @ref teuniz_rs232_library (For more details, see the Table from @ref teuniz_rs232_library ). */
    int teuniz_rs232_lib_comport;
    /** <b>Local variable mode:</b> Used to hold the character values for defining the desired Databits, Parity, Stopbit and to enable/disable the Flow Control, in that orderly fashion, in order to use them for the RS232 Protocol configuration process. @note The additional last value of 0 is required by the @ref teuniz_rs232_library to mark the end of the array. */
    char mode[] = {RS232_MODE_DATA_BITS, RS232_MODE_PARITY, RS232_MODE_STOPBITS, 0};
    /** <b>Local @ref FILE type pointer Fptr:</b> Used to point to a struct that contains all the information necessary to control a File I/O stream. */
    FILE *Fptr = NULL;
    /** <b>Local variable payload_size:</b> Holds the size in bytes of the whole Payload. */
    uint32_t payload_size = 0;
    /** <b>Local variable ret:</b> Used to hold the exception code value returned by a @ref ETX_OTA_Status function type. */
    ETX_OTA_Status ret;

    /* Get the equivalent of the requested COM port Number but as requested by the Teuniz RS232 Library. */
    teuniz_rs232_lib_comport = comport - 1;

    /* Open RS232 Comport that was requested by the user. */
    printf("Opening COM%d...\n", comport);
    if (RS232_OpenComport(teuniz_rs232_lib_comport, RS232_BAUDRATE, mode, RS232_IS_FLOW_CONTROL))
    {
        LOG(ERROR_t, "Can not open Requested Comport %d .", comport);
        return ETX_OTA_EC_ERR;
    }
    LOG(DONE_t, "COM Port has been successfully opened.");

    /* Open the File at the File Path that the user gave via \c payload_path in the case that a Firmware Image request to send to the MCU/MPU, and get the Payload size. */
    switch (ETX_OTA_Payload_Type)
    {
        case ETX_OTA_Bootloader_Firmware_Image:
        case ETX_OTA_Application_Firmware_Image:
            /* Open the File at the File Path that the user gave via \c payload_path in the case that a Firmware Image request to send to the MCU/MPU. */
            LOG(INFO_t, "Opening Payload File with File Path: %s...", payload_path);
            /** <b>Local variable error_code:</b> Stores the return value of the @ref fopen_s function. */
            errno_t error_code;
            error_code = fopen_s(&Fptr, payload_path,"rb");
            if (error_code != 0)
            {
                LOG(ERROR_t, "Could not open %s (errno code = %d)", payload_path, error_code);
                return ETX_OTA_EC_ERR;
            }
            LOG(DONE_t, "Payload File was opened successfully.");

            /* Get the Payload size. */
            LOG(INFO_t, "Getting Payload File...");
            fseek(Fptr, 0L, SEEK_END); // Set File position of the Stream at the end of the File.
            payload_size = ftell(Fptr);
            fseek(Fptr, 0L, SEEK_SET); // Reset File position of the Stream.
            break;
        case ETX_OTA_Custom_Data:
            // Defining the size of the Custom Data that will be later generated in this function.
            payload_size = CUSTOM_DATA_MAX_SIZE;
            break;
        default:
            LOG(ERROR_t, "The Payload Type indicated by the user is not recognized by the current ETX OTA Protocol.");
            return ETX_OTA_EC_NA;
    }

    /* Validate Payload size. */
    switch (ETX_OTA_Payload_Type)
    {
        case ETX_OTA_Bootloader_Firmware_Image:
            LOG(INFO_t, "The Payload Type indicated by the user is that of a Bootloader Firmware Update Image.");
            if (payload_size > ETX_OTA_BL_FW_SIZE)
            {
                LOG(ERROR_t, "The given Firmware Update Image exceeds the maximum bytes designated to the Bootloader Firmware.");
                return ETX_OTA_EC_NA;
            }
            break;
        case ETX_OTA_Application_Firmware_Image:
            LOG(INFO_t, "The Payload Type indicated by the user is that of an Application Firmware Update Image.");
            if (payload_size > ETX_OTA_APP_FW_SIZE)
            {
                LOG(ERROR_t, "The given Firmware Update Image exceeds the maximum bytes designated to the Application Firmware.");
                return ETX_OTA_EC_NA;
            }
            break;
        case ETX_OTA_Custom_Data:
            LOG(INFO_t, "The Payload Type indicated by the user is that of an ETX OTA Custom Data.");
            if (payload_size > CUSTOM_DATA_MAX_SIZE)
            {
                LOG(ERROR_t, "The given ETX OTA Custom Data exceeds the maximum bytes allows by the application.");
                return ETX_OTA_EC_NA;
            }
            break;
        default:
            LOG(ERROR_t, "The Payload Type indicated by the user is not recognized by the current ETX OTA Protocol.");
            return ETX_OTA_EC_NA;
    }
    LOG(INFO_t, "Payload File size = %d bytes.", payload_size);

    /* Read Payload file/data. */
    switch (ETX_OTA_Payload_Type)
    {
        case ETX_OTA_Bootloader_Firmware_Image:
        case ETX_OTA_Application_Firmware_Image:
            // NOTE: The "fread()" function returns the total number of elements that were successfully read.
            if (fread(PAYLOAD_CONTENT, 1, payload_size, Fptr) != payload_size)
            {
                LOG(ERROR_t, "Could not read File %s.", payload_path);
                return ETX_OTA_EC_ERR;
            }
            LOG(DONE_t, "Payload File was read successfully.");
            break;
        case ETX_OTA_Custom_Data:
            // Generating some Custom Data.
            for (uint32_t i=0; i<CUSTOM_DATA_MAX_SIZE; i++)
            {
                PAYLOAD_CONTENT[i] = i;
            }
            break;
        default:
            LOG(ERROR_t, "The Payload Type indicated by the user is not recognized by the current ETX OTA Protocol.");
            return ETX_OTA_EC_NA;
    }

    /* Send ETX OTA Abort Command to stop any ongoing transaction before starting this new one. */
    // NOTE:    Empirical measurements of an approximate of how much the following do-while loop can last is around 75
    //          seconds, but probably a safe value would be 90 seconds.
    if (payload_send_attempts == 0)
    {
        LOG(INFO_t, "Aborting any ongoing ETX OTA current Process...");
        LOG(INFO_t, "Sending Abort Command to external device...");
        do
        {
            ret = send_etx_ota_abort(teuniz_rs232_lib_comport);
        }
        while (ret != ETX_OTA_EC_OK);
        LOG(DONE_t, "Abort Command has been successfully send to the external device.");
    }

    /* Send OTA Start Command. */
    LOG(INFO_t, "Starting ETX OTA Process...");
    LOG(INFO_t, "Sending Start Command to external device...");
    ret = send_etx_ota_start(teuniz_rs232_lib_comport);
    if (ret != ETX_OTA_EC_OK)
    {
        if (payload_send_attempts++ == 0)
        {
            if (ETX_OTA_Payload_Type == ETX_OTA_Bootloader_Firmware_Image)
            {
                printf("Since a NACK Status Response was received after attempting to send an ETX OTA Start Command Packet, then our host machine will try again to send the desired Bootloader Firmware Image once after %.2f seconds.\n", ((float)(TRY_AGAIN_SENDING_FWI_DELAY))/1000000.0);
            }
            else
            {
                printf("Since a NACK Status Response was received after attempting to send an ETX OTA Start Command Packet, then our host machine will try again to send the desired Application Firmware Image once after %.2f seconds.\n", ((float)(TRY_AGAIN_SENDING_FWI_DELAY))/1000000.0);
            }
            if (Fptr)
            {
                fclose(Fptr);
            }
            RS232_CloseComport(teuniz_rs232_lib_comport);
            usleep(TRY_AGAIN_SENDING_FWI_DELAY);
            ret = start_etx_ota_process(comport, payload_path, ETX_OTA_Payload_Type);
            return ret;
        }
        LOG(ERROR_t, "Sending Start Command to MCU failed (ETX OTA Exception code = %d).", ret);
        return ETX_OTA_EC_ERR;
    }
    LOG(DONE_t, "Start Command has been successfully send to the external device.");

    /* Send ETX OTA Header Type Packet. */
    /** <b>Local variable etx_ota_header_info:</b> Holds the general information of the Payload, which are its size, its 32-bit CRC and its payload type. */
    header_data_t etx_ota_header_info;
    etx_ota_header_info.package_size = payload_size;
    etx_ota_header_info.package_crc  = crc32_mpeg2(PAYLOAD_CONTENT, payload_size);
    etx_ota_header_info.reserved1 = ETX_OTA_32BITS_RESET_VALUE;
    etx_ota_header_info.reserved2 = ETX_OTA_16BITS_RESET_VALUE;
    etx_ota_header_info.reserved3 = ETX_OTA_8BITS_RESET_VALUE;
    etx_ota_header_info.payload_type = ETX_OTA_Payload_Type;
    LOG(INFO_t, "Sending ETX OTA Header Type Packet...");
    ret = send_etx_ota_header(teuniz_rs232_lib_comport, &etx_ota_header_info);
    if (ret != ETX_OTA_EC_OK)
    {
        if (payload_send_attempts++ == 0)
        {
            if (ETX_OTA_Payload_Type == ETX_OTA_Bootloader_Firmware_Image)
            {
                printf("Since a NACK Status Response was received after attempting to send an ETX OTA Header Type Packet, then our host machine will try again to send the desired Bootloader Firmware Image once after %.2f seconds.\n", ((float)(TRY_AGAIN_SENDING_FWI_DELAY))/1000000.0);
            }
            else if (ETX_OTA_Payload_Type == ETX_OTA_Application_Firmware_Image)
            {
                printf("Since a NACK Status Response was received after attempting to send an ETX OTA Header Type Packet, then our host machine will try again to send the desired Application Firmware Image once after %.2f seconds.\n", ((float)(TRY_AGAIN_SENDING_FWI_DELAY))/1000000.0);
            }
            else
            {
                printf("Since a NACK Status Response was received after attempting to send an ETX OTA Header Type Packet, then our host machine will try again to send the desired ETX OTA Custom Data once after %.2f seconds.\n", ((float)(TRY_AGAIN_SENDING_FWI_DELAY))/1000000.0);
            }
            if (Fptr)
            {
                fclose(Fptr);
            }
            RS232_CloseComport(teuniz_rs232_lib_comport);
            usleep(TRY_AGAIN_SENDING_FWI_DELAY);
            ret = start_etx_ota_process(comport, payload_path, ETX_OTA_Payload_Type);
            return ret;
        }
        LOG(ERROR_t, "The ETX OTA Header Type Packet could not not be send (ETX OTA Exception code = %d).", ret);
        return ETX_OTA_EC_ERR;
    }
    LOG(DONE_t, "The ETX OTA Header Type Packet was send successfully.");

    /* Sending Payload Data via one or more ETX OTA Data Type Packets correspondingly. */
    /** <b>Local variable size:</b> Indicates the number of bytes from the Payload that have been send to the external device (i.e., the device that is desired to connect to via the \p comport param) via ETX OTA Data Type Packets. */
    uint16_t size = 0;
    printf("Sending Payload Data via ETX OTA Protocol...\n");
    for (uint32_t i=0; i<payload_size; )
    {
        LOG(INFO_t, "Sending an ETX OTA Data Type Packet...");
        if ((payload_size-i) >= ETX_OTA_DATA_MAX_SIZE)
        {
            size = ETX_OTA_DATA_MAX_SIZE;
        }
        else
        {
            size = payload_size - i;
        }
        if (payload_size%ETX_OTA_DATA_MAX_SIZE == 0)
        {
            printf("[%d/%d]\r\n", i/ETX_OTA_DATA_MAX_SIZE, payload_size/ETX_OTA_DATA_MAX_SIZE);
        }
        else
        {
            printf("[%d/%d]\r\n", i/ETX_OTA_DATA_MAX_SIZE, payload_size/ETX_OTA_DATA_MAX_SIZE+1);
        }
        ret = send_etx_ota_data(teuniz_rs232_lib_comport, &PAYLOAD_CONTENT[i], size);
        if (ret != ETX_OTA_EC_OK)
        {
            LOG(ERROR_t, "The current ETX OTA Data Type Packet could not not be send (ETX OTA Exception code = %d).", ret);
            return ETX_OTA_EC_ERR;
        }
        LOG(DONE_t, "The current ETX OTA Data Type Packet was send successfully.");
        i += size;
    }
    if (payload_size%ETX_OTA_DATA_MAX_SIZE == 0)
    {
        printf("[%d/%d]\r\n", payload_size/ETX_OTA_DATA_MAX_SIZE, payload_size/ETX_OTA_DATA_MAX_SIZE);
    }
    else
    {
        printf("[%d/%d]\r\n", payload_size/ETX_OTA_DATA_MAX_SIZE+1, payload_size/ETX_OTA_DATA_MAX_SIZE+1);
    }
    LOG(DONE_t, "The Payload Data was send successfully.");

    /* Send OTA End Command. */
    LOG(INFO_t, "Sending End Command to external device...");
    ret = send_etx_ota_end(teuniz_rs232_lib_comport);
    if (ret != ETX_OTA_EC_OK)
    {
        LOG(ERROR_t, "Sending End Command to the external device failed (ETX OTA Exception code = %d).", ret);
        return ETX_OTA_EC_ERR;
    }
    LOG(DONE_t, "Start Command has been successfully send to the external device.");

    if (Fptr)
    {
        fclose(Fptr);
    }
    RS232_CloseComport(teuniz_rs232_lib_comport);

    LOG(DONE_t, "ETX OTA Process has concluded successfully.");
    return ETX_OTA_EC_OK;
}

/** @} */
