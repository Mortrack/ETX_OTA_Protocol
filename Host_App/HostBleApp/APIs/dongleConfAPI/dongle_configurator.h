/**@file
 *
 * @defgroup dongle_configurator Dongle Configurator module
 * @{
 *
 * @brief	This module provides the functions required to enable the application to be able to connect and configure an
 *          HM-10 BT Device as Central Mode via the @ref hm10_ble .
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 29, 2023.
 */

#ifndef INC_DONGLE_CONFIGURATOR_H_
#define INC_DONGLE_CONFIGURATOR_H_

#include <stdint.h> // This library contains the aliases: uint8_t, uint16_t, uint32_t, etc.

/**@brief	Dongle Configurator Exception codes.
 *
 * @details	These Exception Codes are returned by the functions of the @ref dongle_configurator module to indicate the
 *          resulting status of having executed the process contained in each of those functions. For example, to
 *          indicate that the process executed by a certain function was successful or that it has failed.
 */
typedef enum
{
    DONGLE_CONFIGURATOR_EC_OK                           = 0U,   //!< Dongle Configurator Process was successful.
    DONGLE_CONFIGURATOR_EC_STOP                         = 1U,   //!< Dongle Configurator Process has been stopped.
    DONGLE_CONFIGURATOR_EC_NR		                    = 2U,	//!< Dongle Configurator Process has concluded with no response from Host.
    DONGLE_CONFIGURATOR_EC_NA                           = 3U,   //!< Dongle Configurator Process Not Applicable.
    DONGLE_CONFIGURATOR_EC_ERR                          = 4U,   //!< Dongle Configurator Process has failed.
    DONGLE_CONFIGURATOR_EC_INV_CMD_LINE_ARG             = 5U,   //!< Dongle Configurator Process API side received invalid number of Command Line Arguments.
    // NOTE: Do not use Enum value 6 so that it can be compatible with the ETX_OTA_Status Enum of the ETX OTA Protocol.
    DONGLE_CONFIGURATOR_EC_OPEN_COM_ERR                 = 7U,   //!< Dongle Configurator Process has failed to Open requested RS232 Comport.
    // NOTE: Do not use Enum values 8 up to 20 so that it can be compatible with the ETX_OTA_Status Enum of the ETX OTA Protocol.
    DONGLE_CONFIGURATOR_EC_BLE_INIT_ERR                 = 21U,  //!< Dongle Configurator Process Bluetooth Initialization has failed due to invalid comport value.
    DONGLE_CONFIGURATOR_EC_BLE_AT_CMD_ERR               = 22U,  //!< Dongle Configurator Process Bluetooth Disconnect Command has failed.
    DONGLE_CONFIGURATOR_EC_BLE_TYPE_CMD_ERR             = 23U,  //!< Dongle Configurator Process Bluetooth Set Pin Code Mode Command has failed.
    DONGLE_CONFIGURATOR_EC_BLE_RESET_CMD_ERR            = 24U,  //!< Dongle Configurator Process Bluetooth Reset Command has failed.
    DONGLE_CONFIGURATOR_EC_BLE_CONN_CMD_ERR             = 25U,  //!< Dongle Configurator Process Bluetooth Connect-To-Address Command has failed.
    DONGLE_CONFIGURATOR_EC_BLE_RENEW_CMD_ERR            = 26U,  //!< Dongle Configurator Process Bluetooth Renew Command has failed.
    DONGLE_CONFIGURATOR_EC_BLE_ROLE_CMD_ERR             = 27U,  //!< Dongle Configurator Process Bluetooth Role Command has failed.
    DONGLE_CONFIGURATOR_EC_BLE_IMME_CMD_ERR             = 28U,  //!< Dongle Configurator Process Bluetooth IMME Command has failed.
    DONGLE_CONFIGURATOR_EC_BLE_NOTI_CMD_ERR             = 29U,  //!< Dongle Configurator Process Bluetooth NOTI Command has failed.
    DONGLE_CONFIGURATOR_EC_BLE_MODE_CMD_ERR             = 30U   //!< Dongle Configurator Process Bluetooth MODE Command has failed.
} DONGLE_CONFIGURATOR_Status;

/**@brief	Command Line Arguments definitions.
 *
 * @details	These definitions are used as identifiers for the Command Line Arguments that should be received right after
 *          the resulting compiled file from this @ref dongle_configurator is executed via the terminal window.
 *
 * @note	For more details see @ref main and the Readme.md file of the project that uses the
 *          @ref dongle_configurator .
 */
typedef enum
{
    TERMINAL_WINDOW_EXECUTION_COMMAND   = 0U,    //!< Command Line Argument Index 0, which should contain the string of the literal terminal window command used by the user to execute the @ref dongle_configurator program.
    COMPORT_NUMBER                      = 1U,    //!< Command Line Argument Index 1, which should contain the Comport with which the user wants the @ref dongle_configurator program to establish a connection with via RS232 protocol.
    RS232_BAUDRATE                      = 2U,    //!< Command Line Argument Index 2, which should contain the Baud rate that wants to be used on the Serial Port (i.e, @ref Command_Line_Arguments::COMPORT_NUMBER ) with which a communication with the HM-10 BT Device will be made. @note For valid values, see @ref teuniz_rs232_library .
    RS232_MODE_DATA_BITS                = 3U,    //!< Command Line Argument Index 3, which should contain the Data-bits with which we want the host to run the RS232 protocol. @note For valid values, see @ref teuniz_rs232_library .
    RS232_MODE_PARITY                   = 4U,    //!< Command Line Argument Index 4, which should contain the Parity with which we want the host to run the RS232 protocol. @note For valid values, see @ref teuniz_rs232_library .
    RS232_MODE_STOPBITS                 = 5U,    //!< Command Line Argument Index 5, which should contain the Stop-bits with which we want the host to run the RS232 protocol. @note For valid values, see @ref teuniz_rs232_library .
    RS232_IS_FLOW_CONTROL               = 6U,    //!< Command Line Argument Index 6, which should contain the Flag value to indicate whether we want the host to run the RS232 protocol with Flow Control with a \c "1" or otherwise with a \c "0" . @note For valid values, see @ref teuniz_rs232_library .
    SEND_PACKET_BYTES_DELAY             = 7U,    //!< Command Line Argument Index 7, which should contain the desired delay in microseconds to be requested after having send a byte to the HM-10 BT Device via the @ref hm10_ble .
    TEUNIZ_LIB_POLL_COMPORT_DELAY       = 8U,    //!< Command Line Argument Index 8, which should contain the desired delay in microseconds that is to be requested to apply each time before calling the @ref get_hm10_ota_data function of the @ref hm10_ble .
} Command_Line_Arguments;

/**@brief	Dongle Configurator API parameters structure.
 *
 * @details	This structure contains all the fields that are transacted with the API part of the @ref main_program
 *          program and whatever external program with which there is some data interaction with the API provided by
 *          @ref main_program .
 */
typedef struct {
    int comport;		                       //!< The actual comport that wants to be used for the RS232 protocol to connect to a desired HM-10 BT Device.
    uint32_t rs232_baudrate;                   //!< Chosen Baud Rate with which we want the host to run the RS232 protocol.
    uint32_t send_packet_bytes_delay;          //!< Delay in microseconds that is desired to be requested after having send a byte to the HM-10 BT Device via the @ref hm10_ble .
    uint32_t teuniz_lib_poll_comport_delay;    //!< Delay in microseconds that it is to be requested to apply each time before calling the @ref get_hm10_ota_data function of the @ref hm10_ble . @note The @ref teuniz_rs232_library suggests to place an interval of 100 milliseconds, but it did not worked for me that way. Instead, it worked for me with 500ms.
    uint8_t rs232_mode_data_bits;              //!< Chosen Data-bits string with which we want the host to run the RS232 protocol. @details The valid data-bits in the Teuniz RS232 Library are '5', '6', '7' and '8'.
    uint8_t rs232_mode_parity;                 //!< Chosen Parity string with which we want the host to run the RS232 protocol. @details The valid parity values in the Teuniz RS232 Library are:<br> - 'N' = None (i.e., No parity bit sent at all).<br> - 'O' = Odd (i.e., Set to mark if the number of 1s in data bits is odd.<br> - 'E' = Even (i.e., Set to space if the number of 1s in data bits is even.
    uint8_t rs232_mode_stopbits;               //!< Chosen Stop-bit string with which we want the host to run the RS232 protocol. @note The possible stopbit values in RS232 are '1' or '2'.
    uint8_t rs232_is_flow_control;             //!< Chosen Flow Control decimal value to indicate with a 1 that we want the host to run the RS232 protocol with Flow Control enabled, or otherwise with a decimal value of 0 to indicate to the host to not run the RS232 protocol with Flow Control.
} DONGLE_CONFIGURATOR_API_t;

/**@brief   Does all the required initializations, configurations and sends the required AT Commands to the desired
 *          HM-10 BT Device in order to configure it as Central Mode.
 *
 * @param[in] p_dongle_configurator_api Should hold all the data received via the API of the @ref main_program .
 *
 * @retval  DONGLE_CONFIGURATOR_EC_OK
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
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 29, 2023.
 */
DONGLE_CONFIGURATOR_Status start_dongle_configurator(DONGLE_CONFIGURATOR_API_t *p_dongle_configurator_api);

#endif /* INC_DONGLE_CONFIGURATOR_H_ */

/** @} */
