/** @addtogroup dongle_configurator
 * @{
 */

#include "dongle_configurator.h"
#include "RS232/rs232.h" // Library for using RS232 protocol.
#include "HM10_ble_driver/Inc/hm10_ble_driver.h" // Custom Mortrack's Library to be able to send and/or receive data to/from the HM-10 BT Device.

/**@brief   Sends the corresponding AT Commands to the HM-10 Device to first initialize the HM-10 Library and to then
 *          configure the HM-10 Device so that it can be used as a Bluetooth Dongle Device.
 *
 * @note    The RS-232 Serial Protocol to be used must be opened independently to this function via the
 *          @ref RS232_OpenComport function from the Teuniz Library.
 *
 * @param[in] p_dongle_configurator_api Should hold all the data received via the API of the @ref main_program .
 *
 * @retval  DONGLE_CONFIGURATOR_EC_OK
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_INIT_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_AT_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_RENEW_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_ROLE_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_IMME_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_NOTI_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_MODE_CMD_ERR
 * @retval  DONGLE_CONFIGURATOR_EC_BLE_RESET_CMD_ERR
 *
 * @author  César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 30, 2023.
 */
static DONGLE_CONFIGURATOR_Status send_hm10_central_mode_cmds(DONGLE_CONFIGURATOR_API_t *p_dongle_configurator_api);

static DONGLE_CONFIGURATOR_Status send_hm10_central_mode_cmds(DONGLE_CONFIGURATOR_API_t *p_dongle_configurator_api)
{
    /** <b>Local variable ret:</b> Used to hold the exception code value returned by either a @ref HM10_Status or a @ref HM10_BT_Connection_Status function type. */
    uint8_t ret;

    /* Initialize the HM-10 Library Module. */
    // NOTE:    The "connect_to_address_timeout" argument of the following @ref init_hm10_module function will be set to
    //          zero since that value will not matter for this program/project due to that we will not make the HM-10 BT
    //          Device to connect to any other remote BT Device.
    ret = init_hm10_module(p_dongle_configurator_api->comport, p_dongle_configurator_api->send_packet_bytes_delay, p_dongle_configurator_api->teuniz_lib_poll_comport_delay, 0);
    if (ret != HM10_EC_OK)
    {
        return DONGLE_CONFIGURATOR_EC_BLE_INIT_ERR;
    }

    /* Send Test Command to the HM-10 BT Device to make sure that the BLE Dongle Device is disconnected from any previously on-going Bluetooth Connection.  */
    ret = disconnect_hm10_from_bt_address();
    if (ret == HM10_BT_Connection_Status_Unknown)
    {
        return DONGLE_CONFIGURATOR_EC_BLE_AT_CMD_ERR;
    }

    /* Send Renew Command to the HM-10 BT Device to set its configuration settings to factory default. */
    ret = send_hm10_renew_cmd();
    if (ret != HM10_EC_OK)
    {
        return DONGLE_CONFIGURATOR_EC_BLE_RENEW_CMD_ERR;
    }

    /* Send IMME Command to the HM-10 BT Device to make that Device to respond to AT Commands after powering-on and to not do anything else until AT+START, AT+CON or AT+CONNL Commands are given to it. */
    ret = set_hm10_module_work_type(HM10_Module_Work_Type_1);
    if (ret != HM10_EC_OK)
    {
        return DONGLE_CONFIGURATOR_EC_BLE_IMME_CMD_ERR;
    }

    /* Send NOTI Command to the HM-10 BT Device to Enable the Notify Information Mode in it. */
    ret = set_hm10_notify_information_mode(HM10_Notify_ENABLED);
    if (ret != HM10_EC_OK)
    {
        return DONGLE_CONFIGURATOR_EC_BLE_NOTI_CMD_ERR;
    }

    /* Send Mode Command to the HM-10 BT Device to configure it with the Transmission Module Work Mode. */
    ret = set_hm10_module_work_mode(HM10_Transmission_Mode);
    if (ret != HM10_EC_OK)
    {
        return DONGLE_CONFIGURATOR_EC_BLE_MODE_CMD_ERR;
    }

    /* Send Role Command to the HM-10 BT Device to set its Bluetooth Role to Central Mode. */
    // TODO: Pending to learn why the problem of the following Note is occurring and to also fix it.
    // NOTE:    Although the AT+ROLE Command is suggested to be sent before the AT+IMME Command, I decided to execute it
    //          here because for some reason I could not figure out, no AT Command works after sending the AT+ROLE until
    //          the end/termination of this program, where even by closing the Serial Port and re-opening it wouldn't
    //          help at all. Because of this same reason, I don't send the AT+RESET Command at the end of all the AT
    //          Commands sent to the HM-10 BT Device.
    ret = set_hm10_role(HM10_Role_Central);
    if (ret != HM10_EC_OK)
    {
        return DONGLE_CONFIGURATOR_EC_BLE_ROLE_CMD_ERR;
    }

    /* Send the Reset Command to the HM-10 BT Device with the already given new configurations. */
    /*
    ret = send_hm10_reset_cmd();
    if (ret != HM10_EC_OK)
    {
        return DONGLE_CONFIGURATOR_EC_BLE_RESET_CMD_ERR;
    }
    */

    return DONGLE_CONFIGURATOR_EC_OK;
}

DONGLE_CONFIGURATOR_Status start_dongle_configurator(DONGLE_CONFIGURATOR_API_t *p_dongle_configurator_api)
{
    /* Define the Core Local Variables of this function. */
    /** <b>Local variable teuniz_rs232_lib_comport:</b> Should hold the converted value of the actual comport that was requested by the user but into its equivalent for the @ref teuniz_rs232_library (For more details, see the Table from @ref teuniz_rs232_library ). */
    int teuniz_rs232_lib_comport;
    /** <b>Local variable mode:</b> Used to hold the character values for defining the desired Databits, Parity, Stopbit and to enable/disable the Flow Control, in that orderly fashion, in order to use them for the RS232 Protocol configuration process. @note The additional last value of 0 is required by the @ref teuniz_rs232_library to mark the end of the array. */
    char mode[] = {p_dongle_configurator_api->rs232_mode_data_bits, p_dongle_configurator_api->rs232_mode_parity, p_dongle_configurator_api->rs232_mode_stopbits, 0};
    /** <b>Local variable ret:</b> Used to hold the exception code value returned by a @ref DONGLE_CONFIGURATOR_Status function type. */
    DONGLE_CONFIGURATOR_Status ret;

    /* Get the equivalent of the requested COM port Number but as requested by the Teuniz RS232 Library. */
    teuniz_rs232_lib_comport = p_dongle_configurator_api->comport - 1;

    /* Open RS232 Comport that was requested by the user. */
    if (RS232_OpenComport(teuniz_rs232_lib_comport, p_dongle_configurator_api->rs232_baudrate, mode, p_dongle_configurator_api->rs232_is_flow_control))
    {
        return DONGLE_CONFIGURATOR_EC_OPEN_COM_ERR;
    }

    /* Configure the HM-10 BT Device to work in Central Mode. */
    ret = send_hm10_central_mode_cmds(p_dongle_configurator_api);
    if (ret != DONGLE_CONFIGURATOR_EC_OK)
    {
        return ret;
    }

    /* Release the Serial Port that has just been used before terminating the program. */
    RS232_CloseComport(teuniz_rs232_lib_comport);

    return DONGLE_CONFIGURATOR_EC_OK;
}

/** @} */
