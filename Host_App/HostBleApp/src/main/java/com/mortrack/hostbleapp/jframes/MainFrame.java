/**@file
 * 
 * @defgroup Main_Frame Main Frame module
 * @{
 * 
 * @brief   This module contains the methods required to generate the Main Frame
 *          of the UI interface of the Host BLE App and to also handle the
 *          events programmed into it.
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 30, 2023.
 */


/* @brief   Package containing all the classes, methods, variables and code in
 *          general to generate the Frames of the Host BLE App UI.
 *
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 30, 2023.
 */
package com.mortrack.hostbleapp.jframes;

import com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocol;
import com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory;
import com.mortrack.hostbleapp.etxotaprotocol.UartEtxOtaProtocolFactory;
import java.io.File;
import com.fazecast.jSerialComm.SerialPort;
import com.mortrack.hostbleapp.etxotaprotocol.BleEtxOtaProtocolFactory;
import com.mortrack.hostbleapp.utils.TerminalProcess;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFileChooser;

/**@brief   Main Class containing all the methods required to generate, make
 *          visible and to handle the events of the Main Frame of the Host BLE
 *          App UI.
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    December 30, 2023.
 */
public class MainFrame extends javax.swing.JFrame {
    /**@brief   Designated BLE Item Name value for the
     *          {@link com.mortrack.hostbleapp.jframes.MainFrame.jComboBoxEtxOtaHwProtocol}
     *          Combo Box.
     */
    private static final String COMBOBOX_ITEM_NAME_BLE = "BLE";
    
    /**@brief   Length in bytes/characters of a Bluetooth Address.
     */
    private static final int BLUETOOTH_ADDRESS_SIZE = 12;
    
    /**@brief   Designated Baud-Rate value for the RS-232 Protocol side of the
     *          BLE module that is to be used for ETX OTA Transactions.
     */
    private static final int BLE_BAUDRATE = 9600;
    
    /**@brief   File Chooser Object used to pop-up a File Chooser Window
     *          whenever the user hits the
     *          {@link com.mortrack.hostbleapp.jframes.MainFrame.jButtonBrowsePayloadFilePath}
     *          Button so that he/she opens a Payload File.
     */
    private JFileChooser openFileChooser = new JFileChooser();
    
    /**@brief   Serial Port Object used to hold all the available Serial Ports
     *          on the current computer.
     */
    SerialPort[] comPorts = null;
    
    /**@brief   Index of the {@link com.mortrack.hostbleapp.jframes.MainFrame#comPorts}
     *          Serial Port Object that has been currently chosen by the user,
     * 
     * @note    The value of this parameter will be \c -1 whenever the user has
     *          never chosen a Serial Port and has also not search for any
     *          available Serial Ports on this computer yet.
     */
    int indexOfChosenComPort = -1;
            
    /**@brief   File Object of the currently chosen Payload File by the user.
     */
    private File payloadFile = null;
    
    /**@brief   Current ETX OTA Payload Type chosen by the user.
     * 
     * @details The valid values for this variable are as in
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaPayloadType}
     *          but in their respective integer values.
     */
    private short etxOtaPayloadType = (short) EtxOtaProtocolFactory.EtxOtaPayloadType.ETX_OTA_Application_Firmware_Image.getNumber();
    
    /**@brief   Current ETX OTA Hardware Protocol chosen by the user.
     * 
     * @details The valid values for this variable are as in
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.EtxOtaHardwareType}
     *          but in their respective integer values.
     */
    private short etxOtaHardwareProtocol = (short) EtxOtaProtocolFactory.EtxOtaHardwareType.ETX_OTA_hw_Protocol_UART.getNumber();
    
    /**@brief   Current ETX OTA RS232 Comport chosen by the user for the ETX OTA
     *          Protocol.
     * 
     * @details The valid and supported Comports are COM1, COM2, ...,
     *          COM32 in Windows, which stand for Comport Numbers 1, 2, ..., 32
     *          respectively.
     * @details For Linux based Operative Systems, valid Comports are ttyS0,
     *          ttyS1, ..., ttyS15 with the Comport Numbers 1, 2, ..., 16
     *          respectively. Other valid Comports for these O.S. are also
     *          ttyUSB0, ttyUSB1, ..., ttyUSB5 with the Comport Numbers 17, 18,
     *          ..., 22. Other valid Comports are also ttyAMA0, ttyAMA1,
     *          ttyACM0, ttyACM1, rfcomm0, rfcomm1, ircomm0, ircomm1, cuau0 and
     *          cuau1 with the Comport Numbers 23, 24, 25, 26, 27, 28, 29, 30,
     *          31 and 32 respectively.
     */
    private short etxOtaRs232ComportNumber;
    
    /**@brief   Current ETX OTA RS232 Comport chosen by the user for the Dongle
     *          Configurator API.
     * 
     * @details The valid and supported Comports are COM1, COM2, ...,
     *          COM32 in Windows, which stand for Comport Numbers 1, 2, ..., 32
     *          respectively.
     * @details For Linux based Operative Systems, valid Comports are ttyS0,
     *          ttyS1, ..., ttyS15 with the Comport Numbers 1, 2, ..., 16
     *          respectively. Other valid Comports for these O.S. are also
     *          ttyUSB0, ttyUSB1, ..., ttyUSB5 with the Comport Numbers 17, 18,
     *          ..., 22. Other valid Comports are also ttyAMA0, ttyAMA1,
     *          ttyACM0, ttyACM1, rfcomm0, rfcomm1, ircomm0, ircomm1, cuau0 and
     *          cuau1 with the Comport Numbers 23, 24, 25, 26, 27, 28, 29, 30,
     *          31 and 32 respectively.
     */
    private short dongleConfiguratorRs232ComportNumber;
    
    /**@brief   Timeout in seconds that it is desired to run the ETX OTA API
     *          program whenever requesting an ETX OTA Transaction from our host
     *          machine to the slave device.
     */
    private static final int ETX_OTA_API_TIMEOUT = 1800;
    
    /**@brief   Chosen Baud-rate with which we want the host to run the RS232
     *          protocol for the Dongle Configurator API.
     */
    private int dongleConfiguratorRs232Baudrate = 9600;
    
    /**@brief   Chosen Data-bits string with which we want the host to run the
     *          RS232 protocol for the Dongle Configurator API.
     * 
     * @details The valid data-bits in the
     *          <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a>
     *          that are used in the PcTool API are '5', '6', '7' and '8'.
     */
    private char dongleConfiguratorRs232ModeDatabits = '8';
    
    /**@brief   Chosen Parity string with which we want the host to run the
     *          RS232 protocol for the Dongle Configurator API.
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
    private char dongleConfiguratorRs232ModeParity = 'N';
    
    /**@brief   Chosen Stop-bit string with which we want the host to run the
     *          RS232 protocol for the Dongle Configurator API.
     * 
     * @note    The possible stop-bit values in RS232 are '1' or '2'.
     */
    private char dongleConfiguratorRs232ModeStopbits = '1';
    
    /**@brief   Chosen Flow Control decimal value to indicate with a 1 that we
     *          want the host to run the RS232 protocol, for the Dongle
     *          Configurator API, with Flow Control enabled, or otherwise with a
     *          decimal value of 0 to indicate to the host not to run the RS232
     *          protocol, for the Dongle Configurator API, with Flow Control.
     */
    private short dongleConfiguratorRs232IsFlowControl = 0;
    
    /**@brief   Delay in microseconds that is desired to request before having
     *          send a byte of data via the RS232 protocol whenever running the
     *          Dongle Configurator API.
     */
    private int dongleConfiguratorSendPacketBytesDelay = 100;
    
    /**@brief   Delay in microseconds that it is to be requested to apply each
     *          time before calling the RS232_PollComport() function of the
     *          <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a>
     *          that is used in the Dongle Configurator API.
     * 
     * @note    The <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a>
     *          suggests to place an interval of 100 milliseconds, but it did
     *          not worked for me that way. Instead, it worked for me with
     *          500ms.
     */
    private int dongleConfiguratorTeunizLibPollComportDelay = 500000;
    
    /**@brief   Designated File Execute Terminal Command towards the executable
     *          compiled file of the Dongle Configurator API that must be used
     *          whenever the Operative System at which the
     *          {@link com.mortrack.hostbleapp.jframes.MainFrame} Class is being
     *          run at is Windows.
     */
    private static final String WINDOWS_DONGLE_CONFIGURATOR_API_PATH = ".\\APIs\\dongleConfAPI\\Dongle_Configurator_API";
    
    /**@brief   Designated File Execute Terminal Command towards the executable
     *          compiled file of the Dongle Configurator API that must be used
     *          whenever the Operative System at which the
     *          {@link com.mortrack.hostbleapp.jframes} Class is being run at is
     *          a Linux based Operative System.
     */
    private static final String LINUX_DONGLE_CONFIGURATOR_API_PATH = "./APIs/dongleConfAPI/Dongle_Configurator_API";
    
    /**@brief   Constructor of the {@link com.mortrack.hostbleapp.jframes.MainFrame}
     *          Class, which initializes the UI components of the Main Frame of
     *          the Host BLE App UI and configures
     *          {@link com.mortrack.hostbleapp.jframes.MainFrame.openFileChooser}
     *          in Files Only Mode.
     * 
     * @note    This Constructor only initializes the UI components of the Main
     *          Frame, but does not make it visible. To make it visible, the
     *          @ref setVisible method must be used afterwards.
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    October 31, 2023.
     */
    public MainFrame() {
        initComponents();
        jLabelSlaveBtAddress.setVisible(false);
        jTextFieldSlaveBtAddress.setVisible(false);
        jButtonProgramBleDongleModule.setVisible(false);
        jLabelCustomData.setVisible(false);
        jTextFieldCustomData.setVisible(false);
        openFileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPanel1 = new javax.swing.JPanel();
        jButtonSendEtxOtaPayload = new javax.swing.JButton();
        jLabelEtxOtaPayloadType = new javax.swing.JLabel();
        jButtonBrowsePayloadFilePath = new javax.swing.JButton();
        jTextFieldPayloadFilePath = new javax.swing.JTextField();
        jLabelPayloadFilePath = new javax.swing.JLabel();
        jComboBoxEtxOtaPayloadType = new javax.swing.JComboBox<>();
        jLabel3 = new javax.swing.JLabel();
        jLabelEtxOtaHardwareProtocol = new javax.swing.JLabel();
        jComboBoxEtxOtaHwProtocol = new javax.swing.JComboBox<>();
        jLabelRs232Comport = new javax.swing.JLabel();
        jLabelSlaveBtAddress = new javax.swing.JLabel();
        jTextFieldSlaveBtAddress = new javax.swing.JTextField();
        jComboBoxRs232Comport = new javax.swing.JComboBox<>();
        jButtonSearch = new javax.swing.JButton();
        jLabelCustomData = new javax.swing.JLabel();
        jTextFieldCustomData = new javax.swing.JTextField();
        jButtonProgramBleDongleModule = new javax.swing.JButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

        jPanel1.setFont(new java.awt.Font("Arial", 0, 12)); // NOI18N

        jButtonSendEtxOtaPayload.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jButtonSendEtxOtaPayload.setText("Send ETX OTA Payload");
        jButtonSendEtxOtaPayload.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonSendEtxOtaPayloadActionPerformed(evt);
            }
        });

        jLabelEtxOtaPayloadType.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N
        jLabelEtxOtaPayloadType.setText("ETX OTA Payload Type:");

        jButtonBrowsePayloadFilePath.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jButtonBrowsePayloadFilePath.setText("Browse...");
        jButtonBrowsePayloadFilePath.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonBrowsePayloadFilePathActionPerformed(evt);
            }
        });

        jTextFieldPayloadFilePath.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N

        jLabelPayloadFilePath.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N
        jLabelPayloadFilePath.setText("Payload File Path:");

        jComboBoxEtxOtaPayloadType.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N
        jComboBoxEtxOtaPayloadType.setModel(new javax.swing.DefaultComboBoxModel<>(new String[] { "ETX OTA Application Firmware Image", "ETX OTA Bootloader Firmware Image", "ETX OTA Custom Data" }));
        jComboBoxEtxOtaPayloadType.addItemListener(new java.awt.event.ItemListener() {
            public void itemStateChanged(java.awt.event.ItemEvent evt) {
                jComboBoxEtxOtaPayloadTypeItemStateChanged(evt);
            }
        });

        jLabel3.setFont(new java.awt.Font("Tahoma", 0, 18)); // NOI18N
        jLabel3.setText("ETX OTA Protocol by Mortrack");

        jLabelEtxOtaHardwareProtocol.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N
        jLabelEtxOtaHardwareProtocol.setText("ETX OTA Hardware Protocol:");

        jComboBoxEtxOtaHwProtocol.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N
        jComboBoxEtxOtaHwProtocol.setModel(new javax.swing.DefaultComboBoxModel<>(new String[] { "UART", "BLE" }));
        jComboBoxEtxOtaHwProtocol.addItemListener(new java.awt.event.ItemListener() {
            public void itemStateChanged(java.awt.event.ItemEvent evt) {
                jComboBoxEtxOtaHwProtocolItemStateChanged(evt);
            }
        });

        jLabelRs232Comport.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N
        jLabelRs232Comport.setText("RS232 Comport:");

        jLabelSlaveBtAddress.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N
        jLabelSlaveBtAddress.setText("Slave Bluetooth Address:");

        jTextFieldSlaveBtAddress.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N

        jComboBoxRs232Comport.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N
        jComboBoxRs232Comport.addItemListener(new java.awt.event.ItemListener() {
            public void itemStateChanged(java.awt.event.ItemEvent evt) {
                jComboBoxRs232ComportItemStateChanged(evt);
            }
        });

        jButtonSearch.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jButtonSearch.setText("Search...");
        jButtonSearch.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonSearchActionPerformed(evt);
            }
        });

        jLabelCustomData.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N
        jLabelCustomData.setText("Custom Data:");

        jTextFieldCustomData.setFont(new java.awt.Font("Tahoma", 0, 12)); // NOI18N

        jButtonProgramBleDongleModule.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jButtonProgramBleDongleModule.setText("Program BLE Dongle Module");
        jButtonProgramBleDongleModule.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonProgramBleDongleModuleActionPerformed(evt);
            }
        });

        org.jdesktop.layout.GroupLayout jPanel1Layout = new org.jdesktop.layout.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(jPanel1Layout.createSequentialGroup()
                .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .add(jLabel3)
                .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
            .add(jPanel1Layout.createSequentialGroup()
                .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                    .add(org.jdesktop.layout.GroupLayout.TRAILING, jPanel1Layout.createSequentialGroup()
                        .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
                            .add(org.jdesktop.layout.GroupLayout.LEADING, jPanel1Layout.createSequentialGroup()
                                .add(jLabelSlaveBtAddress)
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
                                .add(jTextFieldSlaveBtAddress))
                            .add(org.jdesktop.layout.GroupLayout.LEADING, jPanel1Layout.createSequentialGroup()
                                .add(jLabelRs232Comport)
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                                .add(jComboBoxRs232Comport, 0, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                            .add(org.jdesktop.layout.GroupLayout.LEADING, jPanel1Layout.createSequentialGroup()
                                .add(jLabelEtxOtaHardwareProtocol)
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                                .add(jComboBoxEtxOtaHwProtocol, 0, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                            .add(org.jdesktop.layout.GroupLayout.LEADING, jPanel1Layout.createSequentialGroup()
                                .add(jLabelEtxOtaPayloadType)
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                                .add(jComboBoxEtxOtaPayloadType, 0, 247, Short.MAX_VALUE))
                            .add(jPanel1Layout.createSequentialGroup()
                                .add(jLabelPayloadFilePath)
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                                .add(jTextFieldPayloadFilePath))
                            .add(jPanel1Layout.createSequentialGroup()
                                .add(jLabelCustomData)
                                .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
                                .add(jTextFieldCustomData)))
                        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                        .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                            .add(jButtonBrowsePayloadFilePath)
                            .add(org.jdesktop.layout.GroupLayout.TRAILING, jButtonSearch)))
                    .add(org.jdesktop.layout.GroupLayout.TRAILING, jButtonSendEtxOtaPayload, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .add(jButtonProgramBleDongleModule, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(org.jdesktop.layout.GroupLayout.TRAILING, jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .add(jLabel3)
                .add(78, 78, 78)
                .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                    .add(jTextFieldPayloadFilePath, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                    .add(jLabelPayloadFilePath)
                    .add(jButtonBrowsePayloadFilePath))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                    .add(jLabelCustomData)
                    .add(jTextFieldCustomData, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                    .add(jLabelEtxOtaPayloadType)
                    .add(jComboBoxEtxOtaPayloadType, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .add(18, 18, 18)
                .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                    .add(jLabelEtxOtaHardwareProtocol)
                    .add(jComboBoxEtxOtaHwProtocol, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .add(18, 18, 18)
                .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                    .add(jLabelRs232Comport)
                    .add(jComboBoxRs232Comport, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                    .add(jButtonSearch))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                    .add(jLabelSlaveBtAddress)
                    .add(jTextFieldSlaveBtAddress, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED, 21, Short.MAX_VALUE)
                .add(jButtonProgramBleDongleModule)
                .add(10, 10, 10)
                .add(jButtonSendEtxOtaPayload, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 40, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(10, 10, 10))
        );

        org.jdesktop.layout.GroupLayout layout = new org.jdesktop.layout.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(layout.createSequentialGroup()
                .add(23, 23, 23)
                .add(jPanel1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .add(19, 19, 19))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(layout.createSequentialGroup()
                .add(33, 33, 33)
                .add(jPanel1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jButtonProgramBleDongleModuleActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonProgramBleDongleModuleActionPerformed
        /* Validate the currently chosen Serial Port by the user. */
        if (indexOfChosenComPort == -1) {
            new ErrorStatusDialogFrame("No Serial Port has currently been selected.", "Connect your device, click the Search Button and select the device.", this);
            return;
        }

        /* Get and RS232 Serial Port Number requested by the user. */
        dongleConfiguratorRs232ComportNumber = serialPortNameToSerialPortNumber(comPorts[indexOfChosenComPort].getSystemPortName());
        
        /** <b>Local {@link com.mortrack.hostbleapp.utils.TerminalProcess} Type variable terminalProcess:</b> Used to hold a Terminal Process Object, which will be later used for communicating with the Dongle Configurator API made by Mortrack and to request to it to Configure the desired HM-10 BT Device in Central Mode. */
        TerminalProcess terminalProcess = new TerminalProcess();
        /** <b>Local String Type variable ret:</b> Used to hold the String value of the interpreted integral number of whatever exception code value is returned by the Dongle Configurator API made by Mortrack. Note that the returned values will stand for the {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus} Exception Codes but in their respective interpreted integral number, which is to later be converted to its corresponding integer value via the {@link com.mortrack.hostbleapp.etxotaprotocol.bleEtxOtaProtocol.etxOtaStatusHandler} function. */
        String ret = "";
        /** <b>Local String Type variable command:</b> Used to populate it with the Terminal Command required for executing the PcTool BLE API made by Mortrack and to give to it the desired Configuration Parameters and also to request to it to send a desired Payload under those Configuration Settings. */
        String command;
        if (terminalProcess.isWindows()) {
            command = WINDOWS_DONGLE_CONFIGURATOR_API_PATH + " "
                    + dongleConfiguratorRs232ComportNumber + " "
                    + dongleConfiguratorRs232Baudrate + " "
                    + dongleConfiguratorRs232ModeDatabits + " "
                    + dongleConfiguratorRs232ModeParity + " "
                    + dongleConfiguratorRs232ModeStopbits + " "
                    + dongleConfiguratorRs232IsFlowControl + " "
                    + dongleConfiguratorSendPacketBytesDelay + " "
                    + dongleConfiguratorTeunizLibPollComportDelay;
        } else {
            command = LINUX_DONGLE_CONFIGURATOR_API_PATH + " "
                    + dongleConfiguratorRs232ComportNumber + " "
                    + dongleConfiguratorRs232Baudrate + " "
                    + dongleConfiguratorRs232ModeDatabits + " "
                    + dongleConfiguratorRs232ModeParity + " "
                    + dongleConfiguratorRs232ModeStopbits + " "
                    + dongleConfiguratorRs232IsFlowControl + " "
                    + dongleConfiguratorSendPacketBytesDelay + " "
                    + dongleConfiguratorTeunizLibPollComportDelay;
        }
        try {
            ret = terminalProcess.runExecutableFile(command, ETX_OTA_API_TIMEOUT);
        } catch (Exception ex) {
            Logger.getLogger(MainFrame.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        /* Send Status Dialog Message to the user to inform him/her of how the Dongle Configurator API Request concluded. */
        if (dongleConfiguratorStatusHandler(ret) != EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_OK) {
            new ErrorStatusDialogFrame("Dongle Configurator Exception Code = " + dongleConfiguratorStatusHandler(ret).getNumber(), "The BLE Dongle Device could not be programmed as Central Mode.", this);
            return;
        }
        new SuccessStatusDialogFrame("The BLE Dongle Device has been successfully programmed.", this);
    }//GEN-LAST:event_jButtonProgramBleDongleModuleActionPerformed

    private void jButtonSearchActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonSearchActionPerformed
        /* Update the value of the Serial Ports parameter of this Class. */
        comPorts = SerialPort.getCommPorts();

        /* Clear the Items from the RS232 Comport Combo-box and repopulate it with the name of the currently available Serial Ports in this Computer. */
        jComboBoxRs232Comport.removeAllItems();
        indexOfChosenComPort = -1; // Reset the value of the chosen Serial Port parameter of this Class.
        for (SerialPort comPort: comPorts)
        {
            jComboBoxRs232Comport.addItem(comPort.getSystemPortName());
            if (serialPortNameToSerialPortNumber(comPort.getSystemPortName()) == -1) {
                jComboBoxRs232Comport.removeAllItems();
                new ErrorStatusDialogFrame("An invalid Serial Port has been read from your Computer.", "", this);
                return;
            }
        }

        /* Update the value of the index of the chosen Serial Port parameter of this Class. */
        indexOfChosenComPort = 0;
    }//GEN-LAST:event_jButtonSearchActionPerformed

    private void jComboBoxRs232ComportItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jComboBoxRs232ComportItemStateChanged
        /* Update the value of the index of the chosen Serial Port parameter of this Class. */
        if (indexOfChosenComPort != -1) {
            indexOfChosenComPort = jComboBoxRs232Comport.getSelectedIndex();
        }
    }//GEN-LAST:event_jComboBoxRs232ComportItemStateChanged

    private void jComboBoxEtxOtaPayloadTypeItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jComboBoxEtxOtaPayloadTypeItemStateChanged
        /* Update the value of the ETX OTA Payload Type parameter of this Class. */
        etxOtaPayloadType = (short) jComboBoxEtxOtaPayloadType.getSelectedIndex();

        /* Change the Visibility of the corresponding UI fields/items according to the new value of the ETX OTA Payload Type parameter. */
        if (etxOtaPayloadType==EtxOtaProtocolFactory.EtxOtaPayloadType.ETX_OTA_Application_Firmware_Image.getNumber() ||
            etxOtaPayloadType==EtxOtaProtocolFactory.EtxOtaPayloadType.ETX_OTA_Bootloader_Firmware_Image.getNumber()) {
            jLabelPayloadFilePath.setVisible(true);
            jTextFieldPayloadFilePath.setVisible(true);
            jButtonBrowsePayloadFilePath.setVisible(true);
            jLabelCustomData.setVisible(false);
            jTextFieldCustomData.setVisible(false);
        } else if (etxOtaPayloadType == EtxOtaProtocolFactory.EtxOtaPayloadType.ETX_OTA_Custom_Data.getNumber()) {
            jLabelPayloadFilePath.setVisible(false);
            jTextFieldPayloadFilePath.setVisible(false);
            jButtonBrowsePayloadFilePath.setVisible(false);
            jLabelCustomData.setVisible(true);
            jTextFieldCustomData.setVisible(true);
        }
    }//GEN-LAST:event_jComboBoxEtxOtaPayloadTypeItemStateChanged

    /**@brief   Event Handler method for whenever there is an Action Performed
     *          on the {@link com.mortrack.hostbleapp.jframes.MainFrame.jButtonBrowsePayloadFilePath}
     *          Button.
     * 
     * @details Whenever this Event Handler method is called, a File Chooser
     *          Window will pop-up and, if a File was opened from it, then it
     *          will display its File Path in
     *          {@link com.mortrack.hostbleapp.jframes.MainFrame.jTextFieldPayloadFilePath} .
     * 
     * @param evt   Action Performed Event Handler Object for the
     *              {@link com.mortrack.hostbleapp.jframes.MainFrame.jButtonBrowsePayloadFilePath}
     *              Button.
     *
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 02, 2023.
     */
    private void jButtonBrowsePayloadFilePathActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonBrowsePayloadFilePathActionPerformed
        /** <b>Local int variable ret:</b> Used to hold the return state of {@link JFileChooser}. */
        int ret = openFileChooser.showOpenDialog(this);

        if (ret == JFileChooser.APPROVE_OPTION) {
            jTextFieldPayloadFilePath.setText(openFileChooser.getSelectedFile().getAbsolutePath());
        }
    }//GEN-LAST:event_jButtonBrowsePayloadFilePathActionPerformed

    /**@brief   Event Handler method for whenever there is an Action Performed
     *          on the {@link com.mortrack.hostbleapp.jframes.MainFrame.jButtonSendEtxOtaPayload}
     *          Button.
     * 
     * @details Whenever this Event Handler method is called, the remaining data
     *          to be validated from the fields/items of the UI will be
     *          validated (some are validated the moment their values change).
     *          Then, if everything goes well, then the requested Payload will
     *          be sent to the chosen slave device via the ETX OTA Communication
     *          Protocol, the selected Hardware Protocol and also under the
     *          configurations defined in the UI by the user.
     * @details A J Dialog Form will be shown to inform the user of the result
     *          of having started an ETX OTA Transaction.
     * 
     * @param evt   Action Performed Event Handler Object for the
     *              {@link com.mortrack.hostbleapp.jframes.MainFrame.jButtonSendEtxOtaPayload}
     *              Button.
     *
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 16, 2023
     */
    private void jButtonSendEtxOtaPayloadActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonSendEtxOtaPayloadActionPerformed
        /* Validate the existance of the Firmware Image to be sent in case of an ETX OTA Firmware Update request. Otherwise, validate the ETX OTA Custom Data. */
        /** <b>Local String Type variable customData:</b> Used to hold the String value of the Custom Data to be sent during an ETX OTA Custom Data Transaction requested by the user. */
        String customData = jTextFieldCustomData.getText();
        if (etxOtaPayloadType != EtxOtaProtocolFactory.EtxOtaPayloadType.ETX_OTA_Custom_Data.getNumber()) {
            payloadFile = new File(jTextFieldPayloadFilePath.getText());
            if (!payloadFile.exists()) {
                new ErrorStatusDialogFrame("", "The given Payload File Path does not exist.", this);
                return;
            }
        } else {
            if (customData.isEmpty()) {
                new ErrorStatusDialogFrame("An ETX OTA Custom Data Transaction has been requested without", "having specifying any data to be sent to the slave device.", this);
                return;
            }
        }

        /* Validate the currently chosen Serial Port by the user. */
        if (indexOfChosenComPort == -1) {
            new ErrorStatusDialogFrame("No Serial Port has currently been selected.", "Connect your device, click the Search Button and select the device.", this);
            return;
        }

        /* Get and RS232 Serial Port Number requested by the user. */
        etxOtaRs232ComportNumber = serialPortNameToSerialPortNumber(comPorts[indexOfChosenComPort].getSystemPortName());

        /* Send the Payload to the requested slave device with the requested Configurations and Hardware Protocol with the ETX OTA Protocol. */
        /** <b>Local {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocol} Object variable etxOtaApi:</b> Used to hold the Concrete Product of the {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory} Factory Class that corresponds to the Hardware Protocol and ETX OTA Configuration Settings with which the user wants to start an ETX OTA Transaction. */
        EtxOtaProtocol etxOtaApi;
        if (etxOtaHardwareProtocol == EtxOtaProtocolFactory.EtxOtaHardwareType.ETX_OTA_hw_Protocol_UART.getNumber()) {
            /* NOTE: UART Hardware Protocol is currently chosen. */
            EtxOtaProtocolFactory uartEtxOtaFact = new UartEtxOtaProtocolFactory(etxOtaRs232ComportNumber);
            
            /* Send Payload via UART Hardware Protocol. */
            if (etxOtaPayloadType != EtxOtaProtocolFactory.EtxOtaPayloadType.ETX_OTA_Custom_Data.getNumber()) {
                /* NOTE: An ETX OTA Firmware Update has been requested. */
                etxOtaApi = uartEtxOtaFact.sendPayload(payloadFile.getAbsolutePath(), etxOtaPayloadType, ETX_OTA_API_TIMEOUT);
            } else {
                /* NOTE: An ETX OTA Custom Data Transaction has been requested. */
                etxOtaApi = uartEtxOtaFact.sendPayload(customData, etxOtaPayloadType, ETX_OTA_API_TIMEOUT);
            }
        } else if (etxOtaHardwareProtocol == EtxOtaProtocolFactory.EtxOtaHardwareType.ETX_OTA_hw_Protocol_BLE.getNumber()) {
            /* NOTE: BLE Hardware Protocol is currently chosen. */
            /* Validate the given Bluetooth Address of the Slave Device that it is desired to connect to. */
            String slaveBtAddress = jTextFieldSlaveBtAddress.getText();
            if ((slaveBtAddress.isEmpty()) || (slaveBtAddress.length()!=BLUETOOTH_ADDRESS_SIZE)) {
                new ErrorStatusDialogFrame("Expected a Bluetooth Address of " + BLUETOOTH_ADDRESS_SIZE + " characters.", "Please set a valid Bluetooth Address.", this);
                return;
            }
            
            /* Send Payload via BLE Hardware Protocol. */
            EtxOtaProtocolFactory bleEtxOtaFact = new BleEtxOtaProtocolFactory(etxOtaRs232ComportNumber, BLE_BAUDRATE, slaveBtAddress, 3000000);
            if (etxOtaPayloadType != EtxOtaProtocolFactory.EtxOtaPayloadType.ETX_OTA_Custom_Data.getNumber()) {
                /* NOTE: An ETX OTA Firmware Update has been requested. */
                etxOtaApi = bleEtxOtaFact.sendPayload(payloadFile.getAbsolutePath(), etxOtaPayloadType, ETX_OTA_API_TIMEOUT);
            } else {
                /* NOTE: An ETX OTA Custom Data Transaction has been requested. */
                etxOtaApi = bleEtxOtaFact.sendPayload(customData, etxOtaPayloadType, ETX_OTA_API_TIMEOUT);
            }
        } else {
            new ErrorStatusDialogFrame("An invalid ETX OTA Hardware Protocol has been chosen.", "", this);
            return;
        }

        /* Send Status Dialog Message to the user to inform him/her of how the ETX OTA Request concluded. */
        if (etxOtaApi.lastEtxOtaStatus() != EtxOtaProtocolFactory.EtxOtaStatus.ETX_OTA_EC_OK) {
            new ErrorStatusDialogFrame("ETX OTA Exception Code = " + etxOtaApi.lastEtxOtaStatus().getNumber(), "The requested Payload could not be sent to the slave device.", this);
            return;
        }
        new SuccessStatusDialogFrame("The requested Payload has been successfully sent to the slave device.", this);
    }//GEN-LAST:event_jButtonSendEtxOtaPayloadActionPerformed

    private void jComboBoxEtxOtaHwProtocolItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jComboBoxEtxOtaHwProtocolItemStateChanged
        /* Update the value of the ETX OTA Hardware Protocol parameter of this Class. */
        etxOtaHardwareProtocol = (short) jComboBoxEtxOtaHwProtocol.getSelectedIndex();
                
        /* Change the Visibility of the corresponding UI fields/items according to the new value of the ETX OTA Hardware Protocol parameter. */
        if (etxOtaHardwareProtocol == EtxOtaProtocolFactory.EtxOtaHardwareType.ETX_OTA_hw_Protocol_UART.getNumber()) {
            jLabelSlaveBtAddress.setVisible(false);
            jTextFieldSlaveBtAddress.setVisible(false);
            jButtonProgramBleDongleModule.setVisible(false);
        } else if (etxOtaHardwareProtocol == EtxOtaProtocolFactory.EtxOtaHardwareType.ETX_OTA_hw_Protocol_BLE.getNumber()) {
            jLabelSlaveBtAddress.setVisible(true);
            jTextFieldSlaveBtAddress.setVisible(true);
            jButtonProgramBleDongleModule.setVisible(true);
        }
    }//GEN-LAST:event_jComboBoxEtxOtaHwProtocolItemStateChanged

       
    /**@brief   Gets the equivalent Serial Port Number with respect to a given
     *          Serial Port Name.
     * 
     * @param portName  String of the Serial Port Name.
     * 
     * @return  Either a \c -1 whenever an invalid Serial Port Name is given at
     *          the \p portName param. Otherwise, the actual equivalent Serial
     *          Port Number.
     *
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 16, 2023.
     */
    private short serialPortNameToSerialPortNumber(String portName) {
        switch (portName) {
            case "COM1":
            case "ttyS0":
                return 1;
            case "COM2":
            case "ttyS1":
                return 2;
            case "COM3":
            case "ttyS2":
                return 3;
            case "COM4":
            case "ttyS3":
                return 4;
            case "COM5":
            case "ttyS4":
                return 5;
            case "COM6":
            case "ttyS5":
                return 6;
            case "COM7":
            case "ttyS6":
                return 7;
            case "COM8":
            case "ttyS7":
                return 8;
            case "COM9":
            case "ttyS8":
                return 9;
            case "COM10":
            case "ttyS9":
                return 10;
            case "COM11":
            case "ttyS10":
                return 11;
            case "COM12":
            case "ttyS11":
                return 12;
            case "COM13":
            case "ttyS12":
                return 13;
            case "COM14":
            case "ttyS13":
                return 14;
            case "COM15":
            case "ttyS14":
                return 15;
            case "COM16":
            case "ttyS15":
                return 16;
            case "COM17":
            case "ttyUSB0":
                return 17;
            case "COM18":
            case "ttyUSB1":
                return 18;
            case "COM19":
            case "ttyUSB2":
                return 19;
            case "COM20":
            case "ttyUSB3":
                return 20;
            case "COM21":
            case "ttyUSB4":
                return 21;
            case "COM22":
            case "ttyUSB5":
                return 22;
            case "COM23":
            case "ttyAMA0":
                return 23;
            case "COM24":
            case "ttyAMA1":
                return 24;
            case "COM25":
            case "ttyACM0":
                return 25;
            case "COM26":
            case "ttyACM1":
                return 26;
            case "COM27":
            case "rfcomm0":
                return 27;
            case "COM28":
            case "rfcomm1":
                return 28;
            case "COM29":
            case "ircomm0":
                return 29;
            case "COM30":
            case "ircomm1":
                return 30;
            case "COM31":
            case "cuau0":
                return 31;
            case "COM32":
            case "cuau1":
                return 32;
            case "cuau2":
                return 33;
            case "cuau3":
                return 34;
            case "cuaU0":
                return 35;
            case "cuaU1":
                return 36;
            case "cuaU2":
                return 37;
            case "cuaU3":
                return 38;
            default:
                return -1;
        }
    }
    
    /**@brief   Parses and gets the equivalent of the
     *          {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus}
     *          Type with respect to a given String value with an interpreted
     *          integral number.
     * 
     * @param response  Should hold the String value that is to be interpreted
     *                  as an integral number to be able to get its
     *                  corresponding
     *                  {@link com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus}
     *                  Type.
     * 
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_OK if \p response param = "0".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_STOP if \p response param = "1".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_NR if \p response param = "2".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_NA if \p response param = "3".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_ERR if \p response param = "4" or if an invalid case gives place.
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_INV_CMD_LINE_ARG if \p response param = "5".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_OPEN_COM_ERR if \p response param = "7".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_INIT_ERR if \p response param = "21".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_AT_CMD_ERR if \p response param = "22".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_TYPE_CMD_ERR if \p response param = "23".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_RESET_CMD_ERR if \p response param = "24".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_CONN_CMD_ERR if \p response param = "25".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_RENEW_CMD_ERR if \p response param = "26".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_ROLE_CMD_ERR if \p response param = "27".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_IMME_CMD_ERR if \p response param = "28".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_NOTI_CMD_ERR if \p response param = "29".
     * @retval  com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_MODE_CMD_ERR if \p response param = "30".
     * 
     * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
     * @date    December 30, 2023.
     */
    private com.mortrack.hostbleapp.etxotaprotocol.EtxOtaProtocolFactory.DongleConfiguratorStatus dongleConfiguratorStatusHandler(String response) {
        switch (response) {
            case "0":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_OK;
            case "1":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_STOP;
            case "2":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_NR;
            case "3":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_NA;
            case "4":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_ERR;
            case "5":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_INV_CMD_LINE_ARG;
            case "7":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_OPEN_COM_ERR;
            case "21":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_INIT_ERR;
            case "22":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_AT_CMD_ERR;
            case "23":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_TYPE_CMD_ERR;
            case "24":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_RESET_CMD_ERR;
            case "25":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_CONN_CMD_ERR;
            case "26":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_RENEW_CMD_ERR;
            case "27":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_ROLE_CMD_ERR;
            case "28":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_IMME_CMD_ERR;
            case "29":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_NOTI_CMD_ERR;
            case "30":
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_BLE_MODE_CMD_ERR;   
            default:
                return EtxOtaProtocolFactory.DongleConfiguratorStatus.DONGLE_CONFIGURATOR_EC_ERR;
        }
    }
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jButtonBrowsePayloadFilePath;
    private javax.swing.JButton jButtonProgramBleDongleModule;
    private javax.swing.JButton jButtonSearch;
    private javax.swing.JButton jButtonSendEtxOtaPayload;
    private javax.swing.JComboBox<String> jComboBoxEtxOtaHwProtocol;
    private javax.swing.JComboBox<String> jComboBoxEtxOtaPayloadType;
    private javax.swing.JComboBox<String> jComboBoxRs232Comport;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabelCustomData;
    private javax.swing.JLabel jLabelEtxOtaHardwareProtocol;
    private javax.swing.JLabel jLabelEtxOtaPayloadType;
    private javax.swing.JLabel jLabelPayloadFilePath;
    private javax.swing.JLabel jLabelRs232Comport;
    private javax.swing.JLabel jLabelSlaveBtAddress;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JTextField jTextFieldCustomData;
    private javax.swing.JTextField jTextFieldPayloadFilePath;
    private javax.swing.JTextField jTextFieldSlaveBtAddress;
    // End of variables declaration//GEN-END:variables
}

/** @} */
