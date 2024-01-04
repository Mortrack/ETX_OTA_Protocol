/** @addtogroup dongle_configurator
 * @{
 */

/**@file
 *
 * @defgroup dongle_configurator_config Dongle Configurator Configuration File.
 * @{
 *
 * @brief   This file contains all the Default Dongle Configurator Configurations.
 *
 * @note    It is highly suggested not to directly edit the Configurations Settings defined in this file. Instead of
 *          doing that whenever requiring different Configuration Settings, it is suggested to do that instead in an
 *          additional header file named as "app_dongle_configurator_config.h" whose File Path should be located as a
 *          sibbling of this @ref dongle_configurator_config header file. However, to enable the use of that additional
 *          header file, you will also need to change the @ref ENABLE_APP_DONGLE_CONFIGURATOR_CONFIG define, that is
 *          located at @ref dongle_configurator_config, to a value of 1.
 * @note    For more details on how to use this @ref dongle_configurator_config , see the Readme.md file of the project
 *          on which the Dongle Configurator library is being used at.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 29, 2023.
 */

#ifndef DONGLE_CONFIGURATOR_CONFIG_H_
#define DONGLE_CONFIGURATOR_CONFIG_H_

#define ENABLE_APP_DONGLE_CONFIGURATOR_CONFIG   (0)           /**< @brief Flag used to enable the use of @ref app_dongle_configurator_config with a 1 or, otherwise to disable it with a 0. */
#if ENABLE_APP_DONGLE_CONFIGURATOR_CONFIG
#include "app_dongle_configurator_config.h" // This is the user custom Dongle Configurator Configuration File that is recommended to use whenever it is desired to edit default configuration settings as defined in this @ref dongle_configurator_config file.
#endif

#ifndef HM10_RESET_AND_RENEW_CMDS_DELAY
#define HM10_RESET_AND_RENEW_CMDS_DELAY         (1000000U)    /**< @brief Designated time in microseconds for the Delay to be requested each time after either the Reset or Renew Command is solicited to the HM-10 BT Device. @details In order to guarantee that any other AT Command will work as expected after Resetting the HM-10 BT Device, a Delay is needed in order to wait for the Device to complete the Reset Process. This is particularly necessary if a Bluetooth Connection is requested to the HM-10 BT Device after applying a Reset to itself. @note On a validation test made with only one HM-10 BT Device, a Delay of 500 milliseconds worked fine, but repeating that test with more units in the future would help to learn the right value for this Definition. Therefore, a higher value than the one mentioned is suggested in order to guarantee that the HM-10 BT Device will work properly. */
#endif

#endif /* DONGLE_CONFIGURATOR_CONFIG_H_ */

/** @} */

/** @} */
