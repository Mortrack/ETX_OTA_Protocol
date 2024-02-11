/* USER CODE BEGIN Header */
/** @file
 *	@brief	Main file at which the main/actual application of our MCU/MPU lies at.
 *
 * @defgroup main Main module
 * @{
 *
 * @brief	This is the Main Module, which is where the main part of the application of the whole code is executed.
 *
 * @details	The purpose of this module is to provide a Pre-Bootloader software that servers the purpose of working as a
 *          handler that decides whether our MCU/MPU will run the Firmware that it has installed in
 *          @ref BOOTLOADER_FIRMWARE_ADDRESS or, if it should attempt to install a Bootloader Firmware Image instead, in
 *          the case that such a Firmware Image has been detected to be temporarily stored at the Flash Memory location
 *          address @ref APLICATION_FIRMWARE_ADDRESS . If this is the case, then our MCU/MPU will install that
 *          Bootloader Firmware Image into the designated Flash Memory for the Bootloader (i.e.,
 *          @ref BOOTLOADER_FIRMWARE_ADDRESS ) and will then apply a software reset via @ref HAL_NVIC_SystemReset .
 *
 * @note    The logic contemplated in this @ref main program expects the implementer to never update its code/firmware
 *          after lunching whatever application our MCU/MPU is used for to the production stage. If there are changes
 *          that are required to be made, those should be made directly into either the Bootloader or the Application
 *          Firmware counterparts.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	November 18, 2023.
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>	// Library from which "printf" is located at.
#include <stdint.h> // This library contains the aliases: uint8_t, uint16_t, uint32_t, etc.
#include "pre_bl_side_etx_ota.h" // This custom Mortrack's library contains the functions, definitions and variables required so that the Main module can install Bootloader Firmware Update Images into our MCU/MPU.
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BOOTLOADER_FIRMWARE_ADDRESS					(ETX_BL_FLASH_ADDR)						/**< @brief Designated Memory Location address for the Bootloader Firmware. */
#define BOOTLOADER_FIRMWARE_RESET_HANDLER_ADDRESS 	(BOOTLOADER_FIRMWARE_ADDRESS + 4U)		/**< @brief Designated Memory Location address for the Reset Handler of the Bootloader Firmware. */
#define APLICATION_FIRMWARE_ADDRESS					(ETX_APP_FLASH_ADDR)					/**< @brief Designated Memory Location address for the Application Firmware. */
#define APPLICATION_FIRMWARE_RESET_HANDLER_ADDRESS 	(APLICATION_FIRMWARE_ADDRESS + 4U)		/**< @brief Designated Memory Location address for the Reset Handler of the Application Firmware. */
#define MAJOR 										(0)										/**< @brief Major version number of our MCU/MPU's Pre-Bootloader Firmware. */
#define MINOR 										(4)										/**< @brief Minor version number of our MCU/MPU's Pre-Bootloader Firmware. */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const uint8_t pre_BL_version[2] = {MAJOR, MINOR};   	/**< @brief Global array variable used to hold the Major and Minor version number of our MCU/MPU's Pre-Bootloader Firmware in the 1st and 2nd byte respectively. */
firmware_update_config_data_t fw_config;				/**< @brief Global struct used to either pass to it the data that we want to write into the designated Flash Memory pages of the @ref firmware_update_config sub-module or, in the case of a read request, where that sub-module will write the latest data contained in the sub-module. */

/**@brief	Pre-Bootloader Processes Exception codes.
 *
 * @details	These Exception Codes are returned by the functions of the @ref main module to indicate the resulting status
 *          of having executed the process contained in each of those functions. For example, to indicate that the
 *          process executed by a certain function was successful or that it has failed.
 */
typedef enum
{
	PRE_BL_EC_OK       = 0U,    //!< PRE Bootloader process was successful. @note The code from the @ref HAL_ret_handler function contemplates that this value will match the one given for \c HAL_OK from @ref HAL_StatusTypeDef .
	PRE_BL_EC_ERR      = 4U     //!< PRE Bootloader process has failed.
} PRE_BL_Status;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/**@brief	Initializes the @ref firmware_update_config sub-module and then loads the latest data that has been written
 *          into it, if there is any.
 *
 * @details	In case that all the processes conclude successfully, the latest data of the @ref firmware_update_config
 *          sub-module will be copied into the global struct \c fw_config .
 *
 * @details	A maximum of three attempts to initialize this module will be made, with a delay of 0.5 seconds each.
 *
 * @retval  FIRM_UPDT_CONF_EC_OK
 * @retval	FIRM_UPDT_CONF_EC_ERR
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	November 15, 2023
 */
static FirmUpdConf_Status custom_firmware_update_config_init();

/**@brief	Validates the CRC of our MCU/MPU's current Application Firmware.
 *
 * @details	This function reads the data stored in the @ref firmware_update_config sub-module to get the recorded CRC of
 *          the Application Firmware and it calculates the CRC of the actual Application Firmware currently installed in
 *          our MCU/MPU. After this, both CRCs are used for validating our MCU/MPU's current Application Firmware.
 *
 * @retval	ETX_OTA_EC_OK	if both the Calculated and Recorded CRCs of our MCU/MPU's Application Firmware match.
 * @retval	ETX_OTA_EC_ERR	otherwise.
 *
 * @author	César Miranda Meza
 * @date	November 15, 2023
 */
static PRE_BL_Status validate_application_firmware();

/**@brief	Makes our MCU/MPU to jump into its Bootloader Firmware.
 *
 * @author	César Miranda Meza
 * @date	November 15, 2023
 */
static void goto_bootloader_firmware(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /** <b>Local variable ret:</b> Used to hold the exception code value returned by either a @ref FirmUpdConf_Status , a @ref ETX_OTA_Status or a @ref PRE_BL_Status function type. */
  uint8_t ret;

  /* We initialize the Firmware Update Configurations sub-module to be able to write and read data from it. */
  ret = custom_firmware_update_config_init();
  if (ret != FIRM_UPDT_CONF_EC_OK)
  {
	  while (1);
  }

  /* Validate if there is a Bootloader Firmware Image pending to be installed and, if true, install it. Otherwise, continue with the program. */
  switch (fw_config.is_bl_fw_install_pending)
  {
  	  case DATA_BLOCK_8BIT_ERASED_VALUE:
	  case NOT_PENDING:
		  goto_bootloader_firmware();
	  case IS_PENDING:
		  if ((validate_application_firmware()!=PRE_BL_EC_OK) || (fw_config.is_bl_fw_stored_in_app_fw!=BT_FW_STORED_IN_APP_FW))
		  {
			  goto_bootloader_firmware();
		  }
		  ret = install_bl_stored_in_app_fw(&fw_config);
		  HAL_NVIC_SystemReset();
	  default:
		  while (1);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

static FirmUpdConf_Status custom_firmware_update_config_init()
{
	/** <b>Local variable ret:</b> Return value of a @ref FirmUpdConf_Status function type. */
	int16_t ret;
	/** <b>Local static variable attempts:</b> Counter for the number of attempts to initialize the Firmware Update Configurations sub-module. */
	static uint8_t attempts = 0;
	/** <b>Local variable end_tick:</b> Defines the HAL Tick that our MCU/MPU needs to reach so that 0.5 seconds have passed with respect to the moment that this function is called. */
	uint32_t end_tick = HAL_GetTick() + 500;
	/** <b>Local variable current_tick:</b> Current HAL Tick in our MCU/MPU. */
	uint32_t current_tick = 0;

	if (attempts > 2)
	{
		return FIRM_UPDT_CONF_EC_ERR;
	}

	/* Delay of 500 milliseconds. */
	while (current_tick < end_tick)
	{
		current_tick = HAL_GetTick();
	}

	/* We initialize the Firmware Update Configurations sub-module. */
	ret = firmware_update_configurations_init();
    attempts++;
	if (ret != FIRM_UPDT_CONF_EC_OK)
	{
		ret = custom_firmware_update_config_init();
        return ret;
	}

	/* We read the latest data that has been written into the Firmware Update Configurations sub-module. */
	firmware_update_configurations_read(&fw_config);

	return FIRM_UPDT_CONF_EC_OK;
}

static PRE_BL_Status validate_application_firmware()
{
	/* Validating the Application Firmware of our MCU/MPU. */
    if ((fw_config.App_fw_size==DATA_BLOCK_32BIT_ERASED_VALUE) || (fw_config.App_fw_size==0x00000000))
	{
		return PRE_BL_EC_ERR;
	}

    if (fw_config.App_fw_rec_crc == DATA_BLOCK_32BIT_ERASED_VALUE)
    {
        return PRE_BL_EC_ERR;
    }

    /** <b>Local variable cal_crc:</b> Value holder for the calculated 32-bit CRC of our MCU/MPU's current Application Firmware. */
	uint32_t cal_crc = crc32_mpeg2((uint8_t *) APLICATION_FIRMWARE_ADDRESS, fw_config.App_fw_size);
    if (cal_crc != fw_config.App_fw_rec_crc)
    {
        return PRE_BL_EC_ERR;
    }

    return PRE_BL_EC_OK;
}

static void goto_bootloader_firmware(void)
{
	/* Create function pointer with no arguments that points to the Memory Location Address of the Reset Handler of the Bootloader Firmware. */
	void (*bl_reset_handler) (void) = (void*) (*(volatile uint32_t *) (BOOTLOADER_FIRMWARE_RESET_HANDLER_ADDRESS));

	/* NOTE: Some MCUs might have already the ASM code available so that the Main Stack Pointer (MSP) is recycled, but this is not the case for all MCUs. */
	/* Therefore, if you were to need to do this from scratch, you would have to do the following: */
	//__set_MSP( ( *(volatile uint32_t *) BOOTLOADER_FIRMWARE_ADDRESS );

	/* Call the Bootloader's Reset Handler. */
	bl_reset_handler();
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/** @} */
