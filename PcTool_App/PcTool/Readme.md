# Compilation and execution instructions of the etx_ota_protocol_host.c program
Follow the steps and explanations given in the following content to be able to successfully compile and execute the
etx_ota_protocol_host.c program

## Configuring the program with custom user settings
The default settings for the ETX OTA Protocol in this program are as defined in the "etx_ota_config.h" file.
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
To make the compilation of this program, run the below command to compile the application.

```bash
$ gcc main.c etx_ota_protocol_host.c RS232/rs232.c -IRS232 -Wall -Wextra -o2 -o etx_ota_app
```

**NOTE:** To be able to compile this program, make sure you have at GCC version >= 11.4.0

## Steps to execute the program
Once you have built the application, then execute it by using the following below syntax as a reference:

```bash
$ ./PATH_TO_THE_COMPILED_FILE COMPORT_NUM PAYLOAD_PATH ETX_OTA_Payload_t
```

where those Command Line Arguments stand for the following:
- **PATH_TO_THE_COMPILED_FILE**: Path to the compiled file of the etx_ota_protocol_host.c program.
- **COMPORT_NUM**: Serial Port number that the user wishes for our host machine to communicate with the external desired device (e.g., an MCU).
- **PAYLOAD_PATH**: Path to the Payload file (i.e., the Firmware Update Image) that user wants our host machine to read to then pass its data to the external desired device (e.g., an MCU).
- **ETX_OTA_Payload_t**: ETX OTA Payload Type for the given Payload file via the **PAYLOAD_PATH** Command Line Argument. For more details on the valid values for the **ETX_OTA_Payload_t** Command Line Argument, see "ETX_OTA_Payload_t" enum from the "etx_ota_protocol_host.c" file.

An example would be the following:

```bash
$ ./etx_ota_app.exe 8 ../../Application/Debug/Blinky.bin 0
```

That's it!. ENJOY !!!.
