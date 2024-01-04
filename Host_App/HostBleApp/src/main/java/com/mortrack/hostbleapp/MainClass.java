/**@file
 * 
 * @defgroup Main_Class Main Class module
 * @{
 * 
 * @brief   This module contains the main Class of the entire project, which is
 *          the one responsible for generating a UI for the purpose of sending
 *          Payload Data to a certain MCU/MPU via a selected Hardware Protocol
 *          (such as UART or BLE) and by using the ETX OTA Communication
 *          Protocol.
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    October 31, 2023.
 */


/* @brief   Package containing all the classes, methods, variables and code in
 *          general to build a UI for the purpose of sending Payload data to a
 *          certain MCU/MPU via a selected Hardware Protocol (such as UART or
 *          BLE) and by using the ETX OTA Comunication Protocol.
 *
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    October 31, 2023.
 */
package com.mortrack.hostbleapp;

import com.mortrack.hostbleapp.jframes.MainFrame;

/**@brief   Main Class of the entire project, which contains the main method of
 *          the project.
 * 
 * @details The main method of this Class generates a UI for the purpose of
 *          sending Payload Data to a certain MCU/MPU via a selected Hardware
 *          Protocol (such as UART or BLE) and by using the ETX OTA
 *          Communication Protocol.
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    October 31, 2023.
 */
public class MainClass {    
    /**@brief   Main method of the project that generates and makes visible a UI
     *          for the purpose of  sending Payload Data to a certain MCU/MPU
     *          via a selected Hardware Protocol (such as UART or BLE) and by
     *          using the ETX OTA Communication Protocol.
     * 
     * @param[in] args  Array into which the given Command Line Arguments
     *                  (whenever executing the program) are populated in.
     *
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    October 31, 2023.
     */
    public static void main(String args[]) {        
        new MainFrame().setVisible(true);
    }
}

/** @} */
