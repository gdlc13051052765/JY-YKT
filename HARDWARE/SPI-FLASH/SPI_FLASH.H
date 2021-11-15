#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H	

#define Dummy_Byte 0

extern unsigned char fac_id,dev_id;



void SPI_Flash_Init(void);	        //SPI��ʼ��
u8 SPI_Flash_ReadByte(void);		//flash����������������һ���ֽ�
u8 SPI_Flash_SendByte(u8 byte);		//	FLASH������������������һ���ֽ�


void FlashWaitBusy(void);			//Flashæ���
void FlashReadID(void);		        //��ȡflashID�ĸ��ֽ�

//void Erase_One_Sector (unsigned long Addr);//FLASH�����

void SPI_Flash_Text(void); //FLASH�Ĳ��Ժ���
#endif
