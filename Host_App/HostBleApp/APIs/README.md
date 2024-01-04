# /'APIs': The APIs with which the Java Host App interacts with

This folder contains all the APIs with which the Java Host App interacts with, which were all coded in C programming
language.

# How to explore the project files.
The following will describe the general purpose of the folders that are located in the current directory address:

- **/'blePcToolAPI'**:
  - This folder contains the C programming language API responsible for interacting with a desired Remote Device via Bluetooth (with the HM-10 Bluetooth Device) by using the ETX OTA Protocol.
- **/'dongleConfAPI'**:
  - This folder contains the C programming language API that is used for configuring a desired HM-10 Bluetooth Device in Central Mode so that our Host Computer can later use it to communicate with a desired Remote Bluetooth Device.
- **/'uartPcToolAPI'**:
  - This folder contains the C programming language API responsible for interacting with a desired Remote Device via RS-232 by using the ETX OTA Protocol.
