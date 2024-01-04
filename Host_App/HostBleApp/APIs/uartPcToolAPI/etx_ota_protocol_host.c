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
#include <string.h> // Library for using the "memcpy()" function.
#include <stdarg.h>

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
#define ETX_OTA_CMD_PACKET_T_SIZE       (sizeof(ETX_OTA_Command_Packet_t))              /**< @brief Length in bytes of the @ref ETX_OTA_Command_Packet_t struct. */
#define ETX_OTA_HEADER_DATA_T_SIZE      (sizeof(header_data_t))                         /**< @brief Length in bytes of the @ref header_data_t struct. */
#define ETX_OTA_HEADER_PACKET_T_SIZE    (sizeof(ETX_OTA_Header_Packet_t))               /**< @brief Length in bytes of the @ref ETX_OTA_Header_Packet_t struct. */
uint8_t payload_send_attempts = 0;                                                      /**< @brief Attempts that have been made to send a Payload to the external device (connected to it via @ref COMPORT_NUMBER ). @note This variable is used only to count the attempts of sending that but only whenever receiving a NACK Response Status from sending an ETX OTA Packet Type Packet to that external device after having sent either an ETX OTA Start Command or an ETX OTA Header Type Command. The reason for this is because if that happens, it is highly possible that this is due to that the external device was doing something else aside waiting to receive an ETX OTA Request from the host at that moment to be able to receive the desired payload from the host. */

static uint8_t ETX_OTA_Packet_Buffer[ETX_OTA_PACKET_MAX_SIZE];        /**< @brief Global buffer that will be used by our host machine to hold the whole data of either a received ETX OTA Packet from the external device (connected to it via @ref COMPORT_NUMBER ) or to populate in it the Packet's bytes to be send to that external device. */
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
 * @param[in] p_ETX_OTA_api         Should hold all the data received via the API of the @ref main_program .
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
 * @date    November 02, 2023.
 */
static bool is_ack_resp_received(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport);

/**@brief   Sends an ETX OTA Command Type Packet containing the Abort Command in it to the external device (connected to
 *          it via @ref Command_Line_Arguments::COMPORT_NUMBER ).
 *
 * @note    Sending an Abort Command to that external device will request to stop any ongoing ETX OTA Protocol process.
 * @note    Unlike the other ETX OTA functions, this @ref send_etx_ota_abort function will immediately receive and
 *          process any data send back from the slave device to our host machine (the other ETX OTA functions receive
 *          the data only after having send their respective Packet Data to the slave device, in case that any is
 *          actually received).
 *
 * @param[in] p_ETX_OTA_api         Should hold all the data received via the API of the @ref main_program .
 * @param teuniz_rs232_lib_comport  The converted value of the actual comport that was requested by the user but into
 *                                  its equivalent for the @ref teuniz_rs232_library (For more details, see the Table
 *                                  from @ref teuniz_rs232_library ).
 *
 * @retval  ETX_OTA_EC_OK
 * @retval 	ETX_OTA_EC_ABORT_CMD_SEND_DATA_ERR
 * @retval  ETX_OTA_EC_ABORT_CMD_NACK_RESP
 *
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @date    December 28, 2023.
 */
static ETX_OTA_Status send_etx_ota_abort(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport);

/**@brief   Sends an ETX OTA Command Type Packet containing the Start Command in it to the external device (connected to
 *          it via @ref COMPORT_NUMBER ).
 *
 * @details Sending a Start Command to that external device will request to start an ETX OTA Protocol process.
 *
 * @param[in] p_ETX_OTA_api         Should hold all the data received via the API of the @ref main_program .
 * @param teuniz_rs232_lib_comport  The converted value of the actual comport that was requested by the user but into
 *                                  its equivalent for the @ref teuniz_rs232_library (For more details, see the Table
 *                                  from @ref teuniz_rs232_library ).
 *
 * @retval  ETX_OTA_EC_OK
 * @retval 	ETX_OTA_EC_START_CMD_SEND_DATA_ERR
 * @retval  ETX_OTA_EC_START_CMD_NACK_RESP
 *
 * @note    For more details on the returned values, see @ref ETX_OTA_Status .
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 02, 2023.
 */
static ETX_OTA_Status send_etx_ota_start(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport);

/**@brief   Sends an ETX OTA Header Type Packet to the external device (connected to it via @ref COMPORT_NUMBER ) that
 *          contains the general information of the Payload to be sent to that external device.
 *
 * @param[in] p_ETX_OTA_api         Should hold all the data received via the API of the @ref main_program .
 * @param teuniz_rs232_lib_comport  The converted value of the actual comport that was requested by the user but into
 *                                  its equivalent for the @ref teuniz_rs232_library (For more details, see the Table
 *                                  from @ref teuniz_rs232_library ).
 * @param[in] etx_ota_header_info   Should hold the the Header data of the ETX OTA Header Type Packet that is to be sent
 *                                  to the external device.
 *
 * @retval  ETX_OTA_EC_OK
 * @retval 	ETX_OTA_EC_HEADER_PCKT_SEND_DATA_ERR
 * @retval 	ETX_OTA_EC_HEADER_PCKT_NACK_RESP
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 02, 2023.
 */
static ETX_OTA_Status send_etx_ota_header(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport, header_data_t *etx_ota_header_info);

/**@brief   Sends an ETX OTA Data Type Packet to the external device (connected to it via @ref COMPORT_NUMBER ) that
 *          contains some desired Payload Data.
 *
 * @param[in] p_ETX_OTA_api         Should hold all the data received via the API of the @ref main_program .
 * @param teuniz_rs232_lib_comport  The converted value of the actual comport that was requested by the user but into
 *                                  its equivalent for the @ref teuniz_rs232_library (For more details, see the Table
 *                                  from @ref teuniz_rs232_library ).
 * @param[in] payload               Pointer to the Payload Data that wants to be send in the current ETX OTA Data Type
 *                                  Packet.
 * @param data_len                  Length in bytes of the Payload Data.
 *
 * @retval  ETX_OTA_EC_OK
 * @retval 	ETX_OTA_EC_DATA_PCKT_SEND_DATA_ERR
 * @retval 	ETX_OTA_EC_DATA_PCKT_NACK_RESP
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 02, 2023.
 */
static ETX_OTA_Status send_etx_ota_data(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport, uint8_t *payload, uint16_t data_len);

/**@brief   Sends an ETX OTA Command Type Packet containing the End Command in it to the external device (connected to
 *          it via @ref COMPORT_NUMBER ).
 *
 * @details Sending a End Command to that external device will allow the host to confirm with that device that the
 *          Packets for the current ETX OTA Process have been all send by now.
 *
 * @param[in] p_ETX_OTA_api         Should hold all the data received via the API of the @ref main_program .
 * @param teuniz_rs232_lib_comport  The converted value of the actual comport that was requested by the user but into
 *                                  its equivalent for the @ref teuniz_rs232_library (For more details, see the Table
 *                                  from @ref teuniz_rs232_library ).
 *
 * @retval  ETX_OTA_EC_OK
 * @retval 	ETX_OTA_EC_END_CMD_SEND_DATA_ERR
 * @retval 	ETX_OTA_EC_END_CMD_NACK_RESP
 *
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 02, 2023.
 */
static ETX_OTA_Status send_etx_ota_end(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport);

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

static bool is_ack_resp_received(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport)
{
    /* Reset the data contained inside the ETX OTA Packet Buffer. */
    memset(ETX_OTA_Packet_Buffer, 0, ETX_OTA_PACKET_MAX_SIZE);

    /* Get the bytes available in the Serial Port if there is any. */
    usleep(p_ETX_OTA_api->teuniz_lib_poll_comport_delay);
    uint16_t len =  RS232_PollComport(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer, sizeof(ETX_OTA_Response_Packet_t));

    /* Validate the received data to be an ETX OTA Response Type Packet and, if true, get its Status Response value. */
    if (len > 0)
    {
        /** <b>Local pointer etx_ota_resp:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Response_Packet_t type. */
        ETX_OTA_Response_Packet_t *etx_ota_resp = (ETX_OTA_Response_Packet_t *) ETX_OTA_Packet_Buffer;
        if (etx_ota_resp->packet_type == ETX_OTA_PACKET_TYPE_RESPONSE)
        {
            if (etx_ota_resp->crc == crc32_mpeg2(&etx_ota_resp->status, 1))
            {
                if (etx_ota_resp->status == ETX_OTA_ACK)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

static ETX_OTA_Status send_etx_ota_abort(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport)
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
    /** <b>Local constant SIZE_OF_ETX_OTA_RESP_PACKT:</b> Length in bytes of a @ref ETX_OTA_Response_Packet_t Packet. */
    const uint8_t SIZE_OF_ETX_OTA_RESP_PACKT = sizeof(ETX_OTA_Response_Packet_t);
    /** <b>Local pointer tmp_ETX_OTA_Packet_Buffer:</b> Temporal buffer that will be used by our host machine to hold the whole data a received ETX OTA Packet from the external device (connected to it via @ref COMPORT_NUMBER ). */
    uint8_t tmp_ETX_OTA_Packet_Buffer[SIZE_OF_ETX_OTA_RESP_PACKT];
    memset(tmp_ETX_OTA_Packet_Buffer, 0, SIZE_OF_ETX_OTA_RESP_PACKT);
    /** <b>Local variable len:</b> Will hold the currently received bytes from the slave device via an ETX OTA Transaction. */
    uint16_t len;
    for (uint8_t i = 0; i<ETX_OTA_CMD_PACKET_T_SIZE; i++)
    {
        usleep(SEND_PACKET_BYTES_DELAY);
        if (RS232_SendByte(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer[i]))
        {
            return ETX_OTA_EC_ABORT_CMD_SEND_DATA_ERR;
        }

        /* Validate receiving back an ACK Status Response from the MCU. */
        // NOTE:    The following code is a modified version of the @ref is_ack_resp_received function that is used
        //          instead in order to formulate a way to receive ETX OTA Response Type Packets such that the
        //          @ref send_etx_ota_abort function can terminate the immediate instant that such a Packet Type is
        //          received. This way, whenever the Abort Command Loop of this program is used, then our host machine
        //          will be able to eventually send a successful Abort Command to the slave device.
        usleep(p_ETX_OTA_api->teuniz_lib_poll_comport_delay);
        len =  RS232_PollComport(teuniz_rs232_lib_comport, tmp_ETX_OTA_Packet_Buffer, SIZE_OF_ETX_OTA_RESP_PACKT);
        if (len > 0)
        {
            /** <b>Local pointer etx_ota_resp:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Response_Packet_t type. */
            ETX_OTA_Response_Packet_t *etx_ota_resp = (ETX_OTA_Response_Packet_t *) tmp_ETX_OTA_Packet_Buffer;
            if (etx_ota_resp->packet_type == ETX_OTA_PACKET_TYPE_RESPONSE)
            {
                if (etx_ota_resp->crc == crc32_mpeg2(&etx_ota_resp->status, 1))
                {
                    if (etx_ota_resp->status == ETX_OTA_ACK)
                    {
                        break;
                    }
                }
            }
            return ETX_OTA_EC_ABORT_CMD_NACK_RESP;
        }
    }

    return ETX_OTA_EC_OK;
}

static ETX_OTA_Status send_etx_ota_start(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport)
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
    for (uint8_t i = 0; i<ETX_OTA_CMD_PACKET_T_SIZE; i++)
    {
        usleep(p_ETX_OTA_api->send_packet_bytes_delay);
        if (RS232_SendByte(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer[i]))
        {
            return ETX_OTA_EC_START_CMD_SEND_DATA_ERR;
        }
    }

    /* Validate receiving back an ACK Status Response from the MCU. */
    if (!is_ack_resp_received(p_ETX_OTA_api, teuniz_rs232_lib_comport))
    {
        return ETX_OTA_EC_START_CMD_NACK_RESP;
    }

    return ETX_OTA_EC_OK;
}

static ETX_OTA_Status send_etx_ota_header(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport, header_data_t *etx_ota_header_info)
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
    for (uint8_t i = 0; i<ETX_OTA_HEADER_PACKET_T_SIZE; i++)
    {
        usleep(p_ETX_OTA_api->send_packet_bytes_delay);
        if (RS232_SendByte(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer[i]))
        {
            return ETX_OTA_EC_HEADER_PCKT_SEND_DATA_ERR;
        }
    }

    /* Validate receiving back an ACK Status Response from the MCU. */
    if (!is_ack_resp_received(p_ETX_OTA_api, teuniz_rs232_lib_comport))
    {
        return ETX_OTA_EC_HEADER_PCKT_NACK_RESP;
    }

    return ETX_OTA_EC_OK;
}

static ETX_OTA_Status send_etx_ota_data(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport, uint8_t *payload, uint16_t data_len)
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
    for (uint16_t i=0; i<offset_index; i++)
    {
        usleep(p_ETX_OTA_api->send_packet_bytes_delay);
        if (RS232_SendByte(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer[i]))
        {
            return ETX_OTA_EC_DATA_PCKT_SEND_DATA_ERR;
        }
    }

    /* Validate receiving back an ACK Status Response from the MCU. */
    /*
     NOTE:  For the cases where packs of 1024 bytes of payload data are send in this function, more delay time for the
            "RS232_PollComport()" function will be required so that we can guarantee that our external device receives
            the data and is able to respond back to our host machine within a time at which both programs (i.e., the
            one for our host machine and the one for the external device) work as expected.
            Therefore, the following additional line of code (i.e., "usleep(teuniz_lib_poll_comport_delay);") is used
            to address this problem.
     */
    usleep(p_ETX_OTA_api->teuniz_lib_poll_comport_delay);
    if (!is_ack_resp_received(p_ETX_OTA_api, teuniz_rs232_lib_comport))
    {
        return ETX_OTA_EC_DATA_PCKT_NACK_RESP;
    }

    return ETX_OTA_EC_OK;
}

static ETX_OTA_Status send_etx_ota_end(ETX_OTA_API_t *p_ETX_OTA_api, int teuniz_rs232_lib_comport)
{
    /** <b>Local pointer etx_ota_end:</b> Points to the data of the latest ETX OTA Packet but in @ref ETX_OTA_Command_Packet_t type. */
    ETX_OTA_Command_Packet_t *etx_ota_end = (ETX_OTA_Command_Packet_t*) ETX_OTA_Packet_Buffer;

    /* Reset and then Populate the ETX OTA Packet Buffer with a ETX OTA Command Type Packet carrying the Start Command. */
    memset(ETX_OTA_Packet_Buffer, 0, ETX_OTA_PACKET_MAX_SIZE);
    etx_ota_end->sof          = ETX_OTA_SOF;
    etx_ota_end->packet_type  = ETX_OTA_PACKET_TYPE_CMD;
    etx_ota_end->data_len     = 1;
    etx_ota_end->cmd          = ETX_OTA_CMD_END;
    etx_ota_end->crc          = crc32_mpeg2(&etx_ota_end->cmd, 1);
    etx_ota_end->eof          = ETX_OTA_EOF;

    /* Send the ETX OTA Command Type Packet containing the End Command. */
    for (uint8_t i=0; i<ETX_OTA_CMD_PACKET_T_SIZE; i++)
    {
        usleep(p_ETX_OTA_api->send_packet_bytes_delay);
        if (RS232_SendByte(teuniz_rs232_lib_comport, ETX_OTA_Packet_Buffer[i]))
        {
            return ETX_OTA_EC_END_CMD_SEND_DATA_ERR;
        }
    }

    /* Validate receiving back an ACK Status Response from the MCU. */
    /*
     NOTE:  Apparently, after the ETX OTA Data Type Packets, here it is also required more delay time so that the
     "RS232_PollComport()" function works as expected.
     */
    usleep(p_ETX_OTA_api->teuniz_lib_poll_comport_delay);
    if (!is_ack_resp_received(p_ETX_OTA_api, teuniz_rs232_lib_comport))
    {
        return ETX_OTA_EC_END_CMD_NACK_RESP;
    }

    return ETX_OTA_EC_OK;
}

ETX_OTA_Status start_etx_ota_process(ETX_OTA_API_t *p_ETX_OTA_api)
{
    /* Define the Core Local Variables of this function. */
    /** <b>Local variable ETX_OTA_BL_FW_SIZE:</b> Maximum size allowable for a Bootloader Firmware Image to have. */
    uint32_t ETX_OTA_BL_FW_SIZE = p_ETX_OTA_api->flash_page_size_in_bytes * p_ETX_OTA_api->ETX_bl_page_size;
    /** <b>Local variable ETX_OTA_APP_FW_SIZE:</b> Maximum size allowable for an Application Firmware Image to have. */
    uint32_t ETX_OTA_APP_FW_SIZE = p_ETX_OTA_api->flash_page_size_in_bytes * p_ETX_OTA_api->ETX_app_page_size;
    /** <b>Local variable ETX_OTA_MAX_FW_SIZE:</b> Maximum size allowable for a Firmware Image to have. */
    uint32_t ETX_OTA_MAX_FW_SIZE;
    if (ETX_OTA_APP_FW_SIZE > ETX_OTA_BL_FW_SIZE)
    {
        ETX_OTA_MAX_FW_SIZE = ETX_OTA_APP_FW_SIZE;
    }
    else
    {
        ETX_OTA_MAX_FW_SIZE = ETX_OTA_BL_FW_SIZE;
    }
    /** <b>Local variable PAYLOAD_CONTENT:</b> Variable used as a holder for the Payload contents. */
    uint8_t PAYLOAD_CONTENT[ETX_OTA_MAX_FW_SIZE];
    /** <b>Local variable teuniz_rs232_lib_comport:</b> Should hold the converted value of the actual comport that was requested by the user but into its equivalent for the @ref teuniz_rs232_library (For more details, see the Table from @ref teuniz_rs232_library ). */
    int teuniz_rs232_lib_comport;
    /** <b>Local variable mode:</b> Used to hold the character values for defining the desired Databits, Parity, Stopbit and to enable/disable the Flow Control, in that orderly fashion, in order to use them for the RS232 Protocol configuration process. @note The additional last value of 0 is required by the @ref teuniz_rs232_library to mark the end of the array. */
    char mode[] = {p_ETX_OTA_api->rs232_mode_data_bits, p_ETX_OTA_api->rs232_mode_parity, p_ETX_OTA_api->rs232_mode_stopbits, 0};
    /** <b>Local @ref FILE type pointer Fptr:</b> Used to point to a struct that contains all the information necessary to control a File I/O stream. */
    FILE *Fptr = NULL;
    /** <b>Local variable payload_size:</b> Holds the size in bytes of the whole Payload. */
    uint32_t payload_size = 0;
    /** <b>Local variable ret:</b> Used to hold the exception code value returned by a @ref ETX_OTA_Status function type. */
    ETX_OTA_Status ret;

    /* Get the equivalent of the requested COM port Number but as requested by the Teuniz RS232 Library. */
    teuniz_rs232_lib_comport = p_ETX_OTA_api->comport - 1;

    /* Open RS232 Comport that was requested by the user. */
    if (RS232_OpenComport(teuniz_rs232_lib_comport, p_ETX_OTA_api->rs232_baudrate, mode, p_ETX_OTA_api->rs232_is_flow_control))
    {
        return ETX_OTA_EC_OPEN_COM_ERR;
    }

    /* Get Payload Data and size. */
    switch (p_ETX_OTA_api->ETX_OTA_Payload_Type)
    {
        case ETX_OTA_Bootloader_Firmware_Image:
        case ETX_OTA_Application_Firmware_Image:
            /* Open the File at the File Path that the user gave via \c payload_path_or_data . */
            /** <b>Local variable error_code:</b> Stores the return value of the @ref fopen_s function. */
            errno_t error_code;
            error_code = fopen_s(&Fptr, p_ETX_OTA_api->payload_path_or_data,"rb");
            if (error_code != 0)
            {
                return ETX_OTA_EC_OPEN_FILE_ERR;
            }

            /* Get and validate Payload size. */
            fseek(Fptr, 0L, SEEK_END); // Set File position of the Stream at the end of the File.
            payload_size = ftell(Fptr);
            fseek(Fptr, 0L, SEEK_SET); // Reset File position of the Stream.
            if (p_ETX_OTA_api->ETX_OTA_Payload_Type == ETX_OTA_Bootloader_Firmware_Image)
            {
                if (payload_size > ETX_OTA_BL_FW_SIZE)
                {
                    return ETX_OTA_EC_NA;
                }
            }
            else
            {
                if (payload_size > ETX_OTA_APP_FW_SIZE)
                {
                    return ETX_OTA_EC_NA;
                }
            }

            /* Read Payload file/data. */
            // NOTE: The "fread()" function returns the total number of elements that were successfully read.
            if (fread(PAYLOAD_CONTENT, 1, payload_size, Fptr) != payload_size)
            {
                return ETX_OTA_EC_READ_FILE_ERR;
            }
            break;
        case ETX_OTA_Custom_Data:
            payload_size = p_ETX_OTA_api->payload_size;
            memcpy(PAYLOAD_CONTENT, p_ETX_OTA_api->payload_path_or_data, payload_size);
            break;
        default:
            return ETX_OTA_EC_UNRECOG_PAYLOAD;
    }

    /* Send ETX OTA Abort Command to stop any ongoing transaction before starting this new one. */
    // NOTE:    The following Abort Command Loop is only needed the first time; whenever at the second attempt of
    //          starting an ETX OTA Transaction with the slave device, the slave device should already have finished
    //          rebooting and should also be at its Bootloader Firmware, which is why this Abort Command Loop is not
    //          needed during the second attempt of starting an ETX OTA Transaction.
    if (payload_send_attempts == 0)
    {
        for (uint8_t sent_attempts=0; sent_attempts<(ETX_OTA_PACKET_MAX_SIZE/sizeof(ETX_OTA_Command_Packet_t)+1); sent_attempts++)
        {
            ret = send_etx_ota_abort(p_ETX_OTA_api, teuniz_rs232_lib_comport);
            if (ret == ETX_OTA_EC_OK)
            {
                break;
            }
        }
        if (ret != ETX_OTA_EC_OK)
        {
            return ETX_OTA_EC_ABORT_LOOP_ERR;
        }
    }

    /* Send ETX OTA Start Command. */
    ret = send_etx_ota_start(p_ETX_OTA_api, teuniz_rs232_lib_comport);

    /* If ETX OTA Start Command could not be sent successfully, then try sending it one more time in case that the slave device was not hearing the host device for a Command. */
    if (ret != ETX_OTA_EC_OK)
    {
        if (payload_send_attempts++ == 0)
        {
            if (Fptr)
            {
                fclose(Fptr);
            }
            RS232_CloseComport(teuniz_rs232_lib_comport);
            usleep(p_ETX_OTA_api->try_again_sending_fwi_delay);
            ret = start_etx_ota_process(p_ETX_OTA_api);
        }
        return ret;
    }

    /* Send ETX OTA Header Type Packet. */
    /** <b>Local variable etx_ota_header_info:</b> Holds the general information of the Payload, which are its size, its 32-bit CRC and its payload type. */
    header_data_t etx_ota_header_info;
    etx_ota_header_info.package_size = payload_size;
    etx_ota_header_info.package_crc  = crc32_mpeg2(PAYLOAD_CONTENT, payload_size);
    etx_ota_header_info.reserved1 = ETX_OTA_32BITS_RESET_VALUE;
    etx_ota_header_info.reserved2 = ETX_OTA_16BITS_RESET_VALUE;
    etx_ota_header_info.reserved3 = ETX_OTA_8BITS_RESET_VALUE;
    etx_ota_header_info.payload_type = p_ETX_OTA_api->ETX_OTA_Payload_Type;
    ret = send_etx_ota_header(p_ETX_OTA_api, teuniz_rs232_lib_comport, &etx_ota_header_info);

    /* If ETX OTA Header Type Packet could not be sent successfully, then try sending it one more time in case that the slave device was not hearing the host device for a Command. */
    if (ret != ETX_OTA_EC_OK)
    {
        if (payload_send_attempts++ == 0)
        {
            if (Fptr)
            {
                fclose(Fptr);
            }
            RS232_CloseComport(teuniz_rs232_lib_comport);
            usleep(p_ETX_OTA_api->try_again_sending_fwi_delay);
            ret = start_etx_ota_process(p_ETX_OTA_api);
        }
        return ret;
    }

    /* Sending Payload Data via one or more ETX OTA Data Type Packets correspondingly. */
    /** <b>Local variable size:</b> Indicates the number of bytes from the Payload that have been send to the external device (i.e., the device that is desired to connect to via the \p comport param) via ETX OTA Data Type Packets. */
    uint16_t size = 0;
    for (uint32_t i=0; i<payload_size; )
    {
        if ((payload_size-i) >= ETX_OTA_DATA_MAX_SIZE)
        {
            size = ETX_OTA_DATA_MAX_SIZE;
        }
        else
        {
            size = payload_size - i;
        }
        ret = send_etx_ota_data(p_ETX_OTA_api, teuniz_rs232_lib_comport, &PAYLOAD_CONTENT[i], size);
        if (ret != ETX_OTA_EC_OK)
        {
            return ret;
        }
        i += size;
    }

    /* Send ETX OTA End Command. */
    ret = send_etx_ota_end(p_ETX_OTA_api, teuniz_rs232_lib_comport);
    if (ret != ETX_OTA_EC_OK)
    {
        return ret;
    }

    if (Fptr)
    {
        fclose(Fptr);
    }
    RS232_CloseComport(teuniz_rs232_lib_comport);

    return ETX_OTA_EC_OK;
}

/** @} */
