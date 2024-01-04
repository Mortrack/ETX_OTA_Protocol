/**@addtogroup ETX_OTA_Protocol_Factory
 * @{
 */

/**@file
 * 
 * @defgroup ETX_OTA_Protocol_Interface ETX OTA Protocol Interface module
 * @{
 * 
 * @brief   This module contains template methods used by the Concrete Products
 *          of the
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory}
 *          Factory Class.
 * 
 * @note    The Concrete Products that use this Interface Class are
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.UartEtxOtaProtocol}
 *          and {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocol}.
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 16, 2023.
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

/**@brief   This Interface provides several template methods for the Concrete
 *          Products
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.UartEtxOtaProtocol}
 *          and {@link com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocol}
 *          that are within the
 *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory}
 *          Factory Class.
 * 
 * @details The provided methods from this Interface should enable the
 *          implementer to initialize and send Payload Data to slave devices via
 *          the ETX OTA Communication Protocol over the already chosen Hardware
 *          Protocol (i.e., either UART or BLE).
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 16, 2023.
 */
public interface EtxOtaProtocol {
    /**@brief   Sends a desired Payload by using the ETX OTA Communication
     *          Protocol in the corresponding Hardware Protocol.
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
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OK
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_NR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_NA
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_INV_CMD_LINE_ARG
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_UNRECOG_PAYLOAD
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OPEN_COM_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OPEN_FILE_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_READ_FILE_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_START_CMD_SEND_DATA_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_START_CMD_NACK_RESP
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_HEADER_PCKT_SEND_DATA_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_HEADER_PCKT_NACK_RESP
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_DATA_PCKT_SEND_DATA_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_DATA_PCKT_NACK_RESP
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_END_CMD_SEND_DATA_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_END_CMD_NACK_RESP
     * 
     * @note	For more details on the returned values listed, see
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 16, 2023.
     */
    com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus startEtxOtaTransaction(
            String payloadPathOrData,
            short etxOtaPayloadType,
            int timeout);
    
    /**@brief   Gets the ETX OTA Status that was obtained from latest ETX OTA
     *          Transaction that was made between our host machine and the slave
     *          device.
     * 
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OK
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_NR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_NA
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_INV_CMD_LINE_ARG
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_UNRECOG_PAYLOAD
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OPEN_COM_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OPEN_FILE_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_READ_FILE_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_START_CMD_SEND_DATA_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_START_CMD_NACK_RESP
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_HEADER_PCKT_SEND_DATA_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_HEADER_PCKT_NACK_RESP
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_DATA_PCKT_SEND_DATA_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_DATA_PCKT_NACK_RESP
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_END_CMD_SEND_DATA_ERR
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_END_CMD_NACK_RESP
     * 
     * @note	For more details on the returned values listed, see
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus}
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 02, 2023.
     */
    com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaStatus lastEtxOtaStatus();
}

/** @} */

/** @} */
