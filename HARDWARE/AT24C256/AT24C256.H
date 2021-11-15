#ifndef __AT24C256_H
#define __AT24C256_H	
#include "char.H"

#define EEPROM_PageSize	128
#define EEPROM_PageNum	512   
#define DwordOrByte_Add  1
#define EEPROM_Total_Size	EEPROM_PageSize*EEPROM_PageNum	
			   
//IIC���в�������
void Init_SWIIC(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
unsigned char IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
unsigned char IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�
void AT24C256_Init(void);
void WrByte_24c256(unsigned short WriteAddr,unsigned char DataToWrite);
unsigned char RdByte_24C256(uint ReadAddr);
//unsigned char Formate_EE(void);
void SW_EEPROM_Delay_nus(unsigned short nus);
void SW_EEPROM_Delay_nms(unsigned short nms);
//void check_24C02(void);
#endif
	 


