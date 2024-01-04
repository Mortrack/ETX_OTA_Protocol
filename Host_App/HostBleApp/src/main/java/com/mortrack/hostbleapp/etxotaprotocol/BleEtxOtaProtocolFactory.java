/**@addtogroup ETX_OTA_Protocol_Factory
 * @{
 */

/**@file
 * 
 * @defgroup BLE_ETX_OTA_Protocol_Factory BLE ETX OTA Protocol Concrete Creator module
 * @{
 * 
 * @brief   This module contains the constructors and method required to enable
 *          the application to create an Object Instance of a Create Product
 *          from the @ref ETX_OTA_Protocol_Factory Factory, which happens to be
 *          the @ref BLE_ETX_OTA_Protocol Object for this particular case.
 * 
 * @details The resulting Object Instance created in this module is used by the
 *          @ref ETX_OTA_Protocol_Factory Factory Class in order to both provide
 *          the implementer with this created Object Instance and to also send
 *          an ETX OTA Payload via the BLE Hardware Protocol to a desired slave
 *          device.
 * 
 * @details For details on some friendly code example of how to use the 
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory}
 *          Factory Class, see the following:
 *          <ul>
 *              <li>@ref UART_ETX_OTA_Protocol </li>
 *              <li>@ref BLE_ETX_OTA_Protocol </li>
 *          </ul>
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

/**@brief   This Class provides several Constructor Classes and the method
 *          required to enable the @ref ETX_OTA_Protocol_Factory Factory Class
 *          to create and get an Object Instance of the
 *          @ref BLE_ETX_OTA_Protocol Create Product.
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 30, 2023.
 */
public class BleEtxOtaProtocolFactory extends EtxOtaProtocolFactory {
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocolFactory}
     *          Class, which persists a custom value for the Comport Number.
     * 
     * @param   pComportNumber  See {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory#comportNumber} for more details.
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 30, 2023.
     */
    public BleEtxOtaProtocolFactory(short pComportNumber) {
        /* Invoke the corresponding parent class constructor. */
        super(pComportNumber);
    }
    
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocolFactory}
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
     * @date    December 30, 2023.
     */
    public BleEtxOtaProtocolFactory(
            short pComportNumber,
            int pRs232Baudrate,
            String pSlaveBtAddress,
            int pHm10ConnectToAddressTimeout) {
        /* Invoke the corresponding parent class constructor. */
        super(pComportNumber,
                pRs232Baudrate,
                pSlaveBtAddress,
                pHm10ConnectToAddressTimeout);
    }
    
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocolFactory}
     *          Class, which persists a custom value for the:
     *          <ul>
     *              <li>Comport Number</li>
     *              <li>RS232 Baud rate</li>
     *              <li>Bluetooth Address of the Slave/External Device</li>
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
     * @date    December 30, 2023.
     */
    public BleEtxOtaProtocolFactory(
            short pComportNumber,
            int pRs232Baudrate,
            String pSlaveBtAddress,
            char pRs232ModeDatabits,
            char pRs232ModeParity,
            char pRs232ModeStopbits,
            short pRs232IsFlowControl,
            int pHm10ConnectToAddressTimeout) {
        /* Invoke the corresponding parent class constructor. */
        super(pComportNumber,
                pRs232Baudrate,
                pSlaveBtAddress,
                pRs232ModeDatabits,
                pRs232ModeParity,
                pRs232ModeStopbits,
                pRs232IsFlowControl,
                pHm10ConnectToAddressTimeout);
    }
    
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocolFactory}
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
     *              <li>Delay in microseconds before calling the RS232_PollComport() function that is used in the PcTool BLE API</li>
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
     * @date    December 30, 2023.
     */
    public BleEtxOtaProtocolFactory(
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
        /* Invoke the corresponding parent class constructor. */
        super(pComportNumber,
                pRs232Baudrate,
                pSlaveBtAddress,
                pRs232ModeDatabits,
                pRs232ModeParity,
                pRs232ModeStopbits,
                pRs232IsFlowControl,
                pSendPacketBytesDelay,
                pTeunizLibPollComportDelay,
                pTryAgainSendingFwiDelay,
                pHm10ConnectToAddressTimeout);
    }
    
    /**@brief   Constructor of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocolFactory}
     *          Class, which persists a custom value for the:
     *          <ul>
     *              <li>Flash Memory page size in bytes of the slave device</li>
     *              <li>Bootloader Firmware size in Flash Memory Pages designated on the slave device</li>
     *              <li>Application Firmware size in Flash Memory Pages designated on the slave device</li>
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
     * @date    December 30, 2023.
     */
    public BleEtxOtaProtocolFactory(
            short pFlashPageSizeInBytes,
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
        /* Invoke the corresponding parent class constructor. */
        super(pFlashPageSizeInBytes,
                pEtxBlPageSize,
                pEtxAppPageSize,
                pComportNumber,
                pRs232Baudrate,
                pSlaveBtAddress,
                pRs232ModeDatabits,
                pRs232ModeParity,
                pRs232ModeStopbits,
                pRs232IsFlowControl,
                pSendPacketBytesDelay,
                pTeunizLibPollComportDelay,
                pTryAgainSendingFwiDelay,
                pHm10ConnectToAddressTimeout);
    }
    
    /**
     * {@inheritDoc}
     * @return  {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocol}
     *          Concrete Product Object Instance.
     */
    @Override
    public EtxOtaProtocol createProtocolInstance() {
        return new BleEtxOtaProtocol(
                flashPageSizeInBytes(),
                etxBlPageSize(),
                etxAppPageSize(),
                comportNumber(),
                rs232Baudrate(),
                slaveBtAddress(),
                rs232ModeDatabits(),
                rs232ModeParity(),
                rs232ModeStopbits(),
                isRs232FlowControl(),
                sendPacketBytesDelay(),
                teunizLibPollComportDelay(),
                tryAgainSendingFwiDelay(),
                hm10ConnectToAddressTimeout());
    }
}

/** @} */

/** @} */
