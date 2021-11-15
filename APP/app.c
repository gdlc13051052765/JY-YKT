#include "includes.h"
#include "ExternVoidDef.h"
#include "beep.h"
#include "mfrc522.h"
#include	"ExternVariableDef.h"

ulong		ReadKeyValue;

OS_STK task_udp_stk[TASK_UDP_STK_SIZE];//����ջ
OS_STK task_led_stk[TASK_UDP_STK_SIZE];//����ջ

//������
void TASK_Start(void *p)
{
	(void)p;
	SysTick_init();
	//����1
	OSTaskCreate(TASK_UDP,(void *)0,
				&task_udp_stk[TASK_UDP_STK_SIZE-1], TASK_UDP_PRIO);
	//����2
	OSTaskCreate(TASK_LED,(void *)0,
				&task_led_stk[TASK_LED_STK_SIZE-1], TASK_LED_PRIO);
	//������
	while(1)
	{
		if (!LoopCount)
			bitUpdateParameter=1;
		ReadSysParameter(1);
		LoopCount++;
		if (bitRecordFull)
		{//��¼����������м�¼�ɼ����������
			if (bitHaveCollectRecord)
			{
				bitHaveCollectRecord=0;
				bitRecordFull=RecordFullDiag();//����
				if (!bitRecordFull)
					DispSumConsumMoney(ConsumMode,0xff,0);
			}
		}
		else
		{//���������
			if (bitNeedDiagRecordFull)
			{
				bitNeedDiagRecordFull=0;
				bitRecordFull=RecordFullDiag();//����
		 	}
		}
		if (bitRecordFull)
			DISP_ErrorSub(SYS_SAVERECORDFULL);
		else
			ConsumSub();//����	
		//	Clr_24C02();
		//UART_ReceiveSub();
		//UART_SendSub();	
		//Clr_WatchDog;
		//ReadMIF_Mode();	
		OSTimeDlyHMSM(0, 0, 0, 10);//���������500ms	
	}
}

//UDP������
void TASK_UDP(void *p_arg)
{	
	(void)p_arg;
	SysTick_init();
	while(1)
	{
		Udp_Serverce();	
		BeepClose();
		//ConsumSub();//����
		OSTimeDlyHMSM(0, 0,0,100);
	}	
}

//LED������
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
		OSTimeDlyHMSM(0, 0, 0, 500);//���������500ms
	   	PCout(2)=0;	
	  // MFRC522Test();
	  //MFRC522_Initializtion();
	  
	   OSTimeDlyHMSM(0, 0, 0, 500);//���������500ms
		
	}
}

