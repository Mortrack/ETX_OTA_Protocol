/* USER CODE BEGIN Header */
/** @file
 *	@brief	Main file at which the main/actual application of our MCU/MPU lies at.
 *
 * @defgroup main Main module
 * @{
 *
 * @brief	This is the Main Module, where the main part of the application of the whole code lies.
 *
 * @details	The purpose of this module is to provide a template code for any general purpose application that is desired
 *          to be developed for an STM32 MCU/MPU in which such a template consists of an Application Firmware that
 *          already provides the code to manage and handle Firmware Updates via the Mortrack's ETX OTA Protocol which
 *          uses the @ref firmware_update_config .
 *
 * @note    The @ref main template is actually functioning as an Application Firmware that has a Bootloader counterpart
 *          and that should be located in an independent project to this, next to the root folder where the @ref main
 *          project's File Path is located at.
 *
 * @details	In addition, the ETX OTA Protocol also optionally allows to receive Custom Data from the host machine for
 * 			whatever purpose the implementer requires it for.
 * @details In the case of receiving an ETX OTA Custom Data Request from the host machine, then the received data will
 *          be handled via the @ref etx_ota_custom_data struct at the @ref etx_ota_status_resp_handler function in order
 *          to be able to immediately process any new received ETX OTA Custom Data from the host.
 *
 * @note    To learn more details about how to configure and handle ETX OTA Custom Data Requests, see the
 *          @ref init_firmware_update_module and @ref etx_ota_status_resp_handler functions.
 *
 * @details The logic contemplated in this @ref main template assumes that neither Bootloader or Application Firmware
 *          Updates will be applied by this Application Firmware. Instead, whenever receiving those requests, our
 *          MCU/MPU should apply a Software Reset in order to enter into Bootloader mode again to try receiving those
 *          Firmware Update requests there, since it is expected that only the Bootloader installs Firmware Images into
 *          our MCU/MPU.
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	December 13, 2023.
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>	// Library from which "printf" is located at.
#include <stdint.h> // This library contains the aliases: uint8_t, uint16_t, uint32_t, etc.
#include "app_side_etx_ota.h" // This custom Mortrack's library contains the functions, definitions and variables required so that the Main module can receive and apply Firmware Update Images to our MCU/MPU.
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define APLICATION_FIRMWARE_ADDRESS					(ETX_APP_FLASH_ADDR)					/**< @brief Designated Memory Location address for the Application Firmware. */
#define GPIO_MCU_LED1_Pin							(GPIO_PIN_13)							/**< @brief Label for Pin PC13 in Output Mode, which is the Green LED1 of our MCU that the @ref main module will use in its program for indicating to the user whenever our MCU/MPU gets a software error or not. @details The following are the output states to be taken into account:<br><br>* 0 (i.e., Low State and also LED1 turned On) = MCU got a Software Error.<br>* 1 (i.e., High State and also LED1 turned Off) = MCU has no software error. */
#define GPIO_is_hm10_default_settings_Port 			((GPIO_TypeDef *) GPIOC)				/**< @brief @ref GPIO_TypeDef Type of the GPIO Port towards which the Input Mode GPIO Pin PC14 will be used so that our MCU can know whether the user wants it to set the default configuration settings in the HM-10 BT Device or not. */
#define GPIO_is_hm10_default_settings_Pin			(GPIO_PIN_14)							/**< @brief Label for the GPIO Pin 14 towards which the GPIO Pin PC14 in Input Mode is at, which is used so that our MCU can know whether the user wants it to set the default configuration settings in the HM-10 BT Device or not. @details The following are the possible values of this Pin:<br><br>* 0 (i.e., Low State) = Do not reset/change the configuration settings of the HM-10 BT Device.<br>* 1 (i.e., High State) = User requests to reset the configuration settings of the HM-10 BT Device to its default settings. */
#define MAJOR 										(0)										/**< @brief Major version number of our MCU/MPU's Application Firmware. */
#define MINOR 										(4)										/**< @brief Minor version number of our MCU/MPU's Application Firmware. */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
// NOTE: "huart1" is used for debugging messages via printf() from stdio.h library.
// NOTE: "huart2" is used for communicating with the host that will be sending firmware images to our MCU via the ETX OTA Protocol with the UART Hardware Protocol.
// NOTE: "huart3" is used for communicating with the host that will be sending firmware images to our MCU via the ETX OTA Protocol with the BT Hardware Protocol.
const uint8_t APP_version[2] = {MAJOR, MINOR};		/**< @brief Global array variable used to hold the Major and Minor version number of our MCU/MPU's Application Firmware in the 1st and 2nd byte respectively. */
firmware_update_config_data_t fw_config;			        /**< @brief Global struct used to either pass to it the data that we want to write into the designated Flash Memory pages of the @ref firmware_update_config sub-module or, in the case of a read request, where that sub-module will write the latest data contained in the sub-module. */
etx_ota_custom_data_t etx_ota_custom_data;			        /**< @brief	Global struct where it is desired to hold the handling data for any received ETX OTA Custom Data. */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/**@brief	Initializes the @ref firmware_update_config sub-module and then loads the latest data that has been written
 *          into it, if there is any. However, in the case that any of these processes fail, then this function will
 *          endlessly loop via a \c while() function and set the @ref GPIO_MCU_LED1_Pin to @ref GPIO_PIN_RESET state.
 *
 * @details	In case that all the processes conclude successfully, the latest data of the @ref firmware_update_config
 *          sub-module will be copied into the global struct \c fw_config .
 *
 * @details	A maximum of three attempts to initialize this module will be made, with a delay of 0.5 seconds each.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date	November 19, 2023
 */
static void custom_firmware_update_config_init();

/**@brief	Initializes the @ref bl_side_firmware_update with a desired Hardware Protocol and, only in the case that the
 *          initialization is unsuccessful, then this function will endlessly loop via a \c while() function and set the
 *          @ref GPIO_MCU_LED1_Pin to @ref GPIO_PIN_RESET state.
 *
 * @details	This function creates and populate the fields of three structs required for initializing the
 *          @ref bl_side_firmware_update via the @ref init_firmware_update_module function. In the case where that
 *          function initializes the @ref bl_side_firmware_update successfully, then this
 *          @ref custom_firmware_update_config_init function will terminate. Otherwise, it will endlessly loop via a \c
 *          while() function and set the @ref GPIO_MCU_LED1_Pin to @ref GPIO_PIN_RESET state.
 *
 * @note    The @ref fw_config Global struct must have already been populated with the latest data written into the
 *          @ref firmware_update_config before calling this function.
 *
 * @param hw_protocol   ETX OTA Hardware Protocol that is desired to enable whenever using the ETX OTA Protocol.
 * @param[in] p_huart   Pointer to the UART that wants to be used from our MCU/MPU as the means for using the chosen ETX
 *                      OTA Hardware Protocol.
 *
 * @author	César Miranda Meza
 * @date	November 19, 2023
 */
static void custom_init_etx_ota_protocol_module(ETX_OTA_hw_Protocol hw_protocol, UART_HandleTypeDef *p_huart);

/**@brief	Validates the CRC of our MCU/MPU's current Application Firmware and, only in the case that the Application
 *          Firmware is not valid or is corrupted, then this function will endlessly loop via a \c while() function and
 *          set the @ref GPIO_MCU_LED1_Pin to @ref GPIO_PIN_RESET state.
 *
 * @details	This function reads the data stored in the @ref firmware_update_config via the @ref fw_config Global struct
 *          to get the recorded CRC of the Application Firmware and it calculates the CRC of the actual Application
 *          Firmware currently installed in our MCU/MPU. After this, both CRCs are used for validating our MCU/MPU's
 *          current Bootloader Firmware. If both CRCs match, then this function will terminate. Otherwise, it will
 *          endlessly loop via a \c while() function and set the @ref GPIO_MCU_LED1_Pin to @ref GPIO_PIN_RESET state.
 *
 * @note    The @ref fw_config Global struct must have already been populated with the latest data written into the
 *          @ref firmware_update_config before calling this function.
 *
 * @author	César Miranda Meza
 * @date	November 19, 2023
 */
static void validate_application_firmware();

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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

    /* Send a message from the Application showing the current Application version there. */
  	printf("Starting Application v%d.%d\r\n", APP_version[0], APP_version[1]);

    /* We initialize the Firmware Update Configurations sub-module and the ETX OTA Firmware Update module, and also validate the currently installed Application Firmware in our MCU/MPU. */
    // NOTE: These initializations must be made in that order. After those, you may call the initialization functions of your actual application.
    custom_firmware_update_config_init();
    custom_init_etx_ota_protocol_module(ETX_OTA_hw_Protocol_BT, &huart3);
    validate_application_firmware();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	// NOTE: Write your actual application code here.
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
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**@brief	Compiler definition to be able to use the @ref printf function from stdio.h library in order to print
 *          characters via the UART1 Peripheral but by using that @ref printf function.
 *
 * @return	The \p ch param.
 *
 * author	César Miranda Meza
 * @date	July 07, 2023
 */
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar(). */
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
{
	/* Place your implementation of fputc here. */
	/* NOTE: The characters written into the UART1 Protocol will be looped until the end of transmission. */
	HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, HAL_MAX_DELAY);
	return ch;
}

static void custom_firmware_update_config_init()
{
    /** <b>Local variable ret:</b> Return value of a @ref FirmUpdConf_Status function type. */
    int16_t ret;
    /** <b>Local variable attempts:</b> Counter for the number of attempts to initialize the Firmware Update Configurations sub-module. */
    uint8_t attempts = 0;
    /** <b>Local variable end_tick:</b> Defines the HAL Tick that our MCU/MPU needs to reach so that 0.5 seconds have passed with respect to each attempt to initialize the @ref firmware_update_config . */
    uint32_t end_tick;
    /** <b>Local variable current_tick:</b> Current HAL Tick in our MCU/MPU. */
    uint32_t current_tick;

    printf("Initializing the Firmware Update Configurations sub-module...\r\n");
    do
    {
        /* Delay of 500 milliseconds. */
        end_tick = HAL_GetTick() + 500;
        current_tick = 0;
        while (current_tick < end_tick)
        {
            current_tick = HAL_GetTick();
        }

        /* We attempt to initialize the Firmware Update Configurations sub-module. */
        ret = firmware_update_configurations_init();
        attempts++;
        if (ret == FIRM_UPDT_CONF_EC_OK)
        {
            /* We read the latest data that has been written into the Firmware Update Configurations sub-module. */
            firmware_update_configurations_read(&fw_config);
            printf("DONE: Firmware Update Configurations sub-module has been successfully initialized.\r\n");

            return;
        }
        printf("WARNING: The Firmware Update Configurations sub-module could not be initialized at attempt %d...\r\n", attempts);
    }
    while(attempts < 3);

    printf("ERROR: The Firmware Update Configurations sub-module could not be initialized. Our MCU/MPU will halt!.\r\n");
    HAL_GPIO_WritePin(GPIOC, GPIO_MCU_LED1_Pin, GPIO_PIN_RESET);
    while (1);
}

static void custom_init_etx_ota_protocol_module(ETX_OTA_hw_Protocol hw_protocol, UART_HandleTypeDef *p_huart)
{
    /** <b>Local variable ret:</b> Used to hold the exception code value returned by a @ref ETX_OTA_Status function type. */
    ETX_OTA_Status ret;

    printf("Initializing the ETX OTA Firmware Update Module.\r\n");
    /** <b>Local variable GPIO_is_hm10_default_settings:</b> Used to hold the GPIO pin parameters of the Input Mode GPIO Pin to be used so that our MCU can know whether the user wants it to set the default configuration settings in the HM-10 BT Device or not. */
    HM10_GPIO_def_t GPIO_is_hm10_default_settings;
    GPIO_is_hm10_default_settings.GPIO_Port = GPIO_is_hm10_default_settings_Port;
    GPIO_is_hm10_default_settings.GPIO_Pin = GPIO_is_hm10_default_settings_Pin;

    ret = init_firmware_update_module(hw_protocol, p_huart, &fw_config, &GPIO_is_hm10_default_settings, &etx_ota_custom_data);
    if (ret != ETX_OTA_EC_OK)
    {
    	printf("ERROR: The ETX OTA Firmware Update Module could not be initialized. Our MCU/MPU will halt!.\r\n");
        HAL_GPIO_WritePin(GPIOC, GPIO_MCU_LED1_Pin, GPIO_PIN_RESET);
        while (1);
    }
    printf("DONE: The ETX OTA Firmware Update Module has been successfully initialized.\r\n");
}

static void validate_application_firmware()
{
	printf("Validating currently installed Application Firmware in our MCU/MPU...\r\n");
    if ((fw_config.App_fw_size==DATA_BLOCK_32BIT_ERASED_VALUE) || (fw_config.App_fw_size==0x00000000))
	{
    	printf("ERROR: No Application Firmware has been identified to be installed in our MCU/MPU.\r\n");
        HAL_GPIO_WritePin(GPIOC, GPIO_MCU_LED1_Pin, GPIO_PIN_RESET);
        while (1);
	}

    if (fw_config.App_fw_rec_crc == DATA_BLOCK_32BIT_ERASED_VALUE)
    {
    	printf("ERROR: The recorded 32-bit CRC of the installed Application Firmware has no value in it.\r\n");
        HAL_GPIO_WritePin(GPIOC, GPIO_MCU_LED1_Pin, GPIO_PIN_RESET);
        while (1);
    }

    /** <b>Local variable cal_crc:</b> Value holder for the calculated 32-bit CRC of our MCU/MPU's current Application Firmware. */
	uint32_t cal_crc = crc32_mpeg2((uint8_t *) APLICATION_FIRMWARE_ADDRESS, fw_config.App_fw_size);

    if (cal_crc != fw_config.App_fw_rec_crc)
    {
    	printf("ERROR: The recorded 32-bit CRC of the installed Application Firmware Image mismatches with the calculated one: [Calculated CRC = 0x%08X] [Recorded CRC = 0x%08X]\r\n",
    			(unsigned int) cal_crc, (unsigned int) fw_config.App_fw_rec_crc);
        HAL_GPIO_WritePin(GPIOC, GPIO_MCU_LED1_Pin, GPIO_PIN_RESET);
        while (1);
    }
    printf("DONE: The currently installed Application Firmware in our MCU/MPU has been successfully validated.\r\n");
}

void etx_ota_pre_transaction_handler()
{
	// NOTE: The code contained is this function is what you should substitute with whatever you wish to do or to stop doing before an ETX OTA Transaction gives place.
	printf("An ETX OTA Transaction is about to give place.\r\n");
	printf("Finishing or stopping a certain task before proceeding with the ETX OTA Transaction...\r\n");
}

void etx_ota_status_resp_handler(ETX_OTA_Status resp)
{
    switch (resp)
    {
        case ETX_OTA_EC_OK:
        	// NOTE: The whole code of this case is what you should substitute with whatever you wish to do with the received ETX OTA Custom Data.
        	printf("DONE: An ETX OTA Transaction has been successfully completed.\r\n");
        	printf("Showing the ETX OTA Custom Data that was received: [\r\n");
        	for (int i=0; i<etx_ota_custom_data.size-1; i++)
        	{
        		printf("%d, ", etx_ota_custom_data.data[i]);
        	}
        	printf("%d]\r\n", etx_ota_custom_data.data[etx_ota_custom_data.size-1]);
        	// NOTE: The following if-code that stops ETX OTA Transactions is just to display an example on a possible way to use/call the @ref stop_etx_ota function, but must not be taken as something that must always be made whenever using this ETX OTA Protocol library.
        	if (etx_ota_custom_data.data[0] == 0xFF)
        	{
        		stop_etx_ota();
        	}
	    	break;
        case ETX_OTA_EC_STOP:
        	printf("DONE: ETX OTA process has been aborted. Try again...\r\n");
			start_etx_ota();
	    	break;
        case ETX_OTA_EC_NR:
            // No response was received from host. Therefore, try hearing for a response from the host again in case our MCU/MPU is still in DFU mode.
            break;
        case ETX_OTA_EC_NA:
        	printf("WARNING: A Firmware Image Update has been request.\r\n");
        	printf("Resetting our MCU/MPU to jump into its Bootloader Firmware to receive the desired Firmware Image there and then try again...\r\n");
			HAL_NVIC_SystemReset(); // We reset our MCU/MPU to try installing a Firmware Image there.
	    	break;
        case ETX_OTA_EC_ERR:
        	printf("ERROR: ETX OTA process has failed. Try again...\r\n");
        	start_etx_ota();
	    	break;
        default:
        	/* This should never be called. */
        	printf("ERROR: Exception Code received %d is not recognized. Our MCU/MPU will halt!.\r\n", resp);
			HAL_GPIO_WritePin(GPIOC, GPIO_MCU_LED1_Pin, GPIO_PIN_RESET);
	    	while (1);
    }
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