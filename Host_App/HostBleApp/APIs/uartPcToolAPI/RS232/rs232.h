/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2005 - 2021 Teunis van Beelen
*
* Email: teuniz@protonmail.com
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*/

/* For more info and how to use this library, visit: http://www.teuniz.net/RS-232/ */

/**@file
 *
 * @defgroup teuniz_rs232_library Teuniz RS232 Library
 * @{
 *
 * @brief   This is the <a href=https://www.teuniz.net/RS-232/>Teuniz RS232 Library</a> where you can find code to use
 *          the serial port.
 *
 * @details It has been tested with GCC on Linux and <a href=https://www.mingw-w64.org/>Mingw-w64</a> on Windows.
 * @details It uses polling to receive characters from the serial port.
 * @details Interrupt/event-based is not supported.
 * @details It is licensed under the <a href=https://www.gnu.org/licenses/gpl-3.0.txt>GPL version 3</a>.
 *
 * @note    No serial port available on your computer? Use a <a href=https://www.digikey.com/products/en?keywords=768-1074-ND>USB to RS232 cable</a>
 * @note    On the other hand, for more details on setting the right value for the comport, baudrate and mode
 *          configuration parameters of this Library, see the Table of the following image, which illustrates the
 *          possible valid values for each of them:<br>
 *          <img src="../img/List_of_comport_numbers_and_baudrates_and_modes.png">
 *
 * @author 	Teuniz (<a href=https://gitlab.com/Teuniz/RS-232>See GitLab code here.</a>)
 *
 * @note    Cesar Miranda Meza (cmirandameza3@hotmail.com) added Doxygen comments to the code of the @ref teuniz_rs232_library based on the information
 *          that the actual code's author has published in his <a href=https://www.teuniz.net/RS-232/>website</a>.
 *
 * @date	August 05, 2017 (Last Update at January 14, 2023).
 */

#ifndef rs232_INCLUDED
#define rs232_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>



#if defined(__linux__) || defined(__FreeBSD__)

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/file.h>
#include <errno.h>

#else

#include <windows.h>

#endif

/**@brief   Opens a Serial Port using the RS232 protocol on a desired comport and with a desired configuration.
 *
 * @param comport_number    Converted Comport number, whose valid range is any number between 0 up to 37 (See the Table
 *                          from @ref teuniz_rs232_library for some example values).
 * @param baudrate          Baudrate value in baud per second (See the Table from @ref teuniz_rs232_library for some
 *                          example values).
 * @param[in] mode          String value from which the first ASCII character defines the desired Databits, the next
 *                          character stands for the parity and the third character defines the desired stopbit (See the
 *                          Table from @ref teuniz_rs232_library for some example values).
 * @param flowctrl          Flag used to define whether it is desired to use Flow Control with a value of \c 1 , or to
 *                          not use it with a value of \c 0 .
 *
 * @retval                  0 If RS232 comport was opened successfully.
 * @retval                  1 Otherwise.
 *
 * @author 	Teuniz (<a href=https://gitlab.com/Teuniz/RS-232>See GitLab code here.</a>)
 */
int RS232_OpenComport(int comport_number, int baudrate, const char *mode, int flowctrl);

/**@brief   Gets the bytes received from the Serial Port via the RS232 protocol form the @ref teuniz_rs232_library , if
 *          there is any.
 *
 * @details This function will immediately either receive available data from the Serial Port or will determine that
 *          there is no data to be received (i.e., It will not wait). Because of this, it is suggested to place a delay
 *          or timer with an approximate interval of 100 milliseconds.
 *
 * @param comport_number    The converted value of the actual comport that was requested by the user but into its
 *                          equivalent for the @ref teuniz_rs232_library (For more details, see the Table from
 *                          @ref teuniz_rs232_library ).
 * @param[out] buf          Pointer towards an appointed buffer where the received data from the Serial Port will be
 *                          stored.
 * @param size              Length in bytes of the given buffer, towards which the \p buf param points to.
 *
 * @return                  The length in bytes of the data that was received from the Serial Port.
 *
 * @note 0 \f$\leq\f$ valid possibly returned values \f$ \leq \f$ \p size param.
 */
int RS232_PollComport(int comport_number, unsigned char *buf, int size);

/**@brief   Sends a byte of data over the Serial Port using the RS232 protocol from the @ref teuniz_rs232_library .
 *
 * @param comport_number    The converted value of the actual comport that was requested by the user but into its
 *                          equivalent for the @ref teuniz_rs232_library (For more details, see the Table from
 *                          @ref teuniz_rs232_library ).
 * @param byte              The byte that is desired to sent to the other end of the Serial Port.
 *
 * @retval                  0 If the desired byte was successfully send over the Serial Port.
 * @retval                  1 Otherwise.
 */
int RS232_SendByte(int comport_number, unsigned char byte);

/**@brief   Sends multiple bytes of data over the Serial Port using the RS232 protocol from the
 *          @ref teuniz_rs232_library .
 *
 * @note    This function sends data in blocking-mode (i.e., this function concludes running after all the bytes to be
 *          sent have been processed).
 *
 * @param comport_number    The converted value of the actual comport that was requested by the user but into its
 *                          equivalent for the @ref teuniz_rs232_library (For more details, see the Table from
 *                          @ref teuniz_rs232_library ).
 * @param[out] buf          Pointer towards an appointed buffer where the data to be sent over the Serial Port is stored
 *                          at.
 * @param size              Length in bytes of the given buffer, towards which the \p buf param points to.
 *
 * @return  The number of bytes that have been sent in case that this function concludes its processing. Otherwise,
 *          \c -1 in case of an error.
 */
int RS232_SendBuf(int comport_number, unsigned char *buf, int size);

/**@brief   Closes the Serial Port using the RS232 protocol from the @ref teuniz_rs232_library .
 *
 * @param comport_number    The converted value of the actual comport that was requested by the user but into its
 *                          equivalent for the @ref teuniz_rs232_library (For more details, see the Table from
 *                          @ref teuniz_rs232_library ).
 */
void RS232_CloseComport(int comport_number);
void RS232_cputs(int, const char *);
int RS232_IsDCDEnabled(int);
int RS232_IsRINGEnabled(int);
int RS232_IsCTSEnabled(int);
int RS232_IsDSREnabled(int);
void RS232_enableDTR(int);
void RS232_disableDTR(int);
void RS232_enableRTS(int);
void RS232_disableRTS(int);
void RS232_enableBREAK(int);
void RS232_disableBREAK(int);

/**@brief   Flushes data received but not read.
 *
 * @param comport_number    The converted value of the actual comport that was requested by the user but into its
 *                          equivalent for the @ref teuniz_rs232_library (For more details, see the Table from
 *                          @ref teuniz_rs232_library ).
 */
void RS232_flushRX(int comport_number);
void RS232_flushTX(int);
void RS232_flushRXTX(int);
int RS232_GetPortnr(const char *);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

/** @} */
