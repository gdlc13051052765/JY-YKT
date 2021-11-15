#include "includes.h"
#include "ExternVoidDef.h"
#include "beep.h"
#include "mfrc522.h"
#include	"ExternVariableDef.h"

ulong		ReadKeyValue;

OS_STK task_udp_stk[TASK_UDP_STK_SIZE];//定义栈
OS_STK task_led_stk[TASK_UDP_STK_SIZE];//定义栈

//主任务
void TASK_Start(void *p)
{
	(void)p;
	SysTick_init();
	//任务1
	OSTaskCreate(TASK_UDP,(void *)0,
				&task_udp_stk[TASK_UDP_STK_SIZE-1], TASK_UDP_PRIO);
	//任务2
	OSTaskCreate(TASK_LED,(void *)0,
				&task_led_stk[TASK_LED_STK_SIZE-1], TASK_LED_PRIO);
	//主任务
	while(1)
	{
		if (!LoopCount)
			bitUpdateParameter=1;
		ReadSysParameter(1);
		LoopCount++;
		if (bitRecordFull)
		{//记录已满，如果有记录采集，重新诊断
			if (bitHaveCollectRecord)
			{
				bitHaveCollectRecord=0;
				bitRecordFull=RecordFullDiag();//擦除
				if (!bitRecordFull)
					DispSumConsumMoney(ConsumMode,0xff,0);
			}
		}
		else
		{//如果有消费
			if (bitNeedDiagRecordFull)
			{
				bitNeedDiagRecordFull=0;
				bitRecordFull=RecordFullDiag();//擦除
		 	}
		}
		if (bitRecordFull)
			DISP_ErrorSub(SYS_SAVERECORDFULL);
		else
			ConsumSub();//消费	
		//	Clr_24C02();
		//UART_ReceiveSub();
		//UART_SendSub();	
		//Clr_WatchDog;
		//ReadMIF_Mode();	
		OSTimeDlyHMSM(0, 0, 0, 10);//将任务挂起500ms	
	}
}

//UDP任务函数
void TASK_UDP(void *p_arg)
{	
	(void)p_arg;
	SysTick_init();
	while(1)
	{
		Udp_Serverce();	
		BeepClose();
		//ConsumSub();//消费
		OSTimeDlyHMSM(0, 0,0,100);
	}	
}

//LED任务函数
void TASK_LED(void *p_arg)
{	unsigned char buffer[16] = {0x4f,0x00,0x79,0x50,0x50,0x00,0x39,0x77,0x50,0x5e,0x00,0x00,0x00,0x00,0x00,0x00};
	
	(void)p_arg;
	SysTick_init();
	LED_DispDatas_all(buffer);
	while(1)
	{	 
		//CheckPF8563();
		//check_24C02();
		//check_24C512();
		//SPI_Flash_Text();	
		
		PCout(2)=1;		      	  
		OSTimeDlyHMSM(0, 0, 0, 500);//将任务挂起500ms
	   	PCout(2)=0;	
	  // MFRC522Test();
	  //MFRC522_Initializtion();
	  
	   OSTimeDlyHMSM(0, 0, 0, 500);//将任务挂起500ms
		
	}
}

