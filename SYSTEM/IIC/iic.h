#ifndef __IIC_H
#define __IIC_H
#include "stdio.h"	


//模拟IIC管脚配置
#define 	SWIIC_SDA_GPIO     		PB
#define 	SWIIC_SDA_BIT      		GPIO_Pin_7
#define 	SWIIC_SDA_Type_Out     	Out_PP
#define 	SWIIC_SDA_Type_In     	IPU


#define 	SWIIC_CLK_GPIO     		PB
#define 	SWIIC_CLK_BIT      		GPIO_Pin_6
#define 	SWIIC_CLK_Type	     	Out_PP
 
//IO方向设置
#define CLK_OUT() DK_GPIO_Init(SWIIC_CLK_GPIO,SWIIC_CLK_BIT,SWIIC_CLK_Type);
#define SDA_IN()  DK_GPIO_Init(SWIIC_SDA_GPIO,SWIIC_SDA_BIT,SWIIC_SDA_Type_In);
#define SDA_OUT() DK_GPIO_Init(SWIIC_SDA_GPIO,SWIIC_SDA_BIT,SWIIC_SDA_Type_Out);
//IO操作函数	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //输入SDA 

//初始化模拟总线
 void Init_IIC(void);
 //产生IIC起始信号
 void IIC_Start(void);
 //产生IIC停止信号
 void IIC_Stop(void);
 //等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
 unsigned char IIC_Wait_Ack(void);
 //产生ACK应答
 void IIC_Ack(void)	;
 //不产生ACK应答		    
void IIC_NAck(void);
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
 void IIC_Send_Byte(unsigned char txd);
 //读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
 unsigned char IIC_Read_Byte(unsigned char ack);
  void SW_EEPROM_Delay_nus(unsigned short nus);
  void SW_EEPROM_Delay_nms(unsigned short nms);

#endif


