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
* 名    称：void	Beep_init(void)
* 功    能：
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void	Beep_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	

}
/****************************************************************************
* 名    称：void	BeepOn(uchar Num)
* 功    能：
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
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

