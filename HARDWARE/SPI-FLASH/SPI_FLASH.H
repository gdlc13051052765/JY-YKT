#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H	

#define Dummy_Byte 0

extern unsigned char fac_id,dev_id;



void SPI_Flash_Init(void);	        //SPI初始化
u8 SPI_Flash_ReadByte(void);		//flash操作基本函数，读一个字节
u8 SPI_Flash_SendByte(u8 byte);		//	FLASH操作基本函数，发送一个字节


void FlashWaitBusy(void);			//Flash忙检测
void FlashReadID(void);		        //读取flashID四个字节

//void Erase_One_Sector (unsigned long Addr);//FLASH块擦除

void SPI_Flash_Text(void); //FLASH的测试函数
#endif

