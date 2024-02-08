/** @addtogroup firmware_update_config
 * @{
 */
// TODO: Improve the auto-restoring capacity of this sub-module by using the @ref restore_firm_updt_config_flash_memory function in the @ref firmware_update_configurations_write function to restore the designated Flash Memory Pages in case that the @ref firmware_update_configurations_write function cannnot write data into the Flash Memory.
// TODO: Improve write function to make it not write into flash in case that the input data is already written as it is in the flash.
#include "firmware_update_config.h"
#include <stdio.h>	// Library from which "printf" is located at.
#include <string.h>	// Library from which "memset()" and "memcpy()" are located at.
#include "main.h" // The CRC32 of our MCU/MPU is being called at the "main" file.

#define FIRMWARE_UPDATE_CONFIG_PAGE_1_START_ADDR	(FIRMWARE_UPDATE_CONFIG_START_PAGE * FW_UPDT_CONFIG_PAGE_SIZE/2 + FLASH_START_ADDR)	/**< @brief Designated Flash Memory address for the start of the Firmware Update Configurations page 1. @details The start of Firmware Update Configurations page 1 should be 0x0801'E000. This is because the designated Flash Memory page for the Firmware Update Configurations page 1 is 120 (see @ref FIRMWARE_UPDATE_CONFIG_START_PAGE ) @details Also, the length of the Firmware Update Configurations page 1 is 2048 bytes @note Although each Flash Memory Page in the medium-density STM32 devices have a length of 1024 bytes, the minimum that can be erased is to two pages, i.e., 2048 bytes. */
#define FIRMWARE_UPDATE_CONFIG_PAGE_2_START_ADDR  	(FIRMWARE_UPDATE_CONFIG_PAGE_1_START_ADDR + FW_UPDT_CONFIG_PAGE_SIZE)      			/**< @brief Designated Flash Memory address for the start of Firmware Update Configurations page 2. @details The start of Firmware Update Configurations page 2 should be 0x0801'E800. This is because the start of Firmware Update Configurations page 1 is address is 0x0801'E000 (see @ref FIRMWARE_UPDATE_CONFIG_PAGE_1_START_ADDR ) and the size of the Firmware Update Configurations page 1 length of 2048 bytes. */
#define FIRMWARE_UPDATE_CONFIG_START_ADDR  			(FIRMWARE_UPDATE_CONFIG_PAGE_1_START_ADDR)                              			/**< @brief Designated start Flash Memory address for the @ref firmware_update_config . @details For more information see @ref FIRMWARE_UPDATE_CONFIG_PAGE_1_START_ADDR . */
#define FIRMWARE_UPDATE_CONFIG_END_ADDR_PLUS_ONE	(FIRMWARE_UPDATE_CONFIG_PAGE_2_START_ADDR + FW_UPDT_CONFIG_PAGE_SIZE)             	/**< @brief Flash Memory address at which the start of the first page after the ones designated for the @ref firmware_update_config begins. @details For more information see @ref FIRMWARE_UPDATE_CONFIG_PAGE_2_START_ADDR . */
#define FLASH_BLOCK_NOT_ERASED  					(0x00)                          			                            			/**< @brief Designated value to indicate that a Firmware Update Configurations block has not been erased via @ref firmware_update_config_flags_t::is_erased . */
#define FLASH_BLOCK_ERASED              			(0xFF)                          			                            			/**< @brief Designated value to indicate that a Firmware Update Configurations block has been erased via @ref firmware_update_config_flags_t::is_erased . */
#define DATA_BLOCK_16BIT_ERASED_VALUE				(0xFFFF)									                            			/**< @brief Designated value to indicate that a certain 16-bit field value of the @ref firmware_update_config_data_t structure has either been erased or that there is no data in it. */
#define FIRMWARE_UPDATE_CONFIG_DATA_SIZE 			(sizeof(firmware_update_config_data_t))		                            			/**< @brief Length in bytes of the @ref firmware_update_config_data_t struct. */

/**@brief	Firmware Update Configurations Flags parameters structure. This contains all the fields needed for the flags
 *          used by the Firmware Update Configurations Blocks parameter structure (i.e., @ref firmware_update_config_t ).
 *
 * @note	If the size of this struct is to be changed, then make sure that the bytes size of the designated pages of
 *          the @ref firmware_update_config (which is 4096 bytes) are divisible by the resulting size of
 *          @ref firmware_update_config_t .
 */
typedef struct {
	uint16_t reserved2;			//!< 16-bits reserved for future possible uses for the Firmware Update Configurations sub-module.
	uint8_t reserved1;			//!< 8-bits reserved for future possible uses for the Firmware Update Configurations sub-module.
	uint8_t is_erased;			//!< Flag to indicate whether a Firmware Update Configurations block has been erased or not. @details 0x00 = Not erased<br> 0xFF = Has been erased
} firmware_update_config_flags_t;

/**@brief	Firmware Update Configurations Blocks parameters structure. This contains all the fields needed to
 * 			write/read/erase the Firmware Update Configurations Blocks.
 *
 * @note	If the size of this struct is to be changed, then make sure that the bytes size of the designated pages of
 *          the @ref firmware_update_config (which is 4096 bytes) are divisible by the resulting size of
 *          @ref firmware_update_config_t .
 * @note	For more details on the total size of all the designated Flash Memory pages of the
 * 			@ref firmware_update_config , see the @ref FW_UPDT_CONFIG_PAGE_SIZE definition and have in mind that the
 * 			@ref firmware_update_config states that this sub-module has been designated 4 Flash Memory pages.
 * @note	The \c aligned(4) attribute is used here because this struct must be a multiple of 4 bytes (i.e., 32-bits),
 *			since that is the configured TypeProgram with which our MCU/MPU's Flash Memory was configured with (see
 *			@ref write_data_to_flash_app and @ref FLASH_TYPEPROGRAM_WORD ). However, it is best if you set all the
 *			fields of this @ref firmware_update_config_t struct to types of 4-bytes instead of having the \c aligned(4)
 *			attribute for filling the required size of this struct. The reason for this is because that attribute will
 *			not initialize all those additional bytes to zeros or ones, and will set them to random values instead,
 *			which means that this could change the CRC32 calculation each time you write a new data block (note that as
 *			long as the CRC for each data block is correctly calculated, then that should not affect the functionality
 *			of this sub-module, but will have more difficult data to follow/read).
 */
typedef struct __attribute__ ((__packed__)) __attribute__ ((aligned (4))) {
	uint32_t crc32;										//!< Recorded 32-bits CRC of all the data contained in this struct.
	firmware_update_config_data_t data;					//!< Block data, which is where the actual data of a Data Block is stored.
	firmware_update_config_flags_t flags;				//!< Firmware Update Configurations Flags.
} firmware_update_config_t;

static const uint8_t FIRMWARE_UPDATE_CONFIG_BLOCK_SIZE = sizeof(firmware_update_config_t)/4;									/**< @brief The size in words (i.e., in 4 bytes) of one Firmware Update Configurations block. */
static const uint8_t FIRMWARE_UPDATE_CONFIG_BLOCK_SIZE_WITHOUT_CRC = FIRMWARE_UPDATE_CONFIG_BLOCK_SIZE*4 - sizeof(uint32_t);	/**< @brief The size in bytes of one Firmware Update Configurations block but without the space used for the 32-bit CRC field. */
static firmware_update_config_t *p_most_recent_val = NULL;																		/**< @brief Pointer to the Firmware Update Configurations Block containing the most recently written Firmware Update Configurations value. @details The valid Flash Memory addresses for this variable are: @ref FIRMWARE_UPDATE_CONFIG_START_ADDR \f$\leq validFlashMemoryAddresses <\f$ @ref FIRMWARE_UPDATE_CONFIG_END_ADDR_PLUS_ONE . @details If this variable has its pointer to \c NULL , then this will mean that there is currently no data in the Firmware Update Configuration's designated Flash Memory pages. */

/**@brief	Erases all the designated Flash Memory pages of the @ref firmware_update_config sub-module to restore them
 * 			to their original factory form.
 *
 * @retval				FIRM_UPDT_CONF_EC_OK
 * @retval				FIRM_UPDT_CONF_EC_NR
 * @retval				FIRM_UPDT_CONF_EC_ERR
 *
 * @note	For more details, see @ref FirmUpdConf_Status .
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    September 26, 2023.
 * @date    LAST UPDATE: February 07, 2024.
 */
static FirmUpdConf_Status restore_firm_updt_config_flash_memory();

/**@brief	Identifies if there is a Firmware Update Configurations page that is currently fully occupied with data
 *          while having already written data into the other page, such that if that is the case, then an instruction
 *          for erasing the Firmware Update Configurations page that is full will be send. Otherwise, this function does
 *          nothing.
 *
 * @note	This function sends the instruction of deleting a certain Firmware Update Configurations page number via the
 *          @ref page_erase function.
 * @note    See @ref FIRMWARE_UPDATE_CONFIG_PAGE_1_START_ADDR and @ref FIRMWARE_UPDATE_CONFIG_PAGE_2_START_ADDR to be
 *          able to correctly differentiate a Firmware Update Configurations page from a Flash Memory Page.
 *
 * @retval				FIRM_UPDT_CONF_EC_OK
 * @retval				FIRM_UPDT_CONF_EC_NR
 * @retval				FIRM_UPDT_CONF_EC_ERR
 *
 * @note	For more details, see @ref FirmUpdConf_Status .
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    September 26, 2023.
 * @date    LAST UPDATE: February 07, 2024.
 */
static FirmUpdConf_Status prep_page_swap();

/**@brief	Erases a desired Flash Memory page.
 *
 * @param page_start_addr	Pointer to the Flash Memory start address of the page that is desired to be erased.
 *
 * @note	For some reason that it is not explained in the STMicroelectronics documentation, whenever you request to
 *          erase 1 Flash Memory Page to our MCU/MPU's Hardware (which is the minimum that you can request to erase,
 *          i.e., you cannot request to erase 0 pages and 1 is the minimum accepted), 2 Flash Memory Pages are erased
 *          instead of 1, which leaves us to the restriction of only being able to erase a minimum of 2 Flash Memory
 *          Pages and not 1. However, if you request to erase several Flash Memory Pages, then our MCU/MPU's Hardware
 *          will only delete the exact number of Flash Memory Pages.
 *
 * @retval				FIRM_UPDT_CONF_EC_OK
 * @retval				FIRM_UPDT_CONF_EC_NR
 * @retval				FIRM_UPDT_CONF_EC_ERR
 *
 * @note	For more details, see @ref FirmUpdConf_Status .
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    October 13, 2023.
 * @date    LAST UPDATE: February 07, 2024.
 */
static FirmUpdConf_Status page_erase(uint32_t *page_start_addr);

/**@brief	Gets the corresponding @ref FirmUpdConf_Status value depending on the given @ref HAL_StatusTypeDef value.
 *
 * @param HAL_status	HAL Status value (see @ref HAL_StatusTypeDef ) that wants to be converted into its equivalent
 * 						of a @ref FirmUpdConf_Status value.
 *
 * @retval				FIRM_UPDT_CONF_EC_NR if \p HAL_status param equals \c HAL_BUSY or \c HAL_TIMEOUT .
 * @retval				FIRM_UPDT_CONF_EC_ERR if \p HAL_status param equals \c HAL_ERROR .
 * @retval				HAL_status otherwise.
 *
 * @note	For more details on the returned values listed, see @ref FirmUpdConf_Status and @ref HAL_StatusTypeDef .
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date September 26, 2023.
 */
static FirmUpdConf_Status HAL_ret_handler(HAL_StatusTypeDef HAL_status);

FirmUpdConf_Status firmware_update_configurations_init()
{
	/** <b>Local variable ret:</b> Used to hold the exception code value returned by a @ref FirmUpdConf_Status function. */
	uint16_t ret;
	/** <b>Local variable cal_crc:</b> Value holder for the calculated 32-bit CRC of the Data Block to which the @ref p_most_recent_val pointer points to. */
	uint32_t cal_crc;
	/** <b>Local variable p_next_val:</b> Firmware Update Configurations Block pointer that should point to the Firmware Update Configurations Block located right after the one with the most recently written value (i.e., right after the Data Block to which the @ref p_most_recent_val pointer points to). */
	firmware_update_config_t *p_next_val;

	p_most_recent_val = ((firmware_update_config_t *) FIRMWARE_UPDATE_CONFIG_END_ADDR_PLUS_ONE) - 1;

	/* Cycle through flash until an erased value is found. */
	#if ETX_OTA_VERBOSE
		printf("Initializing Firmware Update Configurations sub-module...\r\n");
	#endif
	for (p_next_val = (firmware_update_config_t *)FIRMWARE_UPDATE_CONFIG_START_ADDR; p_next_val < (firmware_update_config_t *)FIRMWARE_UPDATE_CONFIG_END_ADDR_PLUS_ONE; p_next_val++)
	{
		if (p_next_val->flags.is_erased == FLASH_BLOCK_ERASED)
		{
			if (p_most_recent_val->flags.is_erased == FLASH_BLOCK_NOT_ERASED)
			{
				/* Calculate and verify the 32-bit CRC of @ref p_most_recent_val . If validation is successful, then proceed normally. Otherwise, ignore the current Data Block that @ref p_most_recent_val is pointing towards to and keep searching. */
				cal_crc = crc32_mpeg2((uint8_t *) &p_most_recent_val->data, FIRMWARE_UPDATE_CONFIG_BLOCK_SIZE_WITHOUT_CRC);
				if(cal_crc != p_most_recent_val->crc32)
				{
					/* Since the Flash Memory has been identified to be corrupted, erase the contents of the two Flash Memory pages of the Firmware Update Configurations sub-module. */
					#if ETX_OTA_VERBOSE
						printf("WARNING: One of the Flash Memory pages designated to the Firmware Update Configurations sub-module has been identified to be corrupted.\r\n");
					#endif
					ret = restore_firm_updt_config_flash_memory();
					if (ret != FIRM_UPDT_CONF_EC_OK)
					{
						#if ETX_OTA_VERBOSE
							printf("ERROR: The Firmware Update Configurations sub-module could not be initialized.\r\n");
						#endif
						return FIRM_UPDT_CONF_EC_CRPT;
					}

					/* We define that there is no data in the Flash Memory pages of the Firmware Update Configurations sub-module. */
					p_most_recent_val = NULL;

					#if ETX_OTA_VERBOSE
						printf("DONE: The Firmware Update Configurations sub-module was successfully initialized.\r\n");
					#endif
					return FIRM_UPDT_CONF_EC_OK;
				}
				break;
			}
		}
		p_most_recent_val = p_next_val;
	}

	/* If the end of the for-loop is reached and if last location is erased, then there is currently no data in the Firmware Update Configuration's designated Flash Memory pages. */
	if (p_next_val == (firmware_update_config_t *) FIRMWARE_UPDATE_CONFIG_END_ADDR_PLUS_ONE)
	{
		if (p_most_recent_val->flags.is_erased == FLASH_BLOCK_ERASED)
		{
			p_most_recent_val = NULL;
		}
	}

	/* If one of the designated Flash Memory pages of the Firmware Update Configurations sub-module is full, then erase it. */
	ret = prep_page_swap();
    #if ETX_OTA_VERBOSE
        if (ret != FIRM_UPDT_CONF_EC_OK)
        {
            printf("ERROR: The Firmware Update Configurations sub-module could not be initialized.\r\n");
        }
        else
        {
            printf("DONE: The Firmware Update Configurations sub-module was successfully initialized.\r\n");
        }
    #endif

	return ret;
}

FirmUpdConf_Status firmware_update_configurations_read(firmware_update_config_data_t *p_data)
{
    firmware_update_config_t *p_current_val = p_most_recent_val;
    if (p_current_val == NULL)
    {
        p_current_val = (firmware_update_config_t *) FIRMWARE_UPDATE_CONFIG_START_ADDR;
        memcpy(p_data, &(p_current_val->data), FIRMWARE_UPDATE_CONFIG_DATA_SIZE);
        return FIRM_UPDT_CONF_EC_NO_DATA;
    }

    memcpy(p_data, &(p_current_val->data), FIRMWARE_UPDATE_CONFIG_DATA_SIZE);
    return FIRM_UPDT_CONF_EC_OK;
}

FirmUpdConf_Status firmware_update_configurations_write(firmware_update_config_data_t *p_data)
{
	/** <b>Local variable ret:</b> Return value of a @ref FirmUpdConf_Status function. */
	uint8_t ret;
	/**	<b>Local variable new_val_struct:</b> New Data Block into which we will pass the data that wants to be written and where we will also set the corresponding flag and CRC32 values for it. */
	firmware_update_config_t new_val_struct;
	/**	<b>Local pointer p_new_val_struct:</b> Pointer to the \c new_val_struct data but in \c uint32_t Type. */
	uint32_t *p_new_val_struct = (uint32_t *) &new_val_struct;
	/**	<b>Local pointer p_next_val:</b> @ref firmware_update_config_t Type Pointer that will point towards the address of the next available data block of the @ref firmware_update_config . */
	firmware_update_config_t *p_next_val = (firmware_update_config_t *) FIRMWARE_UPDATE_CONFIG_START_ADDR;

	/* We pass the received data into a new Data Block structure and we calculate and also set its corresponding 32-bit CRC. */
    memcpy(&new_val_struct.data, p_data, FIRMWARE_UPDATE_CONFIG_DATA_SIZE);
    new_val_struct.data.reserved1 = DATA_BLOCK_32BIT_ERASED_VALUE; // Make sure to keep reserved data's bits set to 1's.
    new_val_struct.data.reserved2 = DATA_BLOCK_16BIT_ERASED_VALUE; // Make sure to keep reserved data's bits set to 1's.
    new_val_struct.flags.reserved2 = DATA_BLOCK_16BIT_ERASED_VALUE; // Make sure to keep reserved data's bits set to 1's.
    new_val_struct.flags.reserved1 = DATA_BLOCK_8BIT_ERASED_VALUE; // Make sure to keep reserved data's bits set to 1's.
    new_val_struct.flags.is_erased = FLASH_BLOCK_NOT_ERASED;
    new_val_struct.crc32 = crc32_mpeg2((uint8_t *) &new_val_struct.data, FIRMWARE_UPDATE_CONFIG_BLOCK_SIZE_WITHOUT_CRC);

	/* We calculate the next available address. */
	if (p_most_recent_val != NULL)
	{
		p_next_val = p_most_recent_val + 1;
		if (p_next_val == (firmware_update_config_t *) FIRMWARE_UPDATE_CONFIG_END_ADDR_PLUS_ONE)
		{
			p_next_val = (firmware_update_config_t *) FIRMWARE_UPDATE_CONFIG_START_ADDR;
		}
	}
	/**	<b>Local pointer p_next_val_in_words:</b> 32-bits Type Pointer that will point towards the address of the next available data block of the @ref firmware_update_config . */
	uint32_t *p_next_val_in_words = (uint32_t *) p_next_val;

	/* We unlock our MCU/MPU's Flash Memory to be able to write in it. */
	ret = HAL_FLASH_Unlock();
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: HAL Flash could not be unlocked; Firmware Update Configurations Exception code %d.\r\n", ret);
		#endif
		return ret;
	}

	/* Write the new Firmware Update Configuration's Data Block into the corresponding Flash Memory address. */
	for (uint8_t words_written=0; words_written<FIRMWARE_UPDATE_CONFIG_BLOCK_SIZE; words_written++)
	{
		ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
								(uint32_t) (p_next_val_in_words + words_written),
							    p_new_val_struct[words_written]);
		ret = HAL_ret_handler(ret);
		if (ret != HAL_OK)
		{
			#if ETX_OTA_VERBOSE
				printf("ERROR: Flash Write Error.\r\n");
			#endif
			return ret;
		}
	}

	/* Leave the Flash Memory locked as it originally was. */
	ret = HAL_FLASH_Lock();
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The Flash Memory could not be locked; Firmware Update Configurations Exception code %d.\r\n", ret);
		#endif
		return ret;
	}

	/* Update the @ref p_most_recent_val pointer to the most recent Data Block of the Firmware Update Configurations sub-module. */
	p_most_recent_val = p_next_val;

	/* If one of the designated Flash Memory pages of the Firmware Update Configurations sub-module is full, then erase it. */
	ret = prep_page_swap();

	#if ETX_OTA_VERBOSE
		printf("DONE: The requested data was successfully written into the Firmware Update Configurations sub-module.\r\n");
	#endif
	return ret;
}

static FirmUpdConf_Status restore_firm_updt_config_flash_memory()
{
	/** <b>Local variable ret:</b> Return value of a @ref FirmUpdConf_Status function. */
    FirmUpdConf_Status ret;

	#if ETX_OTA_VERBOSE
		printf("Erasing all Flash Memory pages from the Firmware Update Configurations sub-module to restore their functionalities...\r\n");
	#endif
	ret = page_erase((uint32_t *) FIRMWARE_UPDATE_CONFIG_PAGE_1_START_ADDR);
	if (ret != FIRM_UPDT_CONF_EC_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The designated Flash Memory page 1 of the Firmware Update Configurations sub-module could not be restored.\r\n");
		#endif
		return ret;
	}
	ret = page_erase((uint32_t *) FIRMWARE_UPDATE_CONFIG_PAGE_2_START_ADDR);
	if (ret != FIRM_UPDT_CONF_EC_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The designated Flash Memory page 2 of the Firmware Update Configurations sub-module could not be restored.\r\n");
		#endif
		return ret;
	}

	#if ETX_OTA_VERBOSE
		printf("DONE: The Flash Memory pages from the Firmware Update Configurations sub-module have been restored to its original factory form.\r\n");
	#endif
	return FIRM_UPDT_CONF_EC_OK;
}

static FirmUpdConf_Status prep_page_swap()
{
    /** <b>Local variable ret:</b> @ref FirmUpdConf_Status Type variable used to hold the return value of a @ref FirmUpdConf_Status function. */
    FirmUpdConf_Status ret;
	#if ETX_OTA_VERBOSE
		printf("Preparing the Firmware Update Configurations sub-module for a possible future page swap...\r\n");
	#endif

	/* If one of the designated Flash Memory pages of the Firmware Update Configurations sub-module is full, then erase it. */
    if (p_most_recent_val == ((firmware_update_config_t *) FIRMWARE_UPDATE_CONFIG_PAGE_1_START_ADDR) &&
        (((firmware_update_config_t *) FIRMWARE_UPDATE_CONFIG_END_ADDR_PLUS_ONE)-1)->flags.is_erased == FLASH_BLOCK_NOT_ERASED)
    {
        #if ETX_OTA_VERBOSE
            printf("Requesting to erase Firmware Update Configuration's page 2...\r\n");
        #endif
        ret = page_erase((uint32_t *) FIRMWARE_UPDATE_CONFIG_PAGE_2_START_ADDR);
        #if ETX_OTA_VERBOSE
            if (ret != FIRM_UPDT_CONF_EC_OK)
            {
                printf("ERROR: The Firmware Update Configuration's page 2 could not be erased.\r\n");
            }
            else
            {
                printf("DONE: The Firmware Update Configuration's page 2 was successfully erased.\r\n");
            }
        #endif
    }
    else if (p_most_recent_val == ((firmware_update_config_t *) FIRMWARE_UPDATE_CONFIG_PAGE_2_START_ADDR) &&
             (p_most_recent_val-1)->flags.is_erased == FLASH_BLOCK_NOT_ERASED)
    {
        #if ETX_OTA_VERBOSE
            printf("Requesting to erase Firmware Update Configuration's page 1...\r\n");
        #endif
        ret = page_erase((uint32_t *) FIRMWARE_UPDATE_CONFIG_PAGE_1_START_ADDR);
        #if ETX_OTA_VERBOSE
            if (ret != FIRM_UPDT_CONF_EC_OK)
            {
                printf("ERROR: The Firmware Update Configuration's page 1 could not be erased.\r\n");
            }
            else
            {
                printf("DONE: The Firmware Update Configuration's page 1 was successfully erased.\r\n");
            }
        #endif
    }
	else
	{
		ret = FIRM_UPDT_CONF_EC_OK;
		#if ETX_OTA_VERBOSE
			printf("DONE: No page swap preparations required at the moment.\r\n");
		#endif
	}

	return ret;
}

static FirmUpdConf_Status page_erase(uint32_t *page_start_addr)
{
	#if ETX_OTA_VERBOSE
		printf("Erasing the Flash memory page that starts at address 0x%08X...\r\n", (unsigned int) page_start_addr);
	#endif
    /** <b>Local variable ret:</b> @ref uin8_t Type variable used to hold the return value of either a @ref FirmUpdConf_Status or a @ref HAL_StatusTypeDef function. */
    uint8_t ret;

	/* Unlock HAL Flash */
	ret = HAL_FLASH_Unlock();
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: HAL Flash could not be unlocked; Firmware Update Configurations Exception code %d.\r\n", ret);
		#endif
		return ret;
	}

	/* Erase desired Flash Memory page. */
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t page_error;
	EraseInitStruct.TypeErase    = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks        = FLASH_BANK_1;
	EraseInitStruct.PageAddress  = (uint32_t) page_start_addr;
	EraseInitStruct.NbPages      = 1U;

	ret = HAL_FLASHEx_Erase(&EraseInitStruct, &page_error);
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: Requested Flash Memory page at address 0x%08X could not be erased; Firmware Update Configurations Exception code %d.\r\n", (unsigned int) page_start_addr, ret);
		#endif
		return ret;
	}

	/* Leave the Flash Memory locked as it originally was. */
	ret = HAL_FLASH_Lock();
	ret = HAL_ret_handler(ret);
	if (ret != HAL_OK)
	{
		#if ETX_OTA_VERBOSE
			printf("ERROR: The Flash Memory could not be locked; Firmware Update Configurations Exception code %d.\r\n", ret);
		#endif
		return ret;
	}

	#if ETX_OTA_VERBOSE
		printf("DONE: The requested Flash Memory page was successfully erased.\r\n");
	#endif
	return FIRM_UPDT_CONF_EC_OK;
}

static FirmUpdConf_Status HAL_ret_handler(HAL_StatusTypeDef HAL_status)
{
  switch (HAL_status)
    {
  	  case HAL_BUSY:
	  case HAL_TIMEOUT:
		return FIRM_UPDT_CONF_EC_NR;
	  case HAL_ERROR:
		return FIRM_UPDT_CONF_EC_ERR;
	  default:
		return HAL_status;
    }
}

/** @} */
