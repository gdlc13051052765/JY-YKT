#ifndef __IIC_H
#define __IIC_H
#include "stdio.h"	


//ģ��IIC�ܽ�����
#define 	SWIIC_SDA_GPIO     		PB
#define 	SWIIC_SDA_BIT      		GPIO_Pin_7
#define 	SWIIC_SDA_Type_Out     	Out_PP
#define 	SWIIC_SDA_Type_In     	IPU


#define 	SWIIC_CLK_GPIO     		PB
#define 	SWIIC_CLK_BIT      		GPIO_Pin_6
#define 	SWIIC_CLK_Type	     	Out_PP
 
//IO��������
#define CLK_OUT() DK_GPIO_Init(SWIIC_CLK_GPIO,SWIIC_CLK_BIT,SWIIC_CLK_Type);
#define SDA_IN()  DK_GPIO_Init(SWIIC_SDA_GPIO,SWIIC_SDA_BIT,SWIIC_SDA_Type_In);
#define SDA_OUT() DK_GPIO_Init(SWIIC_SDA_GPIO,SWIIC_SDA_BIT,SWIIC_SDA_Type_Out);
//IO��������	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //����SDA 

//��ʼ��ģ������
 void Init_IIC(void);
 //����IIC��ʼ�ź�
 void IIC_Start(void);
 //����IICֹͣ�ź�
 void IIC_Stop(void);
 //�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
 unsigned char IIC_Wait_Ack(void);
 //����ACKӦ��
 void IIC_Ack(void)	;
 //������ACKӦ��		    
void IIC_NAck(void);
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
 void IIC_Send_Byte(unsigned char txd);
 //��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
 unsigned char IIC_Read_Byte(unsigned char ack);
  void SW_EEPROM_Delay_nus(unsigned short nus);
  void SW_EEPROM_Delay_nms(unsigned short nms);

#endif


