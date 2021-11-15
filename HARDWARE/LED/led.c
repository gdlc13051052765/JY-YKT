#include "led.h"
#include "ExternVoidDef.h"

//////////////////////////////////////////////////////////////////////////////////	 
								  
////////////////////////////////////////////////////////////////////////////////// 	


		    
//LED IO初始化
void LED_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD.2
 	GPIO_SetBits(GPIOC,GPIO_Pin_1);	
	//继电器
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD.2
 	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	
	//wifi_REset
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD.2
 	GPIO_SetBits(GPIOD,GPIO_Pin_14);
	

	//电池电量检测
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	//模拟输入
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD.2
	
	//MP3忙检测
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	//模拟输入
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	
  }

	
	void RsetWifi(void)
	{
		GPIO_InitTypeDef  GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD.2
 	GPIO_SetBits(GPIOD,GPIO_Pin_15);
	}

  void OpedDoor(void)
{
   			JDQOn();
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
				JDQClose();
}
