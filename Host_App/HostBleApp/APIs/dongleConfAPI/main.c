/**@file
 *
 * @defgroup main_program Main Program
 * @{
 *
 * @brief	This module contains the main application code.
 *
 * @details	The purpose of this application program is to act as an API that is to receive some RS-232 Serial Port
 *          Configurations from the user via Command Line Arguments in order to then configure the requested HM-10 BT
 *          Device (also via the Command Line Arguments) in Central Mode.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 29, 2023.
 */

#include "dongle_configurator.h" // Custom Dongle Configurator Library.
#include <stdio.h>	// Library from which "printf()" is located at.
#include <string.h> // Library from which the "strcpy_s()", "memset()" and "memcpy()" functions are located at.
#include <stdlib.h> // Library from which the "atoi()" function is located at.

/**@brief   Main function of the main application program whose purpose is to act as an API that is to receive some
 *          RS-232 Serial Port Configurations from the user via Command Line Arguments in order to then configure the
 *          requested HM-10 BT Device (also via the Command Line Arguments) in Central Mode.
 *
 * @param argc      Contains the total number of Command Line Arguments that are given whenever executing the program
 *                  contained in this file. The total number of arguments consider here will also include the command
 *                  executed on the terminal window to run the program contained in this file. For example, if the
 *                  Terminal Window Command used on the terminal window, in a Windows O.S., was
 *                  \code .\APIs\dongleConfAPI\ETX_OTA_Protocol_BLE_API 4 9600 8 N 1 0 1000 500000 \endcode
 *                  then
 *                  \code argc = 9 \endcode
 * @param[in] argv  Holds the actual Command Line Arguments that are given whenever executing the program contained in
 *                  this file. Each argument will be allocated in a different index within the \p argv param, but were
 *                  the first index will contain the command executed on the terminal window to run the program
 *                  contained in this file and then the actual Command Line Arguments will be placed subsequently. For
 *                  example, if the Terminal Window Command used on the terminal window was
 *                  \code .\APIs\dongleConfAPI\ETX_OTA_Protocol_BLE_API 4 9600 8 N 1 0 1000 500000 \endcode
 *                  then
 *                  \code argv[9] = {".\APIs\dongleConfAPI\ETX_OTA_Protocol_BLE_API" "4" "9600" "8" "N" "1" "0" "1000" "500000"} \endcode
 *
 * @note    This @ref main function expects to be given the following Command Line Arguments via the \p argv param:<br>
 *          <ul>
 *              <li>Command Line Argument index 0 = @ref TERMINAL_WINDOW_EXECUTION_COMMAND </li>
 *              <li>Command Line Argument index 1 = @ref COMPORT_NUMBER </li>
 *              <li>Command Line Argument index 2 = @ref RS232_BAUDRATE </li>
 *              <li>Command Line Argument index 3 = @ref RS232_MODE_DATA_BITS </li>
 *              <li>Command Line Argument index 4 = @ref RS232_MODE_PARITY </li>
 *              <li>Command Line Argument index 5 = @ref RS232_MODE_STOPBITS </li>
 *              <li>Command Line Argument index 6 = @ref RS232_IS_FLOW_CONTROL </li>
 *              <li>Command Line Argument index 7 = @ref SEND_PACKET_BYTES_DELAY </li>
 *              <li>Command Line Argument index 8 = @ref TEUNIZ_LIB_POLL_COMPORT_DELAY </li>
 *          </ul>
 *          where each of the values, given in String Type, are expected to be humanly read as the actual values that
 *          they should have. For example, if Command Line Argument index 1 String Type value = "1", then it will mean
 *          that the other end of the API requested the @ref main_program to set the Comport Number 1. Another example
 *          would be: if Command Line Argument index 7 String Type value = "1024", then it will mean that the other end
 *          of the API requested the @ref main_program to set a delay of 1024 microseconds each time after a byte of
 *          data is send to the HM-10 BT Device.
 *
 * @retval  DONGLE_CONFIGURATOR_EC_OK
 * @retval  DONGLE_CONFIGURATOR_INV_CMD_LINE_ARG
 * @retval  DONGLE_CONFIGURATOR_EC_OPEN_COM_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_INIT_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_AT_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_RENEW_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_ROLE_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_IMME_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_NOTI_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_MODE_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_RESET_CMD_ERR
 *
 * @note    For more details on the returned values, see @ref DONGLE_CONFIGURATOR_Status .
 *
 * @note    This function also sends the returned value (i.e., retval) to the other program that called the API of the
 *          @ref main_program via the Stream (i.e., via the @ref printf function), since that is the means of
 *          communication between the @ref main_program and the other program previously mentioned.
 *
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 29, 2023.
 */
int main(int argc, char *argv[])
{
    /** <b>Local variable dongle_configurator_api:</b> @ref DONGLE_CONFIGURATOR_API_t struct where all the data received by the API from the @ref main_program will be stored at. */
    DONGLE_CONFIGURATOR_API_t dongle_configurator_api;
    /** <b>Local variable ret:</b> Used to hold the exception code value returned by a @ref DONGLE_CONFIGURATOR_Status function type. */
    DONGLE_CONFIGURATOR_Status ret;
    /** <b>Local variable tmp_rs232_mode_data_bits:</b> Used to temporarily hold the value of the @ref RS232_MODE_DATA_BITS Command Line Argument. @note The size of this variable is 100 more times than needed to make sure that any undesired \c NULL value is captured in here in order to prevent Memory Overflow whenever populating this variable via the @ref strcpy_s function, which is what would happen if we were to directly populate this field into the @ref dongle_configurator_api local variable struct with the @ref strcpy_s function. */
    char tmp_rs232_mode_data_bits[100];
    /** <b>Local variable tmp_rs232_mode_parity:</b> Used to temporarily hold the value of the @ref RS232_MODE_PARITY Command Line Argument. @note The size of this variable is 100 more times than needed to make sure that any undesired \c NULL value is captured in here in order to prevent Memory Overflow whenever populating this variable via the @ref strcpy_s function, which is what would happen if we were to directly populate this field into the @ref dongle_configurator_api local variable struct with the @ref strcpy_s function. */
    char tmp_rs232_mode_parity[100];
    /** <b>Local variable tmp_rs232_mode_stopbits:</b> Used to temporarily hold the value of the @ref RS232_MODE_STOPBITS Command Line Argument. @note The size of this variable is 100 more times than needed to make sure that any undesired \c NULL value is captured in here in order to prevent Memory Overflow whenever populating this variable via the @ref strcpy_s function, which is what would happen if we were to directly populate this field into the @ref dongle_configurator_api local variable struct with the @ref strcpy_s function. */
    char tmp_rs232_mode_stopbits[100];

    /* Validate the Command Line Arguments given by the user. */
    if (argc != 9)
    {
        printf("%d\n", DONGLE_CONFIGURATOR_EC_INV_CMD_LINE_ARG);
        return DONGLE_CONFIGURATOR_EC_INV_CMD_LINE_ARG;
    }

    /* Get the API data received via the Command Line Arguments and populate them into the Dongle Configurator API structure. */
    // NOTE: "atoi()" converts a string containing numbers to its integer type equivalent so that it represents the
    //       literal string numbers given (e.g., atoi("1346") = (int) 1346).
    dongle_configurator_api.comport = atoi(argv[COMPORT_NUMBER]);
    dongle_configurator_api.rs232_baudrate = atoi(argv[RS232_BAUDRATE]);
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
    dongle_configurator_api.rs232_mode_data_bits = tmp_rs232_mode_data_bits[0];
    strcpy_s(tmp_rs232_mode_parity, 100, argv[RS232_MODE_PARITY]);
    dongle_configurator_api.rs232_mode_parity = tmp_rs232_mode_parity[0];
    strcpy_s(tmp_rs232_mode_stopbits, 100, argv[RS232_MODE_STOPBITS]);
    dongle_configurator_api.rs232_mode_stopbits = tmp_rs232_mode_stopbits[0];
    dongle_configurator_api.rs232_is_flow_control = atoi(argv[RS232_IS_FLOW_CONTROL]);
    dongle_configurator_api.send_packet_bytes_delay = atoi(argv[SEND_PACKET_BYTES_DELAY]);
    dongle_configurator_api.teuniz_lib_poll_comport_delay = atoi(argv[TEUNIZ_LIB_POLL_COMPORT_DELAY]);

    /* Start the Dongle Configurator Process to configure the specified HM-10 BT Device in Central Mode. */
    ret = start_dongle_configurator(&dongle_configurator_api);

    printf("%d\n", ret);
    return ret;
}

/** @} */
