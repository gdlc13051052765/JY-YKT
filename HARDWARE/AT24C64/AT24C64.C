#include "stm32f10x_gpio.h"
#include "iic.H"
#include  "USART.H"
#include <string.h>
extern void UdpSendChar(unsigned char *buffer,unsigned int len);
extern void Clr_WatchDog(void);
/*****************************************************************
AT24C64 ��Ӧ�Ű����ϵ�U24����ַ��ȫ��ѡ��VCC�������洢ϵͳ��ʼ������
****************************************************************/
//�����ͺ�
#define EE24C64	    
//������ַ������ 	��Ϊ��ַ�߶���VCC���Ե�ַ��ae
#define C256_EEPROM_ADD    0xAE

#define EEPROM_PageSize	32
#define EEPROM_PageNum	128   
#define DwordOrByte_Add_64  1
#define EEPROM_Total_Size	0x2000
		 
//�����е���ʱ��������ڲ���Ϊ0��65535
static void SW_EEPROM_Delay_nus(unsigned short nus)
{
	for(;nus>0;nus--)
	{__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();}
}
static void SW_EEPROM_Delay_nms(unsigned short nms)
{
	for(;nms>0;nms--)
	SW_EEPROM_Delay_nus(1000);
}


//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
 unsigned char AT24C64_ReadOneByte(unsigned short ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_64)
	{
		IIC_Send_Byte(C256_EEPROM_ADD);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8); //���͸ߵ�ַ
		//IIC_Wait_Ack();		 
	}else IIC_Send_Byte(C256_EEPROM_ADD+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

		IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
		IIC_Wait_Ack();	    
		IIC_Start();  	 	   
		IIC_Send_Byte(C256_EEPROM_ADD+1);           //�������ģʽ			   
		IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24C64_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_64)
	{
		IIC_Send_Byte(C256_EEPROM_ADD);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
		//IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(C256_EEPROM_ADD+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	SW_EEPROM_Delay_nms(20);	 
}

void AT24CXX_WriteSome(u16 WriteAddr,u8 DataToWrite)
{
		u8 i;
	 IIC_Start();  
	if(1==DwordOrByte_Add_64)
	{
		IIC_Send_Byte(C256_EEPROM_ADD);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
		//IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(C256_EEPROM_ADD+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	
	for(i=0;i<16;i++)
	{
		IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
		IIC_Wait_Ack(); 
	}		
    IIC_Stop();//����һ��ֹͣ���� 
	SW_EEPROM_Delay_nms(10);
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
unsigned char RdBytesFromAT24C64(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
	unsigned int END_Page=0; 
//	unsigned char b[50];
  
	END_Page=(ReadAddr+NumToRead)/EEPROM_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//����Χ 
	while(NumToRead)
	{
		*pBuffer=AT24C64_ReadOneByte(ReadAddr);	
	//	b[NumToRead] = *pBuffer;
		ReadAddr++;
		pBuffer++;
		NumToRead--;
	}
	return 0;
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ 
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
unsigned char WrBytesToAT24C64(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
	unsigned int END_Page=0; 

	END_Page=(WriteAddr+NumToWrite)/EEPROM_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//����Χ 
	while(NumToWrite)
	{
		AT24C64_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
		NumToWrite--;
	}		
	return 0;
}
/************************************************************************************
**��    ��:  ��ʽ��EEPROM
**��ڲ���: 
**���ڲ���: �ɹ�����0
************************************************************************************/

u8 Check_24C64(uint16_t Addr)
{
	char	st_data;
	st_data=AT24C64_ReadOneByte(Addr);	
	AT24C64_WriteOneByte(Addr,0xaa);
	if (AT24C64_ReadOneByte(Addr)!=0xaa)
		return	1;
	else
	{
		AT24C64_WriteOneByte(Addr,0x55);
		if (AT24C64_ReadOneByte(Addr)!=0x55)
			return	1;
		else
		{
			AT24C64_WriteOneByte(Addr,st_data);
			return	0;
		}
	}
} 

void Text_24C64(void)
{
	unsigned char aa,j;
	unsigned short i;
	unsigned long Addr =0 ;
	unsigned char buffer[18];
	j=0;
	for(i=0;i<500;i++)
	{
		AT24CXX_WriteSome(Addr,0xff);
		Addr+=16;
		Clr_WatchDog();	
	}
	for(i=0;i<8000;i++)
	{
		buffer[j++]= AT24C64_ReadOneByte(i);
		if(j==16)
		{
			j=0;
//			UdpSendChar(buffer,16);
			Clr_WatchDog();
		}
	}
}