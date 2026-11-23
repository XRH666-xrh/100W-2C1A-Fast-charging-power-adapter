/**
  ******************************************************************************
  * @file    flash.c
  * @author  MCU Application Team
  * @brief   Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"

/* Private define ------------------------------------------------------------*/
#define FLASH_USER_START_ADDR     0x0800f000
#define FLASH_USER_LENGTH         0x1000
#define PAGE_MASK                 (FLASH_PAGE_SIZE-1)

/* Private user code ---------------------------------------------------------*/
uint8_t buf[FLASH_PAGE_SIZE] = {0};

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_FlashErase(void);
static void APP_FlashProgramPage(uint32_t addr, uint32_t *dat);


uint8_t FlashReadByteConfig(void)
{
	uint8_t dat;
	uint16_t save_offset = 0;
	while(save_offset < FLASH_USER_LENGTH)
	{
		dat = HW8_REG(FLASH_USER_START_ADDR + save_offset);
		if (dat & 0x80)
		{
			// bit8 = valid
			return dat;
		}
		save_offset++;
	}
	return 0; // invalid
}

int32_t FlashReadDWordConfig(void)
{
	uint32_t dat;
	uint16_t save_offset = 0;
	while(save_offset < FLASH_USER_LENGTH)
	{
		dat = HW32_REG(FLASH_USER_START_ADDR + save_offset);
		if (dat & (1<<31))
		{
			return dat;
		}
		save_offset+=4;
	}
	return -1; // invalid
}

static void FLASHReadPage(uint16_t user_offset)
{
	uint8_t buf_offset = 0;
	while (buf_offset < FLASH_PAGE_SIZE)
	{
		(*(uint32_t*)(buf+buf_offset)) = HW32_REG(FLASH_USER_START_ADDR+user_offset+buf_offset);
		buf_offset += 4;
	}
}

uint8_t FlashUpdateByteConfig(uint8_t dat)
{
	uint16_t save_offset = 0;
	while(save_offset < FLASH_USER_LENGTH)
	{
		if (HW8_REG(FLASH_USER_START_ADDR + save_offset) & 0x80)
		{
			break;
		}
		save_offset++;
	}
	
  /* Unlock Flash */
  HAL_FLASH_Unlock();

	if (save_offset >= FLASH_USER_LENGTH -1)
	{
		/* ALL SAVE ADDRESS ARE FULL, ERASE FLASH AND WRITE FROM START*/
		APP_FlashErase();
		memset(buf, 0xff, FLASH_PAGE_SIZE);
		buf[0] = dat;
		APP_FlashProgramPage(FLASH_USER_START_ADDR, (uint32_t*)buf);
		save_offset = 0;
	}
	else
	{
		uint16_t page_start = save_offset & ~PAGE_MASK;
		uint8_t page_offset = save_offset & PAGE_MASK;
		// readout flash
		FLASHReadPage(page_start);
		buf[page_offset++] &= ~0x80; // set old data as invalid and move to next write byte
		if (page_offset == FLASH_PAGE_SIZE)
		{
			APP_FlashProgramPage(FLASH_USER_START_ADDR+page_start, (uint32_t*)buf);
			page_start += FLASH_PAGE_SIZE;
			page_offset = 0; // write next page
			memset(buf, 0xff, FLASH_PAGE_SIZE);
		}
		buf[page_offset] = dat;
		APP_FlashProgramPage(FLASH_USER_START_ADDR+page_start, (uint32_t*)buf);
		save_offset = page_start + page_offset;
	}
  /* Lock Flash */
  HAL_FLASH_Lock();

	return 0;
}

uint8_t FlashUpdateDWordConfig(int32_t dat31)
{
	uint32_t dat = dat31 | (1 << 31);
	uint16_t save_offset = 0;
	uint32_t datr;
	while(save_offset < FLASH_USER_LENGTH)
	{
		datr = HW32_REG(FLASH_USER_START_ADDR + save_offset);
		if (datr & (1<<31))
		{
			break;
		}
		save_offset+=4;
	}
	
  /* Unlock Flash */
  HAL_FLASH_Unlock();

	if (save_offset >= FLASH_USER_LENGTH -5)
	{
		/* ALL SAVE ADDRESS ARE FULL, ERASE FLASH AND WRITE FROM START*/
		APP_FlashErase();
		memset(buf, 0xff, FLASH_PAGE_SIZE);
		uint32_t *buffpt = (uint32_t*)(buf);
		*buffpt = dat;
		APP_FlashProgramPage(FLASH_USER_START_ADDR, (uint32_t*)buf);
		save_offset = 0;
	}
	else
	{
		uint16_t page_start = save_offset & ~PAGE_MASK;
		uint8_t page_offset = save_offset & PAGE_MASK;
		// readout flash
		FLASHReadPage(page_start);
		buf[page_offset+3] &= ~0x80; // set old data as invalid and move to next write byte
		page_offset+=4;
		if (page_offset == FLASH_PAGE_SIZE)
		{
			APP_FlashProgramPage(FLASH_USER_START_ADDR+page_start, (uint32_t*)buf);
			page_start += FLASH_PAGE_SIZE;
			page_offset = 0; // write next page
			memset(buf, 0xff, FLASH_PAGE_SIZE);
		}
		uint32_t *buffpt = (uint32_t*)(buf+page_offset);
		*buffpt = dat;
		APP_FlashProgramPage(FLASH_USER_START_ADDR+page_start, (uint32_t*)buf);
		save_offset = page_start + page_offset;
	}
  /* Lock Flash */
  HAL_FLASH_Lock();

	return 0;
}

/**
  * @brief  FLASH erase FULL User data sector
  * @param  None
  * @retval None
  */
static void APP_FlashErase(void)
{
  uint32_t PAGEError = 0;
  FLASH_EraseInitTypeDef EraseInitStruct = {0};

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORERASE;        /* Sector Erase */
  EraseInitStruct.SectorAddress = FLASH_USER_START_ADDR;            /* Erase Start Address */
  EraseInitStruct.NbSectors  = 1  ;                                 /* Erase Page Number */
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)  /* Page Erase */
  {
    APP_ErrorHandler();
  }
}

/**
  * @brief  FLASH programming a uint32_t
  * @param  None
  * @retval None
  */

static void APP_FlashProgramPage(uint32_t addr, uint32_t *dat)
{
  uint32_t flash_program_start = addr;                                /* Start address of user write flash */
  uint32_t *src = (uint32_t *)dat;                                                     /* Pointer to array */

	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, flash_program_start, src) == HAL_OK)/* Program */
	{
	}
}

