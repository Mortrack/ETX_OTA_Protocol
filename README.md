# ETX OTA Protocol

This repository provides a very well documented template code for a 1) Pre-Bootloader, 2) Bootloader, and 3) Application
Firmware, where a particular Protocol that Cesar Miranda Meza (alias Mortrack) has just introduced in this Repository is
being applied in all these three template codes, and where that Protocol has been coined under the name of ETX OTA
Protocol (name given due to being inspired by
<a href=https://github.com/Embetronicx/STM32-Bootloader/tree/ETX_Bootloader_3.0>the Educational Firmware Update Project made by Embetronicx</a>
). Therefore, in such a way, this project can be considered a continuation of the educational work provided by
Embetronix. However, this ETX OTA Protocol made by Mortrack was made so that it could be used as a template for
professional/industrial applications, where the main highlights of the ETX OTA Protocol are the following:

- The ETX OTA Protocol is a Simplex Communication Protocol that prioritizes guaranteeing the integrity of critically important data being sent from a host machine to a remote device. However, in exchange it is not the fastest Protocol in transmitting data.
- The ETX OTA Protocol Provides functionalities that allows engineers to apply Firmware Updates in an STMicroelectronics Microcontroller or Microprocessor.
- The ETX OTA Protocol has features that enables engineers to also send Custom Data from the host machine to the remote device if desired.

In addition to these three previously mentioned Templates, this repository also provides some simple Apps to serve as
code examples/references for any Computer acting as Host in this ETX OTA Protocol: 1) A Java App with a friendly
User-Interface, and 2) A C Executable program runnable from a Terminal Window, where both of these programs have the
purpose of sending ETX OTA Firmware Update or ETX OTA Custom Data Requests. However, note that due to time constraints,
the Java App can do this via either the RS-232 Protocol or a HM-10 Bluetooth Device (via the
<a href=https://github.com/Mortrack/hm10_ble_driver>HM-10 Library made by Mortrack</a>, while the C Executable program
can only do that via the RS-232 Protocol.

<i style="color:orange"><b style="background-color:yellow"><u>TODO:</u></b> Pending to add information with diagrams and images that explain how the technical part of the ETX OTA Protocol
works.</i>

# How to explore the project files.
The following will describe the general purpose of the folders that are located in the current directory address:

- **/'Application_firmware_v0.4'**:
    - This folder contains a template project of an Application Code Firmware, that uses the ETX OTA Protocol made by Mortrack, for an STM32F103C8T6 Microcontroller, but that can be easily modified for using it on any other Microcontroller or Microprocessor of the STMicroelectronics Family.
- **/Custom_Bootloader_v0.4**:
    - This folder contains a template project of a Bootloader Code Firmware, that uses the ETX OTA Protocol made by Mortrack, for an STM32F103C8T6 Microcontroller, but that can be easily modified for using it on any other Microcontroller or Microprocessor of the STMicroelectronics Family.
- **/EXTRA**:
    - This folder contains any complementary file that can help the users of this Repository to better understand this project.
- **/Host_App**:
    - This folder contains a simple Java Application with a friendly User-Interface from which it is possible to send either ETX OTA Bootloader or ETX OTA Application Firmware Update requests or even ETX OTA Custom Data requests from our Computer, acting as the Host, to a desired remote device that can understand and communicate with the ETX OTA Protocol. 
- **/PcTool_App**:
    - This folder contains a simple C executable program from which it is possible to send, via a Terminal Window, either ETX OTA Bootloader or ETX OTA Application Firmware Update requests or even ETX OTA Custom Data requests from our Computer, acting as the Host, to a desired remote device that can understand and communicate with the ETX OTA Protocol.
- **/Pre_Bootloader_v0.4**:
    - This folder contains a template project of a Pre-Bootloader Code Firmware, that uses the ETX OTA Protocol made by Mortrack, for an STM32F103C8T6 Microcontroller, but that can be easily modified for using it on any other Microcontroller or Microprocessor of the STMicroelectronics Family.

## Future additions planned for this library

Since the purpose of this project is for educational purposes for learning in detail how do Bootloader and Application
Firmware Updates work, I will limit myself to only improve the documentation and to maybe add some didactical content
that could help to better explain how these concepts work in both conceptual and technical aspects.

However, if the ETX OTA Protocol that I developed is useful for other people, then I plan to continue adding features to
that Protocol such as giving to it the ability of being a Half-Duplex Communication Protocol, instead of being a Simplex
type as it currently is. In addition, I would also invest some effort in making the code of the current ETX OTA Protocol
faster in terms of executing time.
