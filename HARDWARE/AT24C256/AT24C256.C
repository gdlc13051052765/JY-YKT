#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "iic.H"
#include  "USART.H"
#include <string.h>
#include "includes.h"
#include "char.h"

/*****************************************************************
	AT24C256 ��Ӧ�Ű����ϵ�U23����ַ��ȫ��ѡ��أ������洢����������
****************************************************************/
unsigned char buffer[8];
//�����ͺ�
#define EE24C256 		    
//������ַ������ 	��Ϊ��ַ�߶��ӵ����Ե�ַ��a0
#define I2C_EEPROM_ADD    0xA0

#define EEPROM_PageSize	64
#define EEPROM_PageNum	128   
#define DwordOrByte_Add_256  1
#define EEPROM_Total_Size	0x4000
		 		
 //IIC���в�������
static void Init_SWIIC(void);                //��ʼ��IIC��IO��				 
static void IIC_Start(void);				//����IIC��ʼ�ź�
static void IIC_Stop(void);	  			//����IICֹͣ�ź�
static void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
static unsigned char IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
static unsigned char IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
static void IIC_Ack(void);					//IIC����ACK�ź�
static void IIC_NAck(void);				//IIC������ACK�ź�
/**************************************************************************************

**************************************************************************************/

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

//EEPROMоƬ���Ӳ����ʼ����ʼ��
void AT24C256_Init(void)
{
	Init_SWIIC();
}

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
unsigned char RdByte_24C256(uint ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_256)
	{
		IIC_Send_Byte(I2C_EEPROM_ADD);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8); //���͸ߵ�ַ
		//IIC_Wait_Ack();		 
	}else IIC_Send_Byte(I2C_EEPROM_ADD+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(I2C_EEPROM_ADD+1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void WrByte_24C256(unsigned short WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_256)
	{
		IIC_Send_Byte(I2C_EEPROM_ADD);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
		//IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(I2C_EEPROM_ADD+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	SW_EEPROM_Delay_nms(15);	 
}

void WrByte_24C256_Some(unsigned short WriteAddr,unsigned char DataToWrite)
{	
	unsigned char i;
	IIC_Start();	
//	for(i=0;i<4;i++)
	{
	if(1==DwordOrByte_Add_256)
	{
		IIC_Send_Byte(I2C_EEPROM_ADD);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
		//IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(I2C_EEPROM_ADD+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();
	for(i=0;i<16;i++)
	{	
		IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
		IIC_Wait_Ack(); 
	}	
  
}
	IIC_Stop();//����һ��ֹͣ����
	SW_EEPROM_Delay_nms(10);	  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
unsigned char AT24CXX_Read(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
	unsigned int END_Page=0; 
	volatile unsigned char b[20];
  
	END_Page=(ReadAddr+NumToRead)/EEPROM_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//����Χ 
	while(NumToRead)
	{
		*pBuffer=RdByte_24C256(ReadAddr);	
		b[NumToRead] = *pBuffer;
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
unsigned char AT24CXX_Write(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
	unsigned int END_Page=0; 

	END_Page=(WriteAddr+NumToWrite)/EEPROM_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//����Χ 
	while(NumToWrite)
	{
		WrByte_24C256(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
		NumToWrite--;
	}		
	return 0;
}


//��ʼ��ģ������
static void Init_SWIIC(void)
{
	SDA_OUT();				   
	CLK_OUT();
}
//����IIC��ʼ�ź�
static void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	SW_EEPROM_Delay_nus(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	SW_EEPROM_Delay_nus(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
static void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	SW_EEPROM_Delay_nus(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	SW_EEPROM_Delay_nus(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
static unsigned char IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA=1;SW_EEPROM_Delay_nus(1);	   
	IIC_SCL=1;SW_EEPROM_Delay_nus(1);
	SW_EEPROM_Delay_nus(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
static void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=1;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=1;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
static void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		SW_EEPROM_Delay_nus(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		SW_EEPROM_Delay_nus(2); 
		IIC_SCL=0;	
		SW_EEPROM_Delay_nus(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
static unsigned char IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        SW_EEPROM_Delay_nus(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		SW_EEPROM_Delay_nus(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
} 

unsigned char		Check_BlackCardNum(ulong iii)
{
	uint		Addr;
	uchar		i,j;
	uchar		st_data;
	Addr=iii/8;
	i=iii%8;
	st_data=RdByte_24C256(Addr);
	j=1<<i;
	if (st_data & j)
		return	0;
	else
		return	1;
}
//Ӳ���Լ캯��
u8		Check_24C256(uint	Addr)
{
	uchar	st_data;
	st_data=RdByte_24C256(Addr);	
	WrByte_24C256(Addr,0xaa);
	if (RdByte_24C256(Addr)!=0xaa)
		return	1;
	else
	{
		WrByte_24C256(Addr,0x55);
		if (RdByte_24C256(Addr)!=0x55)
			return	1;
		else
		{
			WrByte_24C256(Addr,st_data);
			return	0;
		}
	}	
}
void Clr_24C02(void)
{	ulong ii;
	for (ii=0;ii<512*64;ii++)
	{
			WrByte_24C256(ii,0xff);
	}
}
 