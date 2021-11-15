
/**************************************************************************************/

#include "stm32f10x.h"
#include "MyDefine.h"
#include "ExternVariableDef.h"
#include "gpio.H"
#include "beep.H"
#include "spi.h"
#include "delay.h"
#include "timerx.h"
#include <stdio.h>



//���Ź��ܽ�����
#define 	WATCH_DOG_GPIO     		PC
#define 	WATCH_DOG_BIT      		GPIO_Pin_0
#define 	WATCH_DOG_Type_Out     	Out_PP

#define 	WATCH_DOG_OUT()  DK_GPIO_Init(WATCH_DOG_GPIO,WATCH_DOG_BIT,WATCH_DOG_Type_Out);

//IO��������	 
#define WATCH_DOG    PCout(0) //
/****************************************************************************
* ��    �ƣ�void	BeepOn(uchar Num)
* ��    �ܣ�
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void	WatchDog_Gpio_init(void)
{
	WATCH_DOG_OUT();
}
//ι������
void Clr_WatchDog(void)
{
	WATCH_DOG = 0;
//	delay_ms(10);
	WATCH_DOG = 1;
}

