/**@file
 *
 * @defgroup main_program Main Program
 * @{
 *
 * @brief	This module contains the main application code.
 *
 * @details	The purpose of this application program is to receive some ETX OTA Payload Data from the user via Command
 *          Line Arguments to then send it to a user specified external device by using the ETX OTA Protocol.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	October 08, 2023.
 */

#include "etx_ota_protocol_host.h" // Custom library that contains the Mortrack's ETX OTA Protocol.
#include <stdio.h>	// Library from which "printf()" is located at.
#include <string.h> // Library from which the "strcpy_s()" function is located at.
#include <stdlib.h> // Library from which the "atoi()" function is located at.

/**@brief   Main function of the main application program whose purpose is to receive some ETX OTA Payload Data from the
 *          user via Command Line Arguments to then send it to a user specified device by using the ETX OTA Protocol.
 *
 * @param argc      Contains the total number of Command Line Arguments that are given whenever executing the program
 *                  contained in this file. The total number of arguments consider here will also include the command
 *                  executed on the terminal window to run the program contained in this file. For example, if the
 *                  Terminal Window Command used on the terminal window was
 *                  \code $./a.out 0 1 2 \endcode
 *                  then
 *                  \code argc = 4 \endcode
 * @param[in] argv  Holds the actual Command Line Arguments that are given whenever executing the program contained in
 *                  this file. Each argument will be allocated in a different index within the \p argv param, but were
 *                  the first index will contain the command executed on the terminal window to run the program
 *                  contained in this file and then the actual Command Line Arguments will be placed subsequently. For
 *                  example, if the Terminal Window Command used on the terminal window was
 *                  \code $./a.out 0 1 2 \endcode
 *                  then
 *                  \code argv[4] = {"./a.out", "0", "1", "2"} \endcode
 *
 * @note    This @ref main function expects to be given the following Command Line Arguments via the \p argv param:<br>
 *          <ul>
 *              <li>Command Line Argument index 0 = @ref TERMINAL_WINDOW_EXECUTION_COMMAND </li>
 *              <li>Command Line Argument index 1 = @ref COMPORT_NUMBER </li>
 *              <li>Command Line Argument index 2 = @ref PAYLOAD_PATH </li>
 *              <li>Command Line Argument index 3 = @ref ETX_OTA_Payload_t </li>
 *          </ul>
 * @note    Although this @ref main function requires the user to always populate a value for the Command Line Argument
 *          2, its value will only be used by this program whenever the value of the Command Line Argument index 3 is
 *          that of either a @ref ETX_OTA_Payload_t::ETX_OTA_Application_Firmware_Image or a @ref
 *          ETX_OTA_Payload_t::ETX_OTA_Bootloader_Firmware_Image . In addition, whenever the @ref
 *          ETX_OTA_Payload_t::ETX_OTA_Custom_Data type is given instead, then this program will send a fixed custom
 *          generated data that is formualted in the @ref start_etx_ota_process function.
 *
 * @retval  ETX_OTA_EC_OK
 * @retval  ETX_OTA_EC_ERR
 *
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @author 	EmbeTronicX (<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>STM32-Bootloader GitHub Repository under ETX_Bootloader_3.0 branch</a>)
 * @date    November 24, 2023.
 */
int main(int argc, char *argv[])
{
    /** <b>Local variable comport:</b> Should hold the actual comport that was requested by the user to be employed for the RS232 protocol. */
    int comport;
    /** <b>Local variable firmware_image_path:</b> File Path towards the Firmware Update Image that the user requested to load and send to the desired MCU for it to install that Image to itself. */
    char firmware_image_path[PAYLOAD_MAX_FILE_PATH_LENGTH];
    /** <b>Local variable ETX_OTA_Payload_Type:</b> Used to hold the Payload Type that is to be given by the user. */
    ETX_OTA_Payload_t ETX_OTA_Payload_Type;
    /** <b>Local variable ret:</b> Used to hold the exception code value returned by a @ref ETX_OTA_Status function type. */
    ETX_OTA_Status ret;

    /* Validate the Command Line Arguments given by the user. */
    printf("Getting Command Line Arguments given by the user...\n");
    if (argc != 4)
    {
        printf("ERROR: Expected 4 Command Line Argument definitions, but received %d instead.\n", argc);
        printf("Please feed the Terminal Window Execution Command, the COM PORT number, the Application Image and the ETX_OTA_Payload_t in that order...!!!\n");
        printf("Example: .\\etx_ota_app.exe 8 ..\\..\\Application\\Debug\\Blinky.bin 0");
        return ETX_OTA_EC_ERR;
    }

    /* Get the COM port Number that was specified by the user via the \p argv param. */
    // NOTE: "atoi()" converts a string containing numbers to its integer type equivalent so that it represents the literal string numbers given (e.g., atoi("1346") = (int) 1346).
    comport = atoi(argv[COMPORT_NUMBER]);

    /* Get the File Path towards the Firmware Update Image that the user requested to load and send to the desired MCU. */
    strcpy_s(firmware_image_path, PAYLOAD_MAX_FILE_PATH_LENGTH, argv[PAYLOAD_PATH]);

    /* Get the Payload Type of the Data that is going to be requested to our host machine to send to the desired MCU. */
    ETX_OTA_Payload_Type = atoi(argv[ETX_OTA_PAYLOAD_TYPE]);
    printf("Command Line Arguments have been successfully obtained.\n");

    /* Start ETX OTA Process to send the requested Payload to the specified external device by the user. */
    printf("Starting the ETX OTA Process with the requested Payload and the specified external device by the user...\n");
    ret = start_etx_ota_process(comport, firmware_image_path, ETX_OTA_Payload_Type);
    if (ret != ETX_OTA_EC_OK)
    {
        printf("ERROR: The ETX OTA Process has failed (ETX OTA Exception Code = %d).\n", ret);
    }
    else
    {
        printf("DONE: The ETX OTA Process has concluded successfully.\n");
    }

    return ret;
}

/** @} */
