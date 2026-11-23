#ifndef __tiny_Flash__
#define __tiny_Flash__

#include "tinyFlash_port.h"

//tinyFlash提供了如下API供应用程序使用
bool tinyFlash_Init();	
int  tinyFlash_Read(unsigned char KEY, unsigned char * buf, unsigned char * len);
int  tinyFlash_Write(unsigned char KEY, unsigned char * buf, unsigned char len);
void tinyFlash_Swap(); //扇区使用完了，需要清理数据，才能存储别的数据

#endif