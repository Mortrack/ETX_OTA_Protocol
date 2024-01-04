/**@file
 *
 * @defgroup main_program Main Program
 * @{
 *
 * @brief	This module contains the main application code.
 *
 * @details	The purpose of this application program is to act as an API that is to receive some ETX OTA Payload Data
 *          from the user via Command Line Arguments to then send it to a user specified external device by using the
 *          ETX OTA Protocol.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 27, 2023.
 */

#include "etx_ota_protocol_host.h" // Custom library that contains the Mortrack's ETX OTA Protocol.
#include <stdio.h>	// Library from which "printf()" is located at.
#include <string.h> // Library from which the "strcpy_s()", "memset()" and "memcpy()" functions are located at.
#include <stdlib.h> // Library from which the "atoi()" function is located at.

/**@brief   Main function of the main application program whose purpose is to act as an API that is to receive some ETX
 *          OTA Payload Data from the user via Command Line Arguments to then send it to a user specified device by
 *          using the ETX OTA Protocol.
 *
 * @param argc      Contains the total number of Command Line Arguments that are given whenever executing the program
 *                  contained in this file. The total number of arguments consider here will also include the command
 *                  executed on the terminal window to run the program contained in this file. For example, if the
 *                  Terminal Window Command used on the terminal window, in a Windows O.S., was
 *                  \code .\APIs\blePcToolAPI\ETX_OTA_Protocol_BLE_API 4 18 "Cesar Miranda Meza" 2 1024 34 86 9600 8 N 1 0 1000 500000 5000000 11000000 "0017EA090909" \endcode
 *                  then
 *                  \code argc = 18 \endcode
 * @param[in] argv  Holds the actual Command Line Arguments that are given whenever executing the program contained in
 *                  this file. Each argument will be allocated in a different index within the \p argv param, but were
 *                  the first index will contain the command executed on the terminal window to run the program
 *                  contained in this file and then the actual Command Line Arguments will be placed subsequently. For
 *                  example, if the Terminal Window Command used on the terminal window was
 *                  \code .\APIs\blePcToolAPI\ETX_OTA_Protocol_BLE_API 4 0 "C:\Users\cmira\OneDrive\Escritorio\Bootloader Project v0.4\PcTool_App\PcTool\Application_Firmware.bin" 0 1024 34 86 9600 8 N 1 0 1000 500000 5000000 11000000 "0017EA090909" \endcode
 *                  then
 *                  \code argv[18] = {".\APIs\blePcToolAPI\ETX_OTA_Protocol_BLE_API" "4" "0" "C:\Users\cmira\OneDrive\Escritorio\Bootloader Project v0.4\PcTool_App\PcTool\Application_Firmware.bin" "0" "1024" "34" "86" "9600" "8" "N" "1" "0" "1000" "500000" "5000000" "11000000" "0017EA090909"} \endcode
 *
 * @note    This @ref main function expects to be given the following Command Line Arguments via the \p argv param:<br>
 *          <ul>
 *              <li>Command Line Argument index 0 = @ref TERMINAL_WINDOW_EXECUTION_COMMAND </li>
 *              <li>Command Line Argument index 1 = @ref COMPORT_NUMBER </li>
 *              <li>Command Line Argument index 2 = @ref PAYLOAD_SIZE </li>
 *              <li>Command Line Argument index 3 = @ref PAYLOAD_PATH_OR_DATA </li>
 *              <li>Command Line Argument index 4 = @ref ETX_OTA_PAYLOAD_TYPE </li>
 *              <li>Command Line Argument index 5 = @ref FLASH_PAGE_SIZE_IN_BYTES </li>
 *              <li>Command Line Argument index 6 = @ref ETX_BL_PAGE_SIZE </li>
 *              <li>Command Line Argument index 7 = @ref ETX_APP_PAGE_SIZE </li>
 *              <li>Command Line Argument index 8 = @ref RS232_BAUDRATE </li>
 *              <li>Command Line Argument index 9 = @ref RS232_MODE_DATA_BITS </li>
 *              <li>Command Line Argument index 10 = @ref RS232_MODE_PARITY </li>
 *              <li>Command Line Argument index 11 = @ref RS232_MODE_STOPBITS </li>
 *              <li>Command Line Argument index 12 = @ref RS232_IS_FLOW_CONTROL </li>
 *              <li>Command Line Argument index 13 = @ref SEND_PACKET_BYTES_DELAY </li>
 *              <li>Command Line Argument index 14 = @ref TEUNIZ_LIB_POLL_COMPORT_DELAY </li>
 *              <li>Command Line Argument index 15 = @ref TRY_AGAIN_SENDING_FWI_DELAY </li>
 *              <li>Command Line Argument index 16 = @ref HM10_CONNECT_TO_ADDRESS_TIMEOUT </li>
 *              <li>Command LIne Argument index 17 = @ref BLUETOOTH_ADDRESS </li>
 *          </ul>
 *          where each of the values, given in String Type, are expected to be humanly read as the actual values that
 *          they should have. For example, if Command Line Argument index 1 String Type value = "1", then it will mean
 *          that the other end of the API requested the @ref main_program to set the Comport Number 1. Another example
 *          would be: if Command Line Argument index 4 String Type value = "1024", then it will mean that the other end
 *          of the API requested the @ref main_program to set the Flash Page Size to be 1024 bytes.
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
 *
 * @note    For more details on the returned values, see @ref ETX_OTA_Status .
 *
 * @note    This function also sends the returned value (i.e., retval) to the other program that called the API of the
 *          @ref main_program via the Stream (i.e., via the @ref printf function), since that is the means of
 *          communication between the @ref main_program and the other program previously mentioned.
 *
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 27, 2023.
 */
int main(int argc, char *argv[])
{
    /** <b>Local variable ETX_OTA_api:</b> @ref ETX_OTA_API_t struct where all the data received by the API from the @ref main_program will be stored in. */
    ETX_OTA_API_t ETX_OTA_api;
    /** <b>Local variable ret:</b> Used to hold the exception code value returned by a @ref ETX_OTA_Status function type. */
    ETX_OTA_Status ret;
    /** <b>Local variable tmp_rs232_mode_data_bits:</b> Used to temporarily hold the value of the @ref RS232_MODE_DATA_BITS Command Line Argument. @note The size of this variable is 100 more times than needed to make sure that any undesired \c NULL value is captured in here in order to prevent Memory Overflow whenever populating this variable via the @ref strcpy_s function, which is what would happen if we were to directly populate this field into the @ref ETX_OTA_api local variable struct with the @ref strcpy_s function. */
    char tmp_rs232_mode_data_bits[100];
    /** <b>Local variable tmp_rs232_mode_parity:</b> Used to temporarily hold the value of the @ref RS232_MODE_PARITY Command Line Argument. @note The size of this variable is 100 more times than needed to make sure that any undesired \c NULL value is captured in here in order to prevent Memory Overflow whenever populating this variable via the @ref strcpy_s function, which is what would happen if we were to directly populate this field into the @ref ETX_OTA_api local variable struct with the @ref strcpy_s function. */
    char tmp_rs232_mode_parity[100];
    /** <b>Local variable tmp_rs232_mode_stopbits:</b> Used to temporarily hold the value of the @ref RS232_MODE_STOPBITS Command Line Argument. @note The size of this variable is 100 more times than needed to make sure that any undesired \c NULL value is captured in here in order to prevent Memory Overflow whenever populating this variable via the @ref strcpy_s function, which is what would happen if we were to directly populate this field into the @ref ETX_OTA_api local variable struct with the @ref strcpy_s function. */
    char tmp_rs232_mode_stopbits[100];
    /** <b>Local constant TMP_BT_ADDR_SIZE:</b> Used to hold the size of the @ref tmp_bt_addr local variable. */
    const short int TMP_BT_ADDR_SIZE = HM10_BT_ADDR_SIZE*100;
    /** <b>Local variable tmp_bt_addr:</b> Used to temporarily hold the value of the @ref BLUETOOTH_ADDRESS Command Line Argument. @note The size of this variable is 100 more times than needed to make sure that any undesired \c NULL value is captured in here in order to prevent Memory Overflow whenever populating this variable via the @ref strcpy_s function, which is what would happen if we were to directly populate this field into the @ref ETX_OTA_api local variable struct with the @ref strcpy_s function. */
    char tmp_bt_addr[TMP_BT_ADDR_SIZE];

    /* Validate the Command Line Arguments given by the user. */
    if (argc != 18)
    {
        printf("%d\n", ETX_OTA_EC_INV_CMD_LINE_ARG);
        return ETX_OTA_EC_INV_CMD_LINE_ARG;
    }

    /* Get the API data received via the Command Line Arguments and populate them into the ETX OTA API structure. */
    // NOTE: "atoi()" converts a string containing numbers to its integer type equivalent so that it represents the
    //       literal string numbers given (e.g., atoi("1346") = (int) 1346).
    ETX_OTA_api.comport = atoi(argv[COMPORT_NUMBER]);
    ETX_OTA_api.payload_size = atoi(argv[PAYLOAD_SIZE]);
    memset(ETX_OTA_api.payload_path_or_data, 0, PAYLOAD_PATH_OR_DATA_MAX_SIZE);
    strcpy_s((char *) &ETX_OTA_api.payload_path_or_data, PAYLOAD_PATH_OR_DATA_MAX_SIZE, argv[PAYLOAD_PATH_OR_DATA]);
    ETX_OTA_api.ETX_OTA_Payload_Type = atoi(argv[ETX_OTA_PAYLOAD_TYPE]);
    ETX_OTA_api.flash_page_size_in_bytes = atoi(argv[FLASH_PAGE_SIZE_IN_BYTES]);
    ETX_OTA_api.ETX_bl_page_size = atoi(argv[ETX_BL_PAGE_SIZE]);
    ETX_OTA_api.ETX_app_page_size = atoi(argv[ETX_APP_PAGE_SIZE]);
    ETX_OTA_api.rs232_baudrate = atoi(argv[RS232_BAUDRATE]);
    // NOTE: I do not really understand how the "SizeInBytes" argument of the "strcpy_s()" function really works, but it
    //       seems like whenever trying to get a Command Line Argument's data with that function, some "garbage" null
    //       data may be picked up by that function before getting the actual data. Therefore, to guarantee getting the
    //       actual data, what worked for me was to request getting an input data that is several times larger in bytes
    //       than it actually is. However, by doing that, memory overflow might occur in some cases, since some \c NULL
    //       bytes after the actual input data might be retrieved as well, making the "strcpy_s()" function to cause
    //       this memory overflow that was just mentioned. Therefore, a solution for that problem is to create local
    //       variables with larger sizes than the actually expected input data to temporarily hold that data in those
    //       local variables to then pass it to whatever struct into which you want to actually hold that data so that
    //       inner fields of that struct are not rewritten with null values or so that the program does not corrupt or
    //       malfunction due to memory overflow in case this solution is not addressed.
    strcpy_s(tmp_rs232_mode_data_bits, 100, argv[RS232_MODE_DATA_BITS]);
    ETX_OTA_api.rs232_mode_data_bits = tmp_rs232_mode_data_bits[0];
    strcpy_s(tmp_rs232_mode_parity, 100, argv[RS232_MODE_PARITY]);
    ETX_OTA_api.rs232_mode_parity = tmp_rs232_mode_parity[0];
    strcpy_s(tmp_rs232_mode_stopbits, 100, argv[RS232_MODE_STOPBITS]);
    ETX_OTA_api.rs232_mode_stopbits = tmp_rs232_mode_stopbits[0];
    ETX_OTA_api.rs232_is_flow_control = atoi(argv[RS232_IS_FLOW_CONTROL]);
    ETX_OTA_api.send_packet_bytes_delay = atoi(argv[SEND_PACKET_BYTES_DELAY]);
    ETX_OTA_api.teuniz_lib_poll_comport_delay = atoi(argv[TEUNIZ_LIB_POLL_COMPORT_DELAY]);
    ETX_OTA_api.try_again_sending_fwi_delay = atoi(argv[TRY_AGAIN_SENDING_FWI_DELAY]);
    ETX_OTA_api.hm10_connect_to_address_timeout = atoi(argv[HM10_CONNECT_TO_ADDRESS_TIMEOUT]);
    strcpy_s(tmp_bt_addr, TMP_BT_ADDR_SIZE, argv[BLUETOOTH_ADDRESS]);
    memcpy(ETX_OTA_api.bt_addr, tmp_bt_addr, HM10_BT_ADDR_SIZE);

    /* Start ETX OTA Process to send the user-requested Payload to the specified external device. */
    ret = start_etx_ota_process(&ETX_OTA_api);

    printf("%d\n", ret);
    return ret;
}

/** @} */
