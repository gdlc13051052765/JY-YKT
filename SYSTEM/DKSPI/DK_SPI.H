#ifndef __DK_SPI_H
#define __DK_SPI_H
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define mySPI1 1
#define mySPI2 2
#define mySPI3 3

void DK_SPI_Init(u8 SPIx);
void DK_SPI1_Send_Byte(u8 SPIx,u8 Data);
u8 DK_SPI1_Recieve_Byte(u8 SPIx);
u8 DK_SPI1_S_R_Byte(u8 SPIx,u8 byte);
void DK_SPI_SetSpeed(u8 SPIx,u8 SpeedSet);

#endif
