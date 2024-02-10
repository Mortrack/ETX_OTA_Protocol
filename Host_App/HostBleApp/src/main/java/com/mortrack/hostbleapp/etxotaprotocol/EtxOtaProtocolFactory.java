/**@file
 * 
 * @defgroup ETX_OTA_Protocol_Factory ETX OTA Protocol Factory module
 * @{
 * 
 * @brief   This module contains the Classes, variables and methods required to
 *          enable the application to initialize and use the ETX OTA
 *          Communication Protocol over a desired and available Hardware
 *          Protocol so that our host machine can communicate with a desired
 *          slave device.
 * 
 * @details Since this module uses the Factory Design Pattern, to use it, it is
 *          required to create an object of one of the extends of the
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory}
 *          Factory Class according to the desired Hardware Protocol in which
 *          the ETX OTA Protocol wants to be used on. The available extends
 *          classes to choose from are
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.UartEtxOtaProtocolFactory}
 *          and
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocolFactory}.
 *          Subsequently, you will have to run the
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#sendPayload}
 *          method from the
 *          previously created object in order to create an instance initialized
 *          with the desired Hardware Protocol in which you want to use the ETX
 *          OTA Communication Protocol and, also, to send the desired Payload
 *          under the Configuration Settings with which both of the previous
 *          objects were created with. After that, you may now call whatever
 *          additional method you require from the
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocol}
 *          Interface in order to either use or get more information from the
 *          ETX OTA Communication Protocol in whatever Hardware Protocol you
 *          have had already configured by this point to send and/or receive
 *          data between your host machine and whatever slave device you
 *          established a connection with.
 * @details For details on some friendly code example of how to use the 
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory}
 *          Factory Class, see the following:
 *          <ul>
 *              <li>@ref UART_ETX_OTA_Protocol </li>
 *              <li>@ref BLE_ETX_OTA_Protocol </li>
 *          </ul>
 * 
 * @see     <a href=https://www.youtube.com/watch?v=EdFq_JIThqM>Learn more about the Factory Design Pattern here</a>
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 30, 2023.
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

/**@brief   Factory Class that enables the application to initialize the ETX OTA
 *          Communication Protocol over a desired and available Hardware
 *          Protocol so that our host machine can send and/or receive with a
 *          desired slave device.
 * 
 * @details This Factory Class provides the required Constructor Classes so that
 *          together with the
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#sendPayload}
 *          method, the implementer can create and get the corresponding
 *          Concrete Product (i.e., to get the corresponding
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocol}
 *          Object) with respect to the Concrete Creator with which that method
 *          is called from and to also send a desired Payload to a requested
 *          slave device.
 * 
 * @see     <a href=https://www.youtube.com/watch?v=EdFq_JIThqM>Learn more about the Factory Design Pattern here</a>
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 30, 2023.
 */
public abstract class EtxOtaProtocolFactory {
    /**@brief   Flash Memory page size in bytes as defined by the MCU/MPU with
     *          which the Serial Port communication is to be established with.
     */
    private short flashPageSizeInBytes = 1024;
    
    /**@brief   Number of Flash Memory pages that have been designated for the
     *          Bootloader Firmware of the MCU/MPU with which the Serial Port
     *          communication is to be established with.
     */
    private short etxBlPageSize = 34;
    
    /**@brief   Number of Flash Memory pages that have been designated for the
     *          Application Firmware of the MCU/MPU with which the Serial Port
     *          communication is to be established with.
     */
    private short etxAppPageSize = 86;
    
    /**@brief   The actual comport that wants to be used for the RS232 protocol
     *          to connect to a desired external device.
     */
    private short comportNumber = 0;
    
    /**@brief   The actual Bluetooth Address of the Slave/External Device with
     *          which it is desired to establish a Bluetooth Connection with.
     */
    private String slaveBtAddress = "";
    
    /**@brief   Chosen Baud-rate with which we want the host to run the RS232
     *          protocol.
     */
    private int rs232Baudrate = 115200;
    
    /**@brief   Chosen Data-bits string with which we want the host to run the
     *          RS232 protocol.
     * 
     * @details The valid data-bits in the
     *          <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a>
     *          that are used in the PcTool API are '5', '6', '7' and '8'.
     */
    private char rs232ModeDatabits = '8';
    
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
    private char rs232ModeParity = 'N';
    
    /**@brief   Chosen Stop-bit string with which we want the host to run the
     *          RS232 protocol.
     * 
     * @note    The possible stop-bit values in RS232 are '1' or '2'.
     */
    private char rs232ModeStopbits = '1';
    
    /**@brief   Chosen Flow Control decimal value to indicate with a 1 that we
     *          want the host to run the RS232 protocol with Flow Control
     *          enabled, or otherwise with a decimal value of 0 to indicate to
     *          the host not to run the RS232 protocol with Flow Control.
     */
    private short rs232IsFlowControl = 0;
    
    /**@brief   Delay in microseconds that is desired to request before having
     *          send a byte of data from a certain ETX OTA Packet that is in
     *          process of being send to the MCU.
     */
    private int sendPacketBytesDelay = 1000;
    
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
    private int teunizLibPollComportDelay = 500000;
    
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
    private int tryAgainSendingFwiDelay = 9000000;
    
    /**@brief   Time in microseconds that is to be requested for waiting for
     *          establishing a Bluetooth Connection between our HM-10 BT Dongle
     *          Device and the desired BT Device.
     * 
     * @note    The maximum value for this field parameter should be the
     *          equivalent of 11 seconds, and its suggested minimum time is the
     *          equivalent of 3 seconds.
     */
    private int hm10ConnectToAddressTimeout = 11000000;
    
    /**@brief	ETX OTA Exception code definitions.
     *
     * @details	These Exception Codes are returned by some of the methods of the
     *          Classes that use the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus}
     *          Interface with the purpose of indicating resulting status of
     *          having executed their respective processes. For example, to
     *          indicate that the process executed by a certain method was
     *          successful or that it has failed.
     */
    public enum EtxOtaStatus {
        ETX_OTA_EC_OK(0),                           //!< ETX OTA Protocol was successful.
        ETX_OTA_EC_STOP(1),                         //!< ETX OTA Protocol has been stopped.
        ETX_OTA_EC_NR(2),                           //!< ETX OTA Protocol has concluded with no response from Slave Device.
        ETX_OTA_EC_NA(3),                           //!< ETX OTA Payload sent or to be sent Not Applicable.
        ETX_OTA_EC_ERR(4),                          //!< ETX OTA Protocol has failed.
        ETX_OTA_EC_INV_CMD_LINE_ARG(5),             //!< ETX OTA Protocol API side received invalid number of Command Line Arguments.
        ETX_OTA_EC_UNRECOG_PAYLOAD(6),              //!< ETX OTA Payload Type not recognized.
        ETX_OTA_EC_OPEN_COM_ERR(7),                 //!< ETX OTA Protocol has failed to Open requested RS232 Comport.
        ETX_OTA_EC_OPEN_FILE_ERR(8),                //!< ETX OTA Protocol has failed to Open requested the File where the Payload lies in.
        ETX_OTA_EC_READ_FILE_ERR(9),                //!< ETX OTA Protocol has failed to Read the requested File where the Payload lies in.
        ETX_OTA_EC_START_CMD_SEND_DATA_ERR(10),     //!< ETX OTA Protocol has failed to send ETX OTA Start Command's bytes to the slave device.
        ETX_OTA_EC_START_CMD_NACK_RESP(11),         //!< ETX OTA Protocol has received a NACK Response after sending an ETX OTA Start Command to the slave device.
        ETX_OTA_EC_HEADER_PCKT_SEND_DATA_ERR(12),   //!< ETX OTA Protocol has failed to send ETX OTA Header Type Packet's bytes to the slave device.
        ETX_OTA_EC_HEADER_PCKT_NACK_RESP(13),       //!< ETX OTA Protocol has received a NACK Response after sending an ETX OTA Header Type Packet to the slave device.
        ETX_OTA_EC_DATA_PCKT_SEND_DATA_ERR(14),     //!< ETX OTA Protocol has failed to send ETX OTA Data Type Packet's bytes to the slave device.
        ETX_OTA_EC_DATA_PCKT_NACK_RESP(15),         //!< ETX OTA Protocol has received a NACK Response after sending an ETX OTA Data Type Packet to the slave device.
        ETX_OTA_EC_END_CMD_SEND_DATA_ERR(16),       //!< ETX OTA Protocol has failed to send ETX OTA End Command's bytes to the slave device.
        ETX_OTA_EC_END_CMD_NACK_RESP(17),           //!< ETX OTA Protocol has received a NACK Response after sending an ETX OTA End Command to the slave device.
        ETX_OTA_EC_ABORT_CMD_SEND_DATA_ERR(18),     //!< ETX OTA Protocol has failed to send ETX OTA Abort Command's bytes to the slave device.
        ETX_OTA_EC_ABORT_CMD_NACK_RESP(19),         //!< ETX OTA Protocol has received a NACK Response after sending an ETX OTA Abort Command to the slave device.
        ETX_OTA_EC_ABORT_LOOP_ERR(20),              //!< ETX OTA Protocol failed to successfully send an Abort Command during the loop-process of sending Abort Commands in an attempt to guarantee getting the slave device to leave any on-going ETX OTA Transaction before starting a new one.
        ETX_OTA_EC_BLE_INIT_ERR(21),                //!< ETX OTA Protocol Bluetooth Initialization has failed due to invalid comport value.
        ETX_OTA_EC_BLE_AT_CMD_ERR(22),              //!< ETX OTA Protocol Bluetooth Disconnect Command has failed.
        ETX_OTA_EC_BLE_TYPE_CMD_ERR(23),            //!< ETX OTA Protocol Bluetooth Set Pin Code Mode Command has failed.
        ETX_OTA_EC_BLE_RESET_CMD_ERR(24),           //!< ETX OTA Protocol Bluetooth Reset Command has failed.
        ETX_OTA_EC_BLE_CONN_CMD_ERR(25);            //!< ETX OTA Protocol Bluetooth Connect-To-Address Command has failed.
        
        
        /**@brief   Holds the Integer Number of the designated numeric value to
         *          the selected
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus}
         *          Enum value.
         */
        private final int EtxOtaEcNumber;
        
        /**@brief   Constructor of the
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus}
         *          Enum, which persists the designated integer number of the
         *          selected Enum value into
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus#EtxOtaEcNumber}.
         * 
         * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
         * @date    November 08, 2023.
         */
        EtxOtaStatus(int EtxOtaEcNumber) {
            this.EtxOtaEcNumber = EtxOtaEcNumber;
        }
        
        /**@brief   Gets the equivalent ETX OTA Status Exception Code but in its
         *          designated integer number value.
         * 
         * @return  The
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus}
         *          Enum but in its designated integer number value.
         * 
         * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
         * @date    November 08, 2023.
         */
        public int getNumber() {
            return EtxOtaEcNumber;
        }
    }
    
    /**@brief	Dongle Configurator Exception codes.
     *
     * @details	These Exception Codes are returned by the Dongle Configurator
     *          API made by Mortrack and have the the purpose of indicating
     *          resulting status of having executed that API. For example, to
     *          indicate that the API completed its processing successfully or
     *          that it has failed.
     */
    public enum DongleConfiguratorStatus {
        DONGLE_CONFIGURATOR_EC_OK(0),                   //!< Dongle Configurator Process was successful.
        DONGLE_CONFIGURATOR_EC_STOP(1),                 //!< Dongle Configurator Process has been stopped.
        DONGLE_CONFIGURATOR_EC_NR(2),                   //!< Dongle Configurator Process has concluded with no response from Host.
        DONGLE_CONFIGURATOR_EC_NA(3),                   //!< Dongle Configurator Process Not Applicable.
        DONGLE_CONFIGURATOR_EC_ERR(4),                  //!< Dongle Configurator Process has failed.
        DONGLE_CONFIGURATOR_EC_INV_CMD_LINE_ARG(5),     //!< Dongle Configurator Process API side received invalid number of Command Line Arguments.
        // NOTE: Do not use Enum value 6 so that it can be compatible with the EtxOtaStatus Enum of the ETX OTA Protocol.
        DONGLE_CONFIGURATOR_EC_OPEN_COM_ERR(7),         //!< Dongle Configurator Process has failed to Open requested RS232 Comport.
        // NOTE: Do not use Enum values 8 up to 20 so that it can be compatible with the EtxOtaStatus Enum of the ETX OTA Protocol.
        DONGLE_CONFIGURATOR_EC_BLE_INIT_ERR(21),        //!< Dongle Configurator Process Bluetooth Initialization has failed due to invalid comport value.
        DONGLE_CONFIGURATOR_EC_BLE_AT_CMD_ERR(22),      //!< Dongle Configurator Process Bluetooth Disconnect Command has failed.
        DONGLE_CONFIGURATOR_EC_BLE_TYPE_CMD_ERR(23),    //!< Dongle Configurator Process Bluetooth Set Pin Code Mode Command has failed.
        DONGLE_CONFIGURATOR_EC_BLE_RESET_CMD_ERR(24),   //!< Dongle Configurator Process Bluetooth Reset Command has failed.
        DONGLE_CONFIGURATOR_EC_BLE_CONN_CMD_ERR(25),    //!< Dongle Configurator Process Bluetooth Connect-To-Address Command has failed.
        DONGLE_CONFIGURATOR_EC_BLE_RENEW_CMD_ERR(26),   //!< Dongle Configurator Process Bluetooth Renew Command has failed.
        DONGLE_CONFIGURATOR_EC_BLE_ROLE_CMD_ERR(27),    //!< Dongle Configurator Process Bluetooth Role Command has failed.
        DONGLE_CONFIGURATOR_EC_BLE_IMME_CMD_ERR(28),    //!< Dongle Configurator Process Bluetooth IMME Command has failed.
        DONGLE_CONFIGURATOR_EC_BLE_NOTI_CMD_ERR(29),    //!< Dongle Configurator Process Bluetooth NOTI Command has failed.
        DONGLE_CONFIGURATOR_EC_BLE_MODE_CMD_ERR(30);    //!< Dongle Configurator Process Bluetooth MODE Command has failed.
        
        /**@brief   Holds the Integer Number of the designated numeric value to
         *          the selected
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus}
         *          Enum value.
         */
        private final int DongleConfiguratorEcNumber;
        
        /**@brief   Constructor of the
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus}
         *          Enum, which persists the designated integer number of the
         *          selected Enum value into
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus#DongleConfiguratorEcNumber}.
         * 
         * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
         * @date    December 30, 2023.
         */
        DongleConfiguratorStatus(int DongleConfiguratorEcNumber) {
            this.DongleConfiguratorEcNumber = DongleConfiguratorEcNumber;
        }
        
        /**@brief   Gets the equivalent Dongle Configurator Exception Code but
         *          in its designated integer number value.
         * 
         * @return  The
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus}
         *          Enum but in its designated integer number value.
         * 
         * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
         * @date    December 30, 2023.
         */
        public int getNumber() {
            return DongleConfiguratorEcNumber;
        }
    }
    
    /**@brief	Payload Type definitions available in the ETX OTA Protocol.
     *
     * @details Whenever the host sends to the slave device (via the
     *          corresponding Hardware Protocol) an ETX OTA Data Packet Type
     *          (i.e., a Data Type Packet), there are several possible Payload
     *          Types for this Data Type Packet that indicate the type of data
     *          that the slave device should expect to receive from the whole
     *          Data Type Packets that it will receive (this is because a whole
     *          Payload can be received from one or more Data Type Packets).
     *          These Payload Types are the following:
     */
    public enum EtxOtaPayloadType {
        ETX_OTA_Application_Firmware_Image(0),  //!< ETX OTA Application Firmware Image Data Packet Type.
        ETX_OTA_Bootloader_Firmware_Image(1),   //!< ETX OTA Bootloader Firmware Image Data Packet Type.
        ETX_OTA_Custom_Data(2);                 //!< ETX OTA Custom Data Packet Type.
        
        /**@brief   Holds the Integer Number of the designated numeric value to
         *          the selected
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaPayloadType}
         *          Enum value.
         */
        private final int EtxOtaPayloadNumber;
        
        /**@brief   Constructor of the
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaPayloadType}
         *          Enum, which persists the designated integer number of the
         *          selected Enum value into
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaPayloadType#EtxOtaPayloadNumber}.
         * 
         * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
         * @date    November 08, 2023.
         */
        EtxOtaPayloadType(int EtxOtaPayloadNumber) {
            this.EtxOtaPayloadNumber = EtxOtaPayloadNumber;
        }
        
        /**@brief   Gets the equivalent ETX OTA Payload Type but in its
         *          designated integer number value.
         * 
         * @return  The
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaPayloadType}
         *          Enum but in its designated integer number value.
         * 
         * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
         * @date    November 08, 2023.
         */
        public int getNumber() {
            return EtxOtaPayloadNumber;
        }
    }
    
    /**@brief	ETX OTA Hardware Protocol definitions.
     *
     * @details	These definitions define the available ETX OTA <u>Hardware
     *          Protocols</u> that the slave device can use to establish a
     *          "physical" connection with our host machine, in which both ends
     *          will communicate with each other by using the ETX OTA
     *          <u>Software Protocol</u> for interpreting the data send/received
     *          on both sides.
     */
    public enum EtxOtaHardwareType {
        ETX_OTA_hw_Protocol_UART(0),  //!< UART Hardware Protocol to be used for the ETX OTA Protocol. @details The default expected UART configuration settings are 115'200 baud rate, 8 data-bits, no parity, 1 stop-bit and no Flow Control. @note These can be changed as long as these configurations are also changed on the host app, where it is indispensable that they match.
        ETX_OTA_hw_Protocol_BLE(1);   //!< BLE Hardware Protocol to be used for the ETX OTA Protocol.
        
        /**@brief   Holds the Integer Number of the designated numeric value to
         *          the selected
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaHardwareType}
         *          Enum value.
         */
        private final int EtxOtaHardwaredNumber;
        
        /**@brief   Constructor of the
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaHardwareType}
         *          Enum, which persists the designated integer number of the
         *          selected Enum value into
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaHardwareType#EtxOtaHardwaredNumber}.
         * 
         * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
         * @date    November 08, 2023.
         */
        EtxOtaHardwareType(int EtxOtaHardwaredNumber) {
            this.EtxOtaHardwaredNumber = EtxOtaHardwaredNumber;
        }
        
        /**@brief   Gets the equivalent ETX OTA Hardware Type but in its
         *          designated integer number value.
         * 
         * @return  The
         *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaHardwareType}
         *          Enum but in its designated integer number value.
         * 
         * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
         * @date    November 08, 2023.
         */
        public int getNumber() {
            return EtxOtaHardwaredNumber;
        }
    }
    
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory}
     *          Class, which persists a custom value for the Comport Number.
     * 
     * @param   pComportNumber  See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#comportNumber} for more details.
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    EtxOtaProtocolFactory(short pComportNumber) {
        comportNumber = pComportNumber;
    }
    
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory}
     *          Class, which persists a custom value for the:
     *          <ul>
     *              <li>Comport Number</li>
     *              <li>RS232 Baud rate</li>
     *              <li>Bluetooth Address of the Slave/External Device</li>
     *              <li>HM-10 Connect To Address Timeout</li>
     *          </ul>
     * 
     * @param   pComportNumber                  See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#comportNumber} for more details.
     * @param   pRs232Baudrate                  See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232Baudrate} for more details.
     * @param   pSlaveBtAddress                 See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#pSlaveBtAddress} for more details.
     * @param   pHm10ConnectToAddressTimeout    See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#pHm10ConnectToAddressTimeout} for more details.
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 28, 2023.
     */
    EtxOtaProtocolFactory(short pComportNumber,
            int pRs232Baudrate,
            String pSlaveBtAddress,
            int pHm10ConnectToAddressTimeout) {
        /* Set the received values in the corresponding private variables for this class. */
        comportNumber = pComportNumber;
        rs232Baudrate = pRs232Baudrate;
        slaveBtAddress = pSlaveBtAddress;
        hm10ConnectToAddressTimeout = pHm10ConnectToAddressTimeout;
    }
    
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory}
     *          Class, which persists a custom value for the:
     *          <ul>
     *              <li>Comport Number</li>
     *              <li>RS232 Baud rate</li>
     *              <li>Bluetooth Address of the Slave/External Device.</li>
     *              <li>RS232 Data-bits</li>
     *              <li>RS232 Parity</li>
     *              <li>RS232 Stop-bits</li>
     *              <li>RS232 Flow Control Flag</li>
     *              <li>HM-10 Connect To Address Timeout</li>
     *          </ul>
     * 
     * @param   pComportNumber                  See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#comportNumber} for more details.
     * @param   pRs232Baudrate                  See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232Baudrate} for more details.
     * @param   pSlaveBtAddress                 See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#pSlaveBtAddress} for more details.
     * @param   pRs232ModeDatabits              See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeDatabits} for more details.
     * @param   pRs232ModeParity                See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeParity} for more details.
     * @param   pRs232ModeStopbits              See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeStopbits} for more details.
     * @param   pRs232IsFlowControl             See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232IsFlowControl} for more details.
     * @param   pHm10ConnectToAddressTimeout    See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#pHm10ConnectToAddressTimeout} for more details.
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 28, 2023.
     */
    EtxOtaProtocolFactory(short pComportNumber,
            int pRs232Baudrate,
            String pSlaveBtAddress,
            char pRs232ModeDatabits,
            char pRs232ModeParity,
            char pRs232ModeStopbits,
            short pRs232IsFlowControl,
            int pHm10ConnectToAddressTimeout) {
        /* Set the received values in the corresponding private variables for this class. */
        comportNumber = pComportNumber;
        rs232Baudrate = pRs232Baudrate;
        slaveBtAddress = pSlaveBtAddress;
        rs232ModeDatabits = pRs232ModeDatabits;
        rs232ModeParity = pRs232ModeParity;
        rs232ModeStopbits = pRs232ModeStopbits;
        rs232IsFlowControl = pRs232IsFlowControl;
        hm10ConnectToAddressTimeout = pHm10ConnectToAddressTimeout;
    }
    
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory}
     *          Class, which persists a custom value for the:
     *          <ul>
     *              <li>Comport Number</li>
     *              <li>RS232 Baud rate</li>
     *              <li>Bluetooth Address of the Slave/External Device</li>
     *              <li>RS232 Data-bits</li>
     *              <li>RS232 Parity</li>
     *              <li>RS232 Stop-bits</li>
     *              <li>RS232 Flow Control Flag</li>
     *              <li>Delay in microseconds before sending ETX OTA bytes</li>
     *              <li>Delay in microseconds before calling the RS232_PollComport() function that is used in the PcTool API</li>
     *              <li>Delay in microseconds to be applied in case that starting an ETX OTA Transaction fails once only</li>
     *              <li>HM-10 Connect To Address Timeout</li>
     *          </ul>
     * 
     * @param   pComportNumber                  See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#comportNumber} for more details.
     * @param   pRs232Baudrate                  See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232Baudrate} for more details.
     * @param   pSlaveBtAddress                 See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#pSlaveBtAddress} for more details.
     * @param   pRs232ModeDatabits              See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeDatabits} for more details.
     * @param   pRs232ModeParity                See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeParity} for more details.
     * @param   pRs232ModeStopbits              See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeStopbits} for more details.
     * @param   pRs232IsFlowControl             See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232IsFlowControl} for more details.
     * @param   pSendPacketBytesDelay           See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#sendPacketBytesDelay} for more details.
     * @param   pTeunizLibPollComportDelay      See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#teunizLibPollComportDelay} for more details.
     * @param   pTryAgainSendingFwiDelay        See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#tryAgainSendingFwiDelay} for more details.
     * @param   pHm10ConnectToAddressTimeout    See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#pHm10ConnectToAddressTimeout} for more details.
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 28, 2023.
     */
    EtxOtaProtocolFactory(short pComportNumber,
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
    
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory}
     *          Class, which persists a custom value for the:
     *          <ul>
     *              <li>Flash Memory page size in bytes of the slave device</li>
     *              <li>Bootloader Firmware size in Flash Memory Pages designated on the slave device</li>
     *              <li>Application Firmware size in Flash Memory Pages designated on the slave device</li>
     *              <li>Comport Number</li>
     *              <li>RS232 Baud rate</li>
     *              <li>Bluetooth Address of the Slave/External Device.</li>
     *              <li>RS232 Data-bits</li>
     *              <li>RS232 Parity</li>
     *              <li>RS232 Stop-bits</li>
     *              <li>RS232 Flow Control Flag</li>
     *              <li>Delay in microseconds before sending ETX OTA bytes</li>
     *              <li>Delay in microseconds before calling the RS232_PollComport() function that is used in the PcTool API</li>
     *              <li>Delay in microseconds to be applied in case that starting an ETX OTA Transaction fails once only</li>
     *              <li>HM-10 Connect To Address Timeout</li>
     *          </ul>
     * 
     * @param   pFlashPageSizeInBytes           See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#flashPageSizeInBytes} for more details.
     * @param   pEtxBlPageSize                  See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#etxBlPageSize} for more details.
     * @param   pEtxAppPageSize                 See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#etxAppPageSize} for more details.
     * @param   pComportNumber                  See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#comportNumber} for more details.
     * @param   pRs232Baudrate                  See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232Baudrate} for more details.
     * @param   pSlaveBtAddress                 See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#pSlaveBtAddress} for more details.
     * @param   pRs232ModeDatabits              See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeDatabits} for more details.
     * @param   pRs232ModeParity                See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeParity} for more details.
     * @param   pRs232ModeStopbits              See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeStopbits} for more details.
     * @param   pRs232IsFlowControl             See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232IsFlowControl} for more details.
     * @param   pSendPacketBytesDelay           See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#sendPacketBytesDelay} for more details.
     * @param   pTeunizLibPollComportDelay      See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#teunizLibPollComportDelay} for more details.
     * @param   pTryAgainSendingFwiDelay        See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#tryAgainSendingFwiDelay} for more details.
     * @param   pHm10ConnectToAddressTimeout    See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#pHm10ConnectToAddressTimeout} for more details.
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 28, 2023.
     */
    EtxOtaProtocolFactory(short pFlashPageSizeInBytes,
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
    
    /**@brief   Creates and gets the corresponding Concrete Product via the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.createProtocolInstance}
     *          method and also sends the desired Payload to the slave device.
     * 
     * @details In other words, it will create and get the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocol}
     *          Type Object that matches/corresponds with the Concrete Creator
     *          of either the UART or BLE Hardware Protocol with which this
     *          method is called with. Then, via the Hardware Protocol that
     *          corresponds to the Concrete Creator used, this method will send
     *          the desired Payload to the requested slave device under the
     *          chosen configuration settings.
     * 
     * @param payloadPathOrData This parameter will stand for the Payload Path
     *                          whenever a Firmware Image is given via the
     *                          \p etxOtaPayloadType param. However, this field
     *                          will correspond to the actual data for whenever
     *                          an ETX OTA Custom Data Payload is given instead
     *                          in that \p etxOtaPayloadType param.
     * @param etxOtaPayloadType ETX OTA Payload Type.
     * 
     * @note                    For more details on the available Type values
     *                          for this field, see
     *                          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#EtxOtaPayloadType} ).
     *                          
     * @param timeout           Timeout in seconds that is desired for the whole
     *                          interacting between our host machine and PcTool
     *                          API to last at the most for a single ETX OTA
     *                          Complete Transaction (i.e., for completely
     *                          sending a Payload to the desired slave device).
     * 
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.UartEtxOtaProtocol
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocol
     * 
     * @note    The returned Object Instance will be given with the chosen
     *          Configuration Settings via both the used Concrete Creator and
     *          this method.
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 16, 2023.
     */
    public com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocol sendPayload(String payloadPathOrData, short etxOtaPayloadType, int timeout) {
        EtxOtaProtocol etxOtaProtocol = createProtocolInstance();
        etxOtaProtocol.startEtxOtaTransaction(payloadPathOrData, etxOtaPayloadType, timeout);
        return etxOtaProtocol;
    }
    
    /**@brief   Gets the Flash Memory Page size of the slave device in bytes.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#flashPageSizeInBytes}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public short flashPageSizeInBytes() {
        return flashPageSizeInBytes;
    }
    
    /**@brief   Gets the number of Flash Memory pages designated for the
     *          Bootloader Firmware of the slave device.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#etxBlPageSize}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public short etxBlPageSize() {
        return etxBlPageSize;
    }
    
    /**@brief   Gets the number of Flash Memory pages designated for the
     *          Application Firmware of the slave device.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#etxAppPageSize}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public short etxAppPageSize() {
        return etxAppPageSize;
    }
    
    /**@brief   Gets the RS-232 Comport Number with which our host machine is to
     *          establish UART Communication with the slave device.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#comportNumber}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public short comportNumber() {
        return comportNumber;
    }
    
    /**@brief   Gets the RS-232 Baud-rate with which our host machine is to
     *          establish UART Communication with the slave device.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232Baudrate}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public int rs232Baudrate() {
        return rs232Baudrate;
    }
    
    /**@brief   Gets the Bluetooth Address of the Slave/External Device with
     *          which our host machine is to establish a Bluetooth Communication
     *          with.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#slaveBtAddress}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 16, 2023.
     */
    public String slaveBtAddress() {
        return slaveBtAddress;
    }
    
    /**@brief   Gets the RS-232 Data-bits with which our host machine is to
     *          establish UART Communication with the slave device.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeDatabits}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public char rs232ModeDatabits() {
        return rs232ModeDatabits;
    }
    
    /**@brief   Gets the RS-232 Parity with which our host machine is to
     *          establish UART Communication with the slave device.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeParity}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public char rs232ModeParity() {
        return rs232ModeParity;
    }
    
    /**@brief   Gets the RS-232 Stop-bit with which our host machine is to
     *          establish UART Communication with the slave device.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232ModeStopbits}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public char rs232ModeStopbits() {
        return rs232ModeStopbits;
    }
    
    /**@brief   Indicates whether the RS-232 Flow Control in the UART
     *          Communication between our host machine and the slave device is
     *          enabled with a \c 1 or disabled with a \c 0 .
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#rs232IsFlowControl}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public short isRs232FlowControl() {
        return rs232IsFlowControl;
    }
    
    /**@brief   Gets the delay in microseconds that will be applied before
     *          having sent a byte of data from a certain ETX OTA Packet.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#sendPacketBytesDelay}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public int sendPacketBytesDelay() {
        return sendPacketBytesDelay;
    }
    
    /**@brief   Gets the delay in microseconds that will be applied before
     *          calling the RS232_PollComport() function of the
     *          <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a>
     *          that is used in the PcTool API.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#sendPacketBytesDelay}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public int teunizLibPollComportDelay() {
        return teunizLibPollComportDelay;
    }
    
    /**@brief   Gets the delay in microseconds that will be applied in case that
     *          starting an ETX OTA Transaction fails once only.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#tryAgainSendingFwiDelay}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public int tryAgainSendingFwiDelay() {
        return tryAgainSendingFwiDelay;
    }
    
    /**@brief   Gets the Timeout value for the HM-10 Connect-to-Address Command.
     * 
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#hm10ConnectToAddressTimeout}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 28, 2023.
     */
    public int hm10ConnectToAddressTimeout() {
        return hm10ConnectToAddressTimeout;
    }
    
    /**@brief   Creates the corresponding Object Instance that
     *          matches/corresponds to the Concrete Product of the
     *          @ref ETX_OTA_Protocol_Factory with which this method was called
     *          with.
     * 
     * @details In other words, it will create the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocol}
     *          Type Object that matches/corresponds with the Concrete Creator
     *          of either the UART or BLE Hardware Protocol with which this
     *          method is called with.
     * @details On the other hand, depending on the Hardware Protocol, the
     *          created Object Instance will be initiated/created with only the
     *          data that it needs from the Concrete Creator Class with which
     *          this method is called with.
     * 
     * @note    This method should only be called from the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.sendPayload}
     *          method.
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 08, 2023.
     */
    public abstract com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocol createProtocolInstance();
}

/** @} */
