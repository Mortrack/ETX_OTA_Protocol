/**@addtogroup ETX_OTA_Protocol_Interface
 * @{
 */

/**@file
 * 
 * @defgroup BLE_ETX_OTA_Protocol BLE ETX OTA Protocol Concrete Product module
 * @{
 * 
 * @brief   This module contains the variables and methods required to enable
 *          the application so that our host machine can initialize and send a
 *          desired Payload Data through the use of the ETX OTA Communication
 *          Protocol over the BLE Hardware Protocol to a desired slave device.
 * 
 * @details The following gives a friendly code example of how to use
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocol}
 *          Class:
 * @code
    // Send the Payload to the requested slave device with the requested Configurations and Hardware Protocol via the ETX OTA Protocol.
    short txOtaRs232ComportNumber = 3; // RS232 Comport from which the slave device is expected to be connected to.
    String payloadFilePath = "C:\\Users\\cmira\\OneDrive\\Escritorio\\Bootloader Project v0.4\\Host_BLE_App\\HostBleApp\\APIs\\PcTool\\Application_Firmware.bin"; // Windows O.S. Absolute File Path towards the Application Firmware Image that wants to be send to the desired slave device.
    short etxOtaPayloadType = (short) com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaPayloadType.ETX_OTA_Application_Firmware_Image.getNumber(); // = 0 because an Application Firmware Image Type is given.
    int baudrate = 9600; // Baud Rate at which it is desired that the Serial Port works with in order to communicate with the BLE Dongle Device.
    String slaveBtAddress = "0017EA090909"; // Bluetooth Address of the Remote Bluetooth Device with which we want our BLE Dongle Device to establish a Bluetooth Communication with.
    int hm10ConnectToAddressTimeout = 3000000; // HM-10 Connect To Address Timeout.
    int etxOtaApiTimeout = 1800; // = 30 ETX OTA API Timeout in minutes.
    EtxOtaProtocolFactory bleEtxOtaFact = new BleEtxOtaProtocolFactory(txOtaRs232ComportNumber, baudrate, slaveBtAddress, hm10ConnectToAddressTimeout);
    EtxOtaProtocol etxOtaApi = bleEtxOtaFact.sendPayload(payloadFilePath, etxOtaPayloadType, etxOtaApiTimeout);
    if (etxOtaApi.lastEtxOtaStatus() != com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OK) {
        System.out.println("ETX OTA Exception Code = " + etxOtaApi.lastEtxOtaStatus().getNumber());
        System.out.println("The requested Payload could not be sent to the slave device.");
    }
    else {
        System.out.println("The requested Payload has been successfully sent to the slave device.");
    }
 * @endcode
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 28, 2023.
 */

/* @brief   Package containing the all the Classes, methods, variables and code
 *          in general to initialize and use the ETX OTA Communication Protocol
 *          over a desired and available Hardware Protocol so that our host
 *          machine can communicate with a desired slave device.
 *
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 30, 2023.
 */
package com.mortrack.hostbleapp.etxotaprotocol;

import com.mortrack.hostbleapp.utils.TerminalProcess;
import java.util.logging.Level;
import java.util.logging.Logger;


/**@brief   This Class provides several methods to enable the application with
 *          the capability of communicating and sending some desired Payload
 *          Data through the use of the ETX OTA Communication Protocol over the
 *          BLE Hardware Protocol.
 * 
 * @details The way that this Class sends Payload Data to a desired slave device
 *          is by calling the PcTool BLE API made by Mortrack
 *          (cmirandameza3@hotmail.com) and, therefore, leaving all the heavy
 *          processing of data and ETX OTA Protocol interaction to that API,
 *          which is to report back by sending a corresponding ETX OTA Status
 *          Code as stated in
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus}
 *          in order to be able to know the result of the processes made by the
 *          PcTool BLE API.
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 28, 2023.
 */
public class BleEtxOtaProtocol implements EtxOtaProtocol {
    /**@brief   ETX OTA Status that was obtained from latest ETX OTA Transaction
     *          that was made between our host machine and the slave device.
     */
    private com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus lastEtxOtaStatus = EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OK;
    
    /**@brief   Flash Memory page size in bytes as defined by the MCU/MPU with
     *          which the Serial Port communication is to be established with.
     */
    private final short flashPageSizeInBytes ;
    
    /**@brief   Number of Flash Memory pages that have been designated for the
     *          Bootloader Firmware of the MCU/MPU with which the Serial Port
     *          communication is to be established with.
     */
    private final short etxBlPageSize;
    
    /**@brief   Number of Flash Memory pages that have been designated for the
     *          Application Firmware of the MCU/MPU with which the Serial Port
     *          communication is to be established with.
     */
    private final short etxAppPageSize;
    
    /**@brief   The actual comport that wants to be used for the RS232 protocol
     *          to connect to a desired external device.
     */
    private final short comportNumber;
    
    /**@brief   Chosen Baud-rate with which we want the host to run the RS232
     *          protocol.
     */
    private final int rs232Baudrate;
    
    /**@brief   Bluetooth Address of the Slave/External Device with which it is
     *          desired that our host machine establishes a Bluetooth
     *          Communication with.
     */
    private final String slaveBtAddress;
    
    /**@brief   Chosen Data-bits string with which we want the host to run the
     *          RS232 protocol.
     * 
     * @details The valid data-bits in the
     *          <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a>
     *          that are used in the PcTool API are '5', '6', '7' and '8'.
     */
    private final char rs232ModeDatabits;
    
    /**@brief   Chosen Parity string with which we want the host to run the
     *          RS232 protocol.
     * 
     * @details The valid parity values in the
     *          <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a>
     *          that are used in the PcTool API are:<br>
     *          - 'N' = None (i.e., No parity bit sent at all).<br>
     *          - 'O' = Odd (i.e., Set to mark if the number of 1s in data bits
     *            is odd.<br>
     *          - 'E' = Even (i.e., Set to space if the number of 1s in data
     *            bits is even.
     */
    private final char rs232ModeParity;
    
    /**@brief   Chosen Stop-bit string with which we want the host to run the
     *          RS232 protocol.
     * 
     * @note    The possible stop-bit values in RS232 are '1' or '2'.
     */
    private final char rs232ModeStopbits;
    
    /**@brief   Chosen Flow Control decimal value to indicate with a 1 that we
     *          want the host to run the RS232 protocol with Flow Control
     *          enabled, or otherwise with a decimal value of 0 to indicate to
     *          the host not to run the RS232 protocol with Flow Control.
     */
    private final short rs232IsFlowControl;
    
    /**@brief   Delay in microseconds that is desired to request before having
     *          send a byte of data from a certain ETX OTA Packet that is in
     *          process of being send to the MCU.
     */
    private final int sendPacketBytesDelay;
    
    /**@brief   Delay in microseconds that it is to be requested to apply each
     *          time before calling the RS232_PollComport() function of the
     *          <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a>
     *          that is used in the PcTool API.
     * 
     * @details For all the calls to the RS232_PollComport() function previously
     *          mentioned, this delay is called once, except in the
     *          send_etx_ota_data() function of the PcTool API, where this
     *          delay is called twice.
     * 
     * @note    The <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a>
     *          suggests to place an interval of 100 milliseconds, but it did
     *          not worked for me that way. Instead, it worked for me with
     *          500ms.
     */
    private final int teunizLibPollComportDelay;
    
    /**@brief   Delay in microseconds that it is to be requested to apply in
     *          case that starting an ETX OTA Transaction fails once only.
     * 
     * @note    The slave device sometimes does not get the start of an ETX OTA
     *          Transaction after its UART Timeout expires, which is expected
     *          since there is some code in the loop that the slave device has
     *          there that makes it do something else before waiting again for
     *          an ETX OTA Transaction, but that should be evaded by making a
     *          second attempt with the delay established in this variable.
     */
    private final int tryAgainSendingFwiDelay;
    
    /**@brief   Time in microseconds that is to be requested for waiting for
     *          establishing a Bluetooth Connection between our HM-10 BT Dongle
     *          Device and the desired BT Device.
     * 
     * @note    The maximum value for this field parameter should be the
     *          equivalent of 11 seconds, and its suggested minimum time is the
     *          equivalent of 3 seconds.
     */
    private final int hm10ConnectToAddressTimeout;
    
    /**@brief   Designated File Execute Terminal Command towards the executable
     *          compiled file of the PcTool BLE API that must be used whenever
     *          the Operative System at which the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocol}
     *          Class is being run at is Windows.
     */
    private static final String WINDOWS_ETX_OTA_API_PATH = ".\\APIs\\blePcToolAPI\\ETX_OTA_Protocol_BLE_API";
    
    /**@brief   Designated File Execute Terminal Command towards the executable
     *          compiled file of the PcTool BLE API that must be used whenever
     *          the Operative System at which the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocol}
     *          Class is being run at is a Linux based Operative System.
     */
    private static final String LINUX_ETX_OTA_API_PATH = "./APIs/blePcToolAPI/ETX_OTA_Protocol_BLE_API";
    
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocol}
     *          Class whose purpose is only to initialize the values of all its
     *          private variables.
     * 
     * @param pFlashPageSizeInBytes         Flash Page Size in Bytes parameter (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#flashPageSizeInBytes} for more details).
     * @param pEtxBlPageSize                ETX OTA Bootloader Firmware Page Size parameter (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#etxBlPageSize} for more details).
     * @param pEtxAppPageSize               ETX OTA Application Firmware Page Size parameter (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#etxAppPageSize} for more details).
     * @param pComportNumber                Comport at which it is desired to use the BLE Hardware Protocol in which the ETX OTA Communication Protocol will be used at (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#comportNumber} for more details).
     * @param pRs232Baudrate                RS232 Baud rate for the BLE Hardware Protocol in which the ETX OTA Communication Protocol will be used at (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#rs232Baudrate} for more details).
     * @param pSlaveBtAddress               Bluetooth Address of the Slave/External Device with which our host machine is expected to establish a Bluetooth Communication with.
     * @param pRs232ModeDatabits            RS232 Data-bits for the BLE Hardware Protocol in which the ETX OTA Communication Protocol will be used at (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#rs232ModeDatabits} for more details).
     * @param pRs232ModeParity              RS232 Parity for the BLE Hardware Protocol in which the ETX OTA Communication Protocol will be used at (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#rs232ModeParity} for more details).
     * @param pRs232ModeStopbits            RS232 stop-bits for the BLE Hardware Protocol in which the ETX OTA Communication Protocol will be used at (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#rs232ModeStopbits} for more details).
     * @param pRs232IsFlowControl           RS232 Is Enable Flow Control Flag for the BLE Hardware Protocol in which the ETX OTA Communication Protocol will be used at (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#rs232IsFlowControl} for more details).
     * @param pSendPacketBytesDelay         Delay in microseconds to request before sending a byte of data from a certain ETX OTA Packet (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#sendPacketBytesDelay} for more details).
     * @param pTeunizLibPollComportDelay    Delay in microseconds that it is to be requested to apply each time before calling the RS232_PollComport() function of the <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a> (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#teunizLibPollComportDelay} for more details).
     * @param pTryAgainSendingFwiDelay      Delay in microseconds that it is to be requested to apply in case that starting an ETX OTA Transaction fails once only (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#tryAgainSendingFwiDelay} for more details).
     * @param pHm10ConnectToAddressTimeout  Timeout for the HM-10 Connect-to-Address Command. (see {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol#hm10ConnectToAddressTimeout} for more details).
     */
    BleEtxOtaProtocol(short pFlashPageSizeInBytes,
                short pEtxBlPageSize,
                short pEtxAppPageSize,
                short pComportNumber,
                int pRs232Baudrate,
                String pSlaveBtAddress,
                char pRs232ModeDatabits,
                char pRs232ModeParity,
                char pRs232ModeStopbits,
                short pRs232IsFlowControl,
                int pSendPacketBytesDelay,
                int pTeunizLibPollComportDelay,
                int pTryAgainSendingFwiDelay,
                int pHm10ConnectToAddressTimeout) {
        /* Set the received values in the corresponding private variables for this class. */
        flashPageSizeInBytes = pFlashPageSizeInBytes;
        etxBlPageSize = pEtxBlPageSize;
        etxAppPageSize = pEtxAppPageSize;
        comportNumber = pComportNumber;
        rs232Baudrate = pRs232Baudrate;
        slaveBtAddress = pSlaveBtAddress;
        rs232ModeDatabits = pRs232ModeDatabits;
        rs232ModeParity = pRs232ModeParity;
        rs232ModeStopbits = pRs232ModeStopbits;
        rs232IsFlowControl = pRs232IsFlowControl;
        sendPacketBytesDelay = pSendPacketBytesDelay;
        teunizLibPollComportDelay = pTeunizLibPollComportDelay;
        tryAgainSendingFwiDelay = pTryAgainSendingFwiDelay;
        hm10ConnectToAddressTimeout = pHm10ConnectToAddressTimeout;
    }
    
    /**
     * {@inheritDoc}
     * @details This method interacts with the PcTool BLE API for sending the
     *          required information so that the mentioned API gets to make all
     *          the Hardware Protocol Configurations, Data Processing and Data
     *          Handling instead of having it done on this Java Project side.
     */
    @Override
    public com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus startEtxOtaTransaction(
            String payloadPathOrData,
            short etxOtaPayloadType,
            int timeout) {
        /** <b>Local {@link com.mortrack.hostbleapp.utils.TerminalProcess} Type variable terminalProcess:</b> Used to hold a Terminal Process Object, which will be later used for communicating with the PcTool BLE API made by Mortrack and to request to it to send a desired Payload under the requested Configuration Settings. */
        TerminalProcess terminalProcess = new TerminalProcess();
        /** <b>Local String Type variable ret:</b> Used to hold the String value of the interpreted integral number of whatever exception code value is returned by the PcTool BLE API made by Mortrack. Note that the returned values will stand for the {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus} Exception Codes but in their respective interpreted integral number, which is to later be converted to its corresponding integer value via the {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol.etxOtaStatusHandler} function. */
        String ret = "";
        /** <b>Local String Type variable command:</b> Used to populate it with the Terminal Command required for executing the PcTool BLE API made by Mortrack and to give to it the desired Configuration Parameters and also to request to it to send a desired Payload under those Configuration Settings. */
        String command;
        if (terminalProcess.isWindows()) {
            command = WINDOWS_ETX_OTA_API_PATH + " "
                    + comportNumber + " "
                    + payloadPathOrData.length() + " "
                    + "\"" + payloadPathOrData + "\"" + " "
                    + etxOtaPayloadType + " "
                    + flashPageSizeInBytes + " "
                    + etxBlPageSize + " "
                    + etxAppPageSize + " "
                    + rs232Baudrate + " "
                    + rs232ModeDatabits + " "
                    + rs232ModeParity + " "
                    + rs232ModeStopbits + " "
                    + rs232IsFlowControl + " "
                    + sendPacketBytesDelay + " "
                    + teunizLibPollComportDelay + " "
                    + tryAgainSendingFwiDelay + " "
                    // TODO: Pending to add the following field param into the code's logic
                    + hm10ConnectToAddressTimeout + " "
                    + slaveBtAddress;
        } else {
            command = LINUX_ETX_OTA_API_PATH + " "
                    + comportNumber + " "
                    + payloadPathOrData.length() + " "
                    + payloadPathOrData + " "
                    + etxOtaPayloadType + " "
                    + flashPageSizeInBytes + " "
                    + etxBlPageSize + " "
                    + etxAppPageSize + " "
                    + rs232Baudrate + " "
                    + rs232ModeDatabits + " "
                    + rs232ModeParity + " "
                    + rs232ModeStopbits + " "
                    + rs232IsFlowControl + " "
                    + sendPacketBytesDelay + " "
                    + teunizLibPollComportDelay + " "
                    + tryAgainSendingFwiDelay + " "
                    // TODO: Pending to add the following field param into the code's logic
                    + hm10ConnectToAddressTimeout + " "
                    + slaveBtAddress;
        }
        try {
            ret = terminalProcess.runExecutableFile(command, timeout);
        } catch (Exception ex) {
            Logger.getLogger(BleEtxOtaProtocol.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        lastEtxOtaStatus = etxOtaStatusHandler(ret);
        return lastEtxOtaStatus;
    }
    
    @Override
    public com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus lastEtxOtaStatus() {
        return lastEtxOtaStatus;
    }
    
    /**@brief   Parses and gets the equivalent of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus}
     *          Type with respect to a given String value with an interpreted
     *          integral number.
     * 
     * @param response  Should hold the String value that is to be interpreted
     *                  as an integral number to be able to get its
     *                  corresponding
     *                  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus}
     *                  Type.
     * 
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OK if \p response param = "0".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_STOP if \p response param = "1".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_NR if \p response param = "2".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_NA if \p response param = "3".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_ERR if \p response param = "4" or if 0 \f$<\f$ \p response param \f$>\f$ 17.
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_INV_CMD_LINE_ARG if \p response param = "5".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_UNRECOG_PAYLOAD if \p response param = "6".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OPEN_COM_ERR if \p response param = "7".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OPEN_FILE_ERR if \p response param = "8".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_READ_FILE_ERR if \p response param = "9".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_START_CMD_SEND_DATA_ERR if \p response param = "10".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_START_CMD_NACK_RESP if \p response param = "11".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_HEADER_PCKT_SEND_DATA_ERR if \p response param = "12".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_HEADER_PCKT_NACK_RESP if \p response param = "13".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_DATA_PCKT_SEND_DATA_ERR if \p response param = "14".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_DATA_PCKT_NACK_RESP if \p response param = "15".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_END_CMD_SEND_DATA_ERR if \p response param = "16".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_END_CMD_NACK_RESP if \p response param = "17".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_ABORT_CMD_SEND_DATA_ERR if \p response param = "18".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_ABORT_CMD_NACK_RESP if \p response param = "19".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_ABORT_LOOP_ERR if \p response param = "20".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_BLE_INIT_ERR if \p response param = "21".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_BLE_AT_CMD_ERR if \p response param = "22".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_BLE_TYPE_CMD_ERR if \p response param = "23".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_BLE_RESET_CMD_ERR if \p response param = "24".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_BLE_CONN_CMD_ERR if \p response param = "25".
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 28, 2023.
     */
    private com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus etxOtaStatusHandler(String response) {
        switch (response) {
            case "0":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OK;
            case "1":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_STOP;
            //case "":
            case "2":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_NR;
            case "3":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_NA;
            case "4":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_ERR;
            case "5":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_INV_CMD_LINE_ARG;
            case "6":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_UNRECOG_PAYLOAD;
            case "7":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OPEN_COM_ERR;
            case "8":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OPEN_FILE_ERR;
            case "9":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_READ_FILE_ERR;
            case "10":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_START_CMD_SEND_DATA_ERR;
            case "11":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_START_CMD_NACK_RESP;
            case "12":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_HEADER_PCKT_SEND_DATA_ERR;
            case "13":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_HEADER_PCKT_NACK_RESP;
            case "14":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_DATA_PCKT_SEND_DATA_ERR;
            case "15":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_DATA_PCKT_NACK_RESP;
            case "16":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_END_CMD_SEND_DATA_ERR;
            case "17":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_END_CMD_NACK_RESP;
            case "18":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_ABORT_CMD_SEND_DATA_ERR;
            case "19":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_ABORT_CMD_NACK_RESP;
            case "20":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_ABORT_LOOP_ERR;
            case "21":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_BLE_INIT_ERR;
            case "22":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_BLE_AT_CMD_ERR;
            case "23":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_BLE_TYPE_CMD_ERR;
            case "24":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_BLE_RESET_CMD_ERR;
            case "25":
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_BLE_CONN_CMD_ERR;    
            default:
                return EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_ERR;
        }
    }
}

/** @} */

/** @} */
