#ifndef __AT24C256_H
#define __AT24C256_H	
#include "char.H"

#define EEPROM_PageSize	128
#define EEPROM_PageNum	512   
#define DwordOrByte_Add  1
#define EEPROM_Total_Size	EEPROM_PageSize*EEPROM_PageNum	
			   
//IIC所有操作函数
void Init_SWIIC(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
unsigned char IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
unsigned char IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号
void AT24C256_Init(void);
void WrByte_24c256(unsigned short WriteAddr,unsigned char DataToWrite);
unsigned char RdByte_24C256(uint ReadAddr);
//unsigned char Formate_EE(void);
void SW_EEPROM_Delay_nus(unsigned short nus);
void SW_EEPROM_Delay_nms(unsigned short nms);
//void check_24C02(void);
#endif
	 



