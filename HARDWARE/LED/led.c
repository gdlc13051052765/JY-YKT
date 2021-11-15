#include "led.h"
#include "ExternVoidDef.h"

//////////////////////////////////////////////////////////////////////////////////	 
								  
////////////////////////////////////////////////////////////////////////////////// 	


		    
//LED IO��ʼ��
void LED_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD.2
 	GPIO_SetBits(GPIOC,GPIO_Pin_1);	
	//�̵���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD.2
 	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	
	//wifi_REset
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD.2
 	GPIO_SetBits(GPIOD,GPIO_Pin_14);
	

	//��ص������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	//ģ������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD.2
	
	//MP3æ���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 	//ģ������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	
  }

	
	void RsetWifi(void)
	{
		GPIO_InitTypeDef  GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD.2
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
