/**************************************************************************************

**************************************************************************************/

#include "stm32f10x.h"
#include "MyDefine.h"
#include "ExternVariableDef.h"
#include "gpio.H"
#include "beep.H"
#include "spi.h"
#include "delay.h"
#include "timerx.h"
#include <stdio.h>


/****************************************************************************
* ��    �ƣ�void	Beep_init(void)
* ��    �ܣ�
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void	Beep_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	

}
/****************************************************************************
* ��    �ƣ�void	BeepOn(uchar Num)
* ��    �ܣ�
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void	BeepOn(unsigned char num)
{
	//if(!BeepTimes)
	{
		BeepTimes=num;
		BeepDelayTimeCount=0;
	}
	
}

void	JDQOn(void)
{
	PAout(0)=1;
}

void	JDQClose(void)
{	
	PAout(0)=0;
}

void	BeepClose(void)
{	
	PDout(7)=0;
}

