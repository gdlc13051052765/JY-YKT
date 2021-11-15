/**********************************************************************************
 * 文件名  ：W5500.c
 * 描述    ：W5500 驱动函数库         
 * 库版本  ：ST_v3.5

 * 淘宝    ：http://yixindianzikeji.taobao.com/
**********************************************************************************/

#include "stm32f10x.h"
#include "stm32f10x_spi.h"	
#include "stm32f10x_exti.h"	
#include "USR-232.h"
#include "delay.h"

extern u8 sendATToWifi(u8 * cmd,u8 * whantRec,u8 wantlen);
//wifi模块复位
void Reset_Wifi()
{
	GPIO_ResetBits(GPIOD,GPIO_Pin_15);
	delay_ms(100000);
	GPIO_SetBits(GPIOD,GPIO_Pin_15);
}

void Set_IP_Dns(void)
{

		GPIO_ResetBits(GPIOD,GPIO_Pin_15);
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
	delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
	delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
	delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
	delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		delay_ms(3000);
		Clr_WatchDog();
		GPIO_SetBits(GPIOD,GPIO_Pin_15);
	NVIC_SystemReset();
		while(1);
		

}