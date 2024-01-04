# Compilation and execution instructions of the main.c program
Follow the steps and explanations given in the following content to be able to successfully compile and execute the
main.c program, whose actual purpose is to be used as an API so that other programs in a different programming language
other than C can just call the main.c program to take care of configuring a desired HM-10 BT Device to Central Mode.

## Configuring the program with custom user settings
While on the API version of the Dongle Configurator (i.e., the main.c program provided in this project) requires most of
the default settings for the Dongle Configurator to be given whenever calling the API provided via the main.c program,
there are a few other default settings in this project that are as defined in the "dongle_configurator_config.h" file.
If you were to require different settings than those ones, it is suggested to change the following define from that
file:

```c
#define ENABLE_APP_DONGLE_CONFIGURATOR_CONFIG   (0)           /**< @brief Flag used to enable the use of @ref app_dongle_configurator_config with a 1 or, otherwise to disable it with a 0. */
```

such that the value of that definition is changed to 1 as shown in the following code illustration:

```c
#define ENABLE_APP_DONGLE_CONFIGURATOR_CONFIG   (1)           /**< @brief Flag used to enable the use of @ref app_dongle_configurator_config with a 1 or, otherwise to disable it with a 0. */
```

This will allow you to enable the use of the "appdongle_configurator_config.h" file, where you can call whatever
definition contained inside the "dongle_configurator_config.h" but into "app_dongle_configurator_config.h" with whatever
custom value you desire. This will allow you to set custom values for the program settings in a separate custom file but
without rewriting the default values.

Now that this has been explained, proceed to set the desired configuration settings for this program.

## Steps to compile the program
To compile this program, run the below command to compile the application.

```bash
$ gcc main.c dongle_configurator.c HM10_ble_driver/Src/hm10_ble_driver.c RS232/rs232.c -IRS232 -Wall -Wextra -o2 -o Dongle_Configurator_API
```

**NOTE:** To be able to compile this program, make sure you have at GCC version >= 11.4.0

## Steps to execute the program
Once you have built the application, then execute it by using the following below syntax as a reference:

```bash
$ ./PATH_TO_THE_COMPILED_FILE COMPORT_NUMBER RS232_BAUDRATE RS232_MODE_DATA_BITS RS232_MODE_PARITY RS232_MODE_STOPBITS RS232_IS_FLOW_CONTROL SEND_PACKET_BYTES_DELAY TEUNIZ_LIB_POLL_COMPORT_DELAY
```

where those Command Line Arguments stand for the following:
- **PATH_TO_THE_COMPILED_FILE**: Path to the compiled file of the etx_ota_protocol_host.c program.
- **COMPORT_NUMBER**: Serial Port number that the user wishes for our host machine to communicate with the HM-10 BT Device.
- **RS232_BAUDRATE**: Baud rate that wants to be used on the RS-232 with which a communication with the HM-10 BT Device will be made.
- **RS232_MODE_DATA_BITS**: Data-bits with which we want the host to run the RS232 protocol.
- **RS232_MODE_PARITY**: Parity with which we want the host to run the RS232 protocol.
- **RS232_MODE_STOPBITS**: Stop-bits with which we want the host to run the RS232 protocol.
- **RS232_IS_FLOW_CONTROL**: Flag value to indicate whether we want the host to run the RS232 protocol with Flow Control with a \c "1" or otherwise with a \c "0" .
- **SEND_PACKET_BYTES_DELAY**: Desired delay in microseconds that is desired to request before having send a byte of data to the HM-10 BT Device via the Serial Port.
- **TEUNIZ_LIB_POLL_COMPORT_DELAY**: Desired delay in microseconds that is to be requested to apply each time before calling the RS232_PollComport() function of the <a href=https://gitlab.com/Teuniz/RS-232>Teuniz RS232 Library</a>.

An example would be the following:

```bash
$ ./APIs/dongleConfAPI/Dongle_Configurator_API 4 9600 8 N 1 0 1000 500000 
```

That's it!. ENJOY !!!.
