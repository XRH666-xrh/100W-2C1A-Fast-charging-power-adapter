#ifndef __FLASH__H__
#define __FLASH__H__

#include "main.h"


// !!! you should not read dword config but save byte config, or inverse
int32_t FlashReadDWordConfig(void);
uint8_t FlashUpdateDWordConfig(uint32_t dat);
uint8_t FlashReadByteConfig(void);
uint8_t FlashUpdateByteConfig(uint8_t dat);

#endif
