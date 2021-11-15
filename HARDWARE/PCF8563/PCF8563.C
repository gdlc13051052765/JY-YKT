#include "MyDefine.h"
#include "ExternvoidDef.h"
#include "ExternVariableDef.h"
#include "PCF8563.H"
#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "char.H"
#include "IIC.H"
#include "CalucationFile.H"


#define PCF8563_Read_Add	0xA3
#define PCF8563_Write_Add	0xA2 

//模拟IIC管脚配置
#define 	SWIIC_SDA_GPIO     		PB
#define 	SWIIC_SDA_BIT      		GPIO_Pin_7
#define 	SWIIC_SDA_Type_Out     	Out_PP
#define 	SWIIC_SDA_Type_In     	IPU
#define 	SWIIC_SDA          		PEout(11) 

#define 	SWIIC_CLK_GPIO     		PB
#define 	SWIIC_CLK_BIT      		GPIO_Pin_6
#define 	SWIIC_CLK_Type	     	Out_PP
#define 	SWIIC_CLK          		PEout(10)  
//IO方向设置
#define CLK_OUT() DK_GPIO_Init(SWIIC_CLK_GPIO,SWIIC_CLK_BIT,SWIIC_CLK_Type);
#define SDA_IN()  DK_GPIO_Init(SWIIC_SDA_GPIO,SWIIC_SDA_BIT,SWIIC_SDA_Type_In);
#define SDA_OUT() DK_GPIO_Init(SWIIC_SDA_GPIO,SWIIC_SDA_BIT,SWIIC_SDA_Type_Out);
//IO操作函数	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //输入SDA 

//IIC所有操作函数
static void Init_SWIIC(void);                //初始化IIC的IO口				 
static void IIC_Start(void);				//发送IIC开始信号
static void IIC_Stop(void);	  			//发送IIC停止信号
static void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
static unsigned char IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
static unsigned char IIC_Wait_Ack(void); 				//IIC等待ACK信号
static void IIC_Ack(void);					//IIC发送ACK信号
static void IIC_NAck(void);				//IIC不发送ACK信号
static unsigned char IIC_WriteBuf(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite);
static unsigned char IIC_ReadBuf(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead);
static void IIC_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite);
static unsigned char IIC_ReadOneByte(unsigned short ReadAddr);
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
//相关硬件初始化初始化
void PCF8563_Init(void)
{
	 Init_SWIIC();
}

/*****************************************************************/
/*
********************************************************************/ 
//PCF8563时钟停止指令   i2c_write_n_bytes(unsigned char SLA_W, uint16 ADDR,uint16 N, unsigned char *DAT)
static unsigned char PCF8563_stop(void)
{
 	unsigned char stopcode=0x20;
 	if(0==IIC_WriteBuf(0,&stopcode,1))
		return 0;
 	else return 1;
}
//PCF8563时钟启动指令
static unsigned char PCF8563_start(void)
{
 	unsigned char startcode=0x00;
	if(0==IIC_WriteBuf(0,&startcode,1))
		return 0;
 	else return 1;
}
////中断产生后清除TF标志位
unsigned char PCF8563_ClearINT(void)
{
 	unsigned char temp=0x12;
	if(0==IIC_WriteBuf(1,&temp,1))
		return 0;
 	else return 1;
}

/************************************************************************
PCF8563时钟设置
参数说明：（BCD码）
yy:  年（0x00到0x99）
mm:	 月（0x01到0x12）
dd:	 日（0x01到0x31）
hh:	 时（0x00到0x23）
mi:	 分（0x00到0x59）
ss:	 秒（0x00到0x59）
da:	 星期（0x01到0x07）
************************************************************************/
void Set_Sysdate(unsigned char * ptr)
{
	 unsigned char time[7];
	 time[6]=ptr[0];//年
	 time[5]=ptr[1];//月
	 //time[4]=ptr[2];//星期
	 time[3]=ptr[2];//日
	 time[2]=ptr[3];//时
	 time[1]=ptr[4];//分
	 time[0]=ptr[5];//秒
	 PCF8563_stop();
	IIC_WriteBuf(2,time,7);
	
	 	PCF8563_start();	   
	
 	 
}

/************************************************************************
PCF8563时钟读取
参数说明：（BCD码）
time:	  存放读出时间的数组名
time[6]:  年（0x00到0x99）
time[5]:  月（0x01到0x12）
time[3]:  日（0x01到0x31）
time[2]:  时（0x00到0x23）
time[1]:  分（0x00到0x59）
time[0]:  秒（0x00到0x59）
time[4]:  星期（0x01到0x07）
************************************************************************/
void Read_Sysdate(unsigned char *ptr)
{
	 unsigned char time[7];
	 IIC_ReadBuf(2,time,7);

	ptr[0]=time[6]; //年
    ptr[1]=time[5]&0x1f; //月
    ptr[2]=time[3]&0x3f; //日
    ptr[3]=time[2]&0x3f; //时
    ptr[4]=time[1]&0x7f; //分
	if (time[0]<0x80)
		bitSysTimeDisable=0;
	else
		bitSysTimeDisable=1;
    ptr[5]=time[0]&0x7f; //秒
	ptr[6]=time[4]&7;
	  
}

uchar		DiagTimeString(uchar bbit,uchar * ptr)
{
	uchar	 aa;
	uchar 	 bb;
	uchar	 Max;
	uchar	MaxDay[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	if (!bbit)
	{
		if (BCD_String_Diag(ptr,3))
			return	1;	
		aa=BCDToHex(ptr[1]);
		if (!aa || aa>12)
			return	1;
		Max=MaxDay[aa];
		if (aa==2)
		{
			bb=BCDToHex(ptr[0]);
			if (!(bb%4))
				Max=29;	
		}
		aa=BCDToHex(ptr[2]);
		if (!aa || aa>Max)
			return	1;				
	}
	else
	{
		if (BCD_String_Diag(ptr,3))
			return	1;	
		aa=BCDToHex(ptr[0]);
		if ( aa>23)
			return	1;
		aa=BCDToHex(ptr[1]);
		if ( aa>59)
			return	1;
		aa=BCDToHex(ptr[2]);
		if ( aa>59)
			return	1;
	}
	return  0;
}

unsigned char		CheckPF8563(void)
{
	unsigned char i;
	uchar		bbit;
	for (i=0;i<5;i++)
	{
		Read_Sysdate(SysTimeDatas.TimeString);
		bbit=DiagTimeString(0,SysTimeDatas.TimeString);
		bbit=DiagTimeString(0,SysTimeDatas.TimeString);
		if (!bbit)
			bbit=DiagTimeString(1,SysTimeDatas.TimeString+3);
		if (!bbit)
		{
			if (!bitSysTimeDisable)
			{
				memcpy(SysTimeDatasBak.TimeString,SysTimeDatas.TimeString,6);
				return	0;
			}
		}
	}
	return	1;
}


/**********************************************************************
** PCF8563中断管脚中断服务函数,根据情况改写
*********************************************************************/

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
//在指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
static unsigned char IIC_ReadOneByte(unsigned short ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
    IIC_Start();  
	IIC_Send_Byte(PCF8563_Write_Add);   //发送器件地址PCF8563_Read_Add,写数据 	 
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(PCF8563_Read_Add);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
static void IIC_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
    IIC_Start();  
	IIC_Send_Byte(PCF8563_Write_Add);   //发送器件地址PCF8563_Write_Add,写数据 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	SW_EEPROM_Delay_nms(10);	 
}
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
static unsigned char IIC_ReadBuf(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=IIC_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
	return 0;
}  
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
static unsigned char IIC_WriteBuf(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
	while(NumToWrite--)
	{
		IIC_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}		
	return 0;
}

