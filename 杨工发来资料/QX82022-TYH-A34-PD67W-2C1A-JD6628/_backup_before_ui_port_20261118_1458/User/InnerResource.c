#include <main.h>
#include "context/guicontext.h"
#include "usermain.h"

#if INTERNAL_FLASH
extern const uint8_t flythings_lite_ui_bin[];


static void fy_flash_read(uint32_t address24, uint8_t* buffer, uint32_t length)
{
  memcpy(buffer, &flythings_lite_ui_bin[address24],length);
}


static uint32_t fy_flash_isreceiveing(void)
{
	return 0;
}

static void fly_flash_init(void)
{
}

const flythings_flash	i_flash ={
  .flash_init = fly_flash_init,
  .flash_deinit = fly_flash_init,
  .flash_readdata = fy_flash_read,
  .flash_readdata_dma = fy_flash_read,
  .flash_wait_dma_end = fly_flash_init, 
  .flash_isreading = fy_flash_isreceiveing
};

#endif
