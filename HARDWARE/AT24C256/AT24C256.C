#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "iic.H"
#include  "USART.H"
#include <string.h>
#include "includes.h"
#include "char.h"

/*****************************************************************
	AT24C256 对应着板子上的U23，地址线全部选择地，用来存储黑名单参数
****************************************************************/
unsigned char buffer[8];
//器件型号
#define EE24C256 		    
//器件地址及参数 	因为地址线都接地所以地址是a0
#define I2C_EEPROM_ADD    0xA0

#define EEPROM_PageSize	64
#define EEPROM_PageNum	128   
#define DwordOrByte_Add_256  1
#define EEPROM_Total_Size	0x4000
		 		
 //IIC所有操作函数
static void Init_SWIIC(void);                //初始化IIC的IO口				 
static void IIC_Start(void);				//发送IIC开始信号
static void IIC_Stop(void);	  			//发送IIC停止信号
static void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
static unsigned char IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
static unsigned char IIC_Wait_Ack(void); 				//IIC等待ACK信号
static void IIC_Ack(void);					//IIC发送ACK信号
static void IIC_NAck(void);				//IIC不发送ACK信号
/**************************************************************************************

**************************************************************************************/

//驱动中的延时函数，入口参数为0到65535
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

//EEPROM芯片相关硬件初始化初始化
void AT24C256_Init(void)
{
	Init_SWIIC();
}

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
unsigned char RdByte_24C256(uint ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_256)
	{
		IIC_Send_Byte(I2C_EEPROM_ADD);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8); //发送高地址
		//IIC_Wait_Ack();		 
	}else IIC_Send_Byte(I2C_EEPROM_ADD+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(I2C_EEPROM_ADD+1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void WrByte_24C256(unsigned short WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(1==DwordOrByte_Add_256)
	{
		IIC_Send_Byte(I2C_EEPROM_ADD);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
		//IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(I2C_EEPROM_ADD+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
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
		IIC_Send_Byte(I2C_EEPROM_ADD);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
		//IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(I2C_EEPROM_ADD+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack();
	for(i=0;i<16;i++)
	{	
		IIC_Send_Byte(DataToWrite);     //发送字节							   
		IIC_Wait_Ack(); 
	}	
  
}
	IIC_Stop();//产生一个停止条件
	SW_EEPROM_Delay_nms(10);	  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
unsigned char AT24CXX_Read(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
	unsigned int END_Page=0; 
	volatile unsigned char b[20];
  
	END_Page=(ReadAddr+NumToRead)/EEPROM_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//超范围 
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
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
unsigned char AT24CXX_Write(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
	unsigned int END_Page=0; 

	END_Page=(WriteAddr+NumToWrite)/EEPROM_PageSize;
	if(END_Page>EEPROM_PageNum-1)
		return 1;//超范围 
	while(NumToWrite)
	{
		WrByte_24C256(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
		NumToWrite--;
	}		
	return 0;
}


//初始化模拟总线
static void Init_SWIIC(void)
{
	SDA_OUT();				   
	CLK_OUT();
}
//产生IIC起始信号
static void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	SW_EEPROM_Delay_nus(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	SW_EEPROM_Delay_nus(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
static void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	SW_EEPROM_Delay_nus(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	SW_EEPROM_Delay_nus(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
static unsigned char IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
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
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
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
//不产生ACK应答		    
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
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
static void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		SW_EEPROM_Delay_nus(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		SW_EEPROM_Delay_nus(2); 
		IIC_SCL=0;	
		SW_EEPROM_Delay_nus(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
static unsigned char IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
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
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
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
//硬件自检函数
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
 