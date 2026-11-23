/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private define ------------------------------------------------------------*/
//#define BOARD_IN_MASTER_MODE
#define DATA_LENGTH      15                 /* Length of data */
#define I2C_ADDRESS      0x5e               /* Own address 0xA0 */
#define I2C_SPEEDCLOCK   100000             /* Communication speed : 100K */
#define I2C_DUTYCYCLE    I2C_DUTYCYCLE_16_9 /* Duty cycle */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef I2cHandle;

uint8_t aTxBuffer[DATA_LENGTH] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t aRxBuffer[DATA_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/**
  * @brief  Main program.
  * @retval int
  */
int i2c_hal_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOF_CLK_ENABLE();                       /* Enable GPIOC clock */
  __HAL_RCC_SYSCFG_CLK_ENABLE();                      /* Enable SYSCFG clock */
  __HAL_RCC_I2C1_CLK_ENABLE();                        /* Enable I2C1 clock */

  /**I2C GPIO Configuration
  PA8  -------> I2C1_SCL
  PA7  -------> I2C1_SDA
  */
  GPIO_InitStruct.Pin       = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;               /* Open-drain mode */
  GPIO_InitStruct.Pull      = GPIO_PULLUP;                   /* Pull-up */
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_I2C;                /* Alternate as I2C */
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);                    /* Initialize GPIO */
  /* I2C initialization */
  I2cHandle.Instance               = I2C1;                        /* I2C */
  I2cHandle.Init.ClockSpeed        = I2C_SPEEDCLOCK;              /* I2C communication speed */
  I2cHandle.Init.DutyCycle         = I2C_DUTYCYCLE;               /* I2C Duty cycle */
  I2cHandle.Init.AddressingMode    = I2C_ADDRESSINGMODE_7BIT;     /* 7-bit addressing mode */
  I2cHandle.Init.DataHoldTimeSel   = I2C_DATA_HOLD_TIME_HARDWARE; /* Data hold time selection */
  I2cHandle.Init.DualAddressMode   = I2C_DUALADDRESS_DISABLE;     /* Disable dual address */
  I2cHandle.Init.TripleAddressMode = I2C_TRIPLEADDRESS_DISABLE;   /* Disable triple address */
  I2cHandle.Init.GeneralCallMode   = I2C_GENERALCALL_DISABLE;     /* Disable general call */
  I2cHandle.Init.NoStretchMode     = I2C_NOSTRETCH_DISABLE;       /* Disable clock stretching */
  I2cHandle.Init.OwnAddress1       = I2C_ADDRESS;                 /* I2C address */
  I2cHandle.Init.OwnAddress2       = 0x0;                         /* Second address */
  I2cHandle.Init.OwnAddress3       = 0x0;                         /* Third  address */
  I2cHandle.Init.OwnAddress2Masks  = I2C_OA2_NOMASK;              /* Own Address2 Masks */
  if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
  {
    APP_ErrorHandler();
  }
	return 0;
}

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
