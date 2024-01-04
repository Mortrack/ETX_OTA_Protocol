# Compilation and execution instructions of the main.c program
Follow the steps and explanations given in the following content to be able to successfully compile and execute the
main.c program, whose actual purpose is to be used as an API so that other programs in a different programming language
other than C can just call the main.c program to take care of applying the ETX OTA Communication Protocol via the BLE
Hardware Protocol.

## Configuring the program with custom user settings
While on the API version of the ETX OTA Protocol (i.e., the main.c program provided in this project) requires most of
the default settings for the ETX OTA Protocol to be given whenever calling the API provided via the main.c program,
there are a few other default settings in this project that are as defined in the "etx_ota_config.h" file.
If you were to require different settings than those ones, it is suggested to change the following define from that
file:

```c
#define ENABLE_APP_ETX_OTA_CONFIG              (0)          /**< @brief Flag used to enable the use of @ref app_etx_ota_config with a 1 or, otherwise to disable it with a 0. */
```

such that the value of that definition is changed to 1 as shown in the following code illustration:

```c
#define ENABLE_APP_ETX_OTA_CONFIG              (1)          /**< @brief Flag used to enable the use of @ref app_etx_ota_config with a 1 or, otherwise to disable it with a 0. */
```

This will allow you to enable the use of the "app_etx_ota_config.h" file, where you can call whatever definition
contained inside the "etx_ota_config.h" but into "app_etx_ota_config.h" with whatever custom value you desire. This will
allow you to set custom values for the program settings in a separate custom file but without rewriting the default
values.

Now that this has been explained, proceed to set the desired configuration settings for this program.

## Steps to compile the program
To compile this program, run the below command to compile the application.

```bash
$ gcc main.c etx_ota_protocol_host.c HM10_ble_driver/Src/hm10_ble_driver.c RS232/rs232.c -IRS232 -Wall -Wextra -o2 -o ETX_OTA_Protocol_BLE_API
```

**NOTE:** To be able to compile this program, make sure you have at GCC version >= 11.4.0

## Steps to execute the program
Once you have built the application, then execute it by using the following below syntax as a reference:

```bash
$ ./PATH_TO_THE_COMPILED_FILE COMPORT_NUMBER PAYLOAD_SIZE PAYLOAD_PATH_OR_DATA ETX_OTA_PAYLOAD_TYPE FLASH_PAGE_SIZE_IN_BYTES ETX_BL_PAGE_SIZE ETX_APP_PAGE_SIZE RS232_BAUDRATE RS232_MODE_DATA_BITS RS232_MODE_PARITY RS232_MODE_STOPBITS RS232_IS_FLOW_CONTROL SEND_PACKET_BYTES_DELAY TEUNIZ_LIB_POLL_COMPORT_DELAY TRY_AGAIN_SENDING_FWI_DELAY HM10_CONNECT_TO_ADDRESS_TIMEOUT BLUETOOTH_ADDRESS
```

where those Command Line Arguments stand for the following:
- **PATH_TO_THE_COMPILED_FILE**: Path to the compiled file of the etx_ota_protocol_host.c program.
- **COMPORT_NUMBER**: Serial Port number that the user wishes for our host machine to communicate with the external desired device (e.g., an MCU).
- **PAYLOAD_SIZE**: Length in bytes of specifically an ETX OTA Custom Data Payload. Otherwise, give any positive numeric value here.
- **PAYLOAD_PATH_OR_DATA**: Should either contain the desired ETX OTA Custom Data to be sent in case of having that type of ETX OTA Transaction, or should contain the File Path to the Payload file (with respect to the File Location of the executed compiled file of the main.c program) that the user wants this program to load and send towards the desired remote BT device.
- **ETX_OTA_PAYLOAD_TYPE**: ETX OTA Payload Type for the given Payload file via the **PAYLOAD_PATH_OR_DATA** Command Line Argument. For more details on the valid values for the **ETX_OTA_PAYLOAD_TYPE** Command Line Argument, see "ETX_OTA_Payload_t" enum from the "etx_ota_protocol_host.c" file.
- **FLASH_PAGE_SIZE_IN_BYTES**: Size in bytes of each Flash Memory Page of the slave MCU/MPU to communicate with via the ETX OTA Protocol.
- **ETX_BL_PAGE_SIZE**: Number of Flash Memory pages that have been designated for the Bootloader Firmware of the MCU/MPU with which the Serial Port communication is to be established with.
- **ETX_APP_PAGE_SIZE**: Number of Flash Memory pages that have been designated for the Application Firmware of the MCU/MPU with which the Serial Port communication is to be established with.
- **RS232_BAUDRATE**: Baud rate that wants to be used on the UART with which a communication with the slave device will be made.
- **RS232_MODE_DATA_BITS**: Data-bits with which we want the host to run the RS232 protocol.
- **RS232_MODE_PARITY**: Parity with which we want the host to run the RS232 protocol.
- **RS232_MODE_STOPBITS**: Stop-bits with which we want the host to run the RS232 protocol.
- **RS232_IS_FLOW_CONTROL**: Flag value to indicate whether we want the host to run the RS232 protocol with Flow Control with a \c "1" or otherwise with a \c "0" .
- **SEND_PACKET_BYTES_DELAY**: Desired delay in microseconds that is desired to request before having send a byte of data from a certain ETX OTA Packet that is in process of being sent to the MCU.
- **TEUNIZ_LIB_POLL_COMPORT_DELAY**: Desired delay in microseconds that is to be requested to apply each time before calling the RS232_PollComport() function of the <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a>.
- **TRY_AGAIN_SENDING_FWI_DELAY**: Delay in microseconds that it is to be requested to apply in case that starting an ETX OTA Transaction fails once only.
- **HM10_CONNECT_TO_ADDRESS_TIMEOUT**: Time in microseconds that is to be requested for waiting for establishing a Bluetooth Connection between our HM-10 BT Device and the desired BT Device.
- **BLUETOOTH_ADDRESS**: Bluetooth Address, without the colon characters, of the Remote Bluetooth Device with which it is desired to establish a Bluetooth Connection with.

An example would be the following:

```bash
$ ./APIs/blePcToolAPI/ETX_OTA_Protocol_BLE_API 4 0 "../../Application/Debug/Blinky.bin" 0 1024 34 86 9600 8 N 1 0 1000 500000 5000000 11000000 "0017EA090909" 
```

That's it!. ENJOY !!!.
