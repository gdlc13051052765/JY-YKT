
#include "includes.h"
#include "bsp.h"	 
#include "BEEP.h"
#include "mfrc522.h"	
#include "iic.h" 
#include "485.h" 
#include "MyDefine.h"
#include "ExternVariableDef.h"
#include "ExternVoidDef.h" 
#include "NVIC.H"
#include "usart.h"
#include "W5500.h"	
#include "MoNiSPI.H"	
#include "USR-232.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 

//�����ֵĳ�ʼ������	   
			   								  
//////////////////////////////////////////////////////////////////////////////////
int memcmpself(unsigned char * data1,unsigned char *data2,int cnt);
void	SysIC_Test(void);//Ӳ���Լ�
extern  void Disp_Gpio(void);
void setcanciall();
u8 sendATToWifi(u8 * cmd,u8 * whantRec,u8 wantlen);
void Set_IP_Dns(void);
extern void	SetWifiModeSub(void);
extern void RsetWifi(void);
void	SetBuTIECode(void);

void BSP_Init(void)
{	
	u32 ReadKeyValue;
 	u8  i;
	SystemInit();	
	delay_init();	    	 //��ʱ������ʼ��
	RsetWifi();
//	delay_ms(1000);
	COMSEL_NoMode();
//	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x10000);
	DK_USART_Init(9600,1);	 	//
	DK_USART_Init(9600,2);	 	//
	DK_USART_Init(19200,3);	 	//
	DK_NVIC_IRQ_Set(USART3_IRQn,0,0,ENABLE);//Ҫʵ�������жϣ�������������������ȼ�
	DK_USART_OPen_INT(3);//��USART3�Ľ����жϣ�Ȼ���ٴ�������жϹ���
	DK_USART_Close_INT(1);
	DK_USART_Close_INT(2);
	//DK_USART_Close_INT(3);
	Init_IIC();           //ģ��IIC��ʼ��
	SPI_Flash_Init();     //FLASH��ʼ��
	RC522_GPIO_INIT();
	MFRC522_Initializtion();  //RC522��ʼ��
	Disp_Gpio_Init();     //�������ʾGPIO��ʼ�� 					
	Beep_init();//��������ʼ��
	LED_Init();
	TIM3_Int_Init(10,7199);  //���ڰ���ɨ��
	TIM2_Int_Init(10,7199);  //���ڰ���ɨ��
	Init_Serial();//���ڽ������ݵ�һЩָ�������ʼ��
	u485_GPIO_INT();	//485���ƹܽų�ʼ��	
	USART_COMSEL_GPIO_INIT();//���ڵ�4052ѡ��ܽų�ʼ��
	

	Usart2ToPrint();//����2�����ڴ�ӡ��ģʽ��
	WatchDog_Gpio_init();//���Ź��ܽų�ʼ��
	Clr_WatchDog();
	u485_RE;
	memset(LoadProgamReDatas,0,6);
	memset(DownLoadPragamSymbl,0,6);
	Init_Serial();//���ڽ������ݵ�һЩָ�������ʼ��
	
	Clr_WatchDog();
	Usart2ToMP3();
	
	SelWifiMode();
	delay_ms(500);
	//Reset_Wifi();
	//Set_IP_Dns();
	
	///**************************//�����ģʽ���ǵ�Դģʽ***********
	BatModeFlag =PCin(2);
	if(BatModeFlag)
	{
		ChangeLedBright(0x8b); //�޸���������ȵ�
	}
	else
	{
		ChangeLedBright(0x8a); //�޸���������ȵ�
	}
	SysIC_Test();     		//ϵͳӲ�����
	ReadSysParameter(0);//��ʼ��ϵͳ����
	//PlayNum(XP_Start );//��ʼ���ɹ�
	BeepOn(3);
	Disp_AllOn();
//	delay_ms(2000);
//	Clr_WatchDog();
//	delay_ms(2000);
//	Clr_WatchDog();
//	delay_ms(2000);
//	Clr_WatchDog();
//	delay_ms(2000);
//	Clr_WatchDog();
//	Disp_AllOff();
	
	
//	while(1)
//	{
//		u485_TE
//		Clr_WatchDog();
//		USART_Send_Char(3,0xA5);
//	}
	
	LED_DispDatas_all(DispBuffer);
	SetValue();
	delay_ms(1000);
//	//POS��������ʼ��
RsetWifi();
	ReadKeyValue=ScanKeySub3(KEY_ANY);
	if (ReadKeyValue==(KEY_1 | KEY_5 | KEY_9))
	//if (ReadKeyValue==(KEY_7 | KEY_8 | KEY_9))
	{
    SetCommModeSub();//����ͨѶģʽ	 
		SetMainCodeSub();//�ֶ�����վ���
		
	}
	else if (ReadKeyValue==(KEY_0 | KEY_POINT | KEY_ADD))
	{
		RsetWifi();
		//SetWifiModeSub();
		//if(Reset_Wifi_Flag)
			Set_IP_Dns();
	}
	else if (ReadKeyValue==(KEY_4 | KEY_8 | KEY_ADD))
	{
		if(CommModeChar == 1)
		{
			SetNetDatas();//�鿴�������������
		}
		else
		{
		  BeepOn(2);	
		}			
	}		
	else if (ReadKeyValue==(KEY_3 | KEY_5 | KEY_7))
		SetConsumModeEnableSub();//�������ѷ�ʽ����λ		
	else if (ReadKeyValue==(KEY_6 | KEY_8 | KEY_0))
		Clr_PosDatas();
	else if (ReadKeyValue==KEY_ENTER)
	{//�汾��Ϣ��ʾ
		i=0;
		Disp_Ver(i++);
		while (1)
		{
			ReadKeyValue=ScanKeySub(KEY_ANY);
			if (ReadKeyValue)
			{
				if (i<=2)
					Disp_Ver(i++);
				else
					break;
			}
			Clr_WatchDog();
		}
	}else if(ReadKeyValue==(KEY_3 | KEY_6 | KEY_9))
	{
		SetBuTIECode();
	}
	SPI_Configuration();		//W5500 SPI��ʼ������(STM32 SPI1)
	W5500_GPIO_Configuration();	//W5500 GPIO��ʼ������	
	if (CommModeChar ==1 )
	{//TCPIPͨѶ��ʽ
		
		W5500_NVIC_Configuration();
		SPI_Configuration();		//W5500 SPI��ʼ������(STM32 SPI1)
		W5500_GPIO_Configuration();	//W5500 GPIO��ʼ������	
		Load_Net_Parameters();		//װ���������	
		W5500_Hardware_Reset();		//Ӳ����λW5500
		W5500_Initialization();		//W5500��ʼ������			
		memcpy(DispBuffer,"\x00\x39\x5c\x40\x00\x78\x39\x73\x00\x00",10);//Co tCP			
	}
	else
	{
		u485_RE;
		COMSEL_485Mode();//��ʼ������3������485ģʽ��
		DK_USART_Init(19200,3);	 	//
		DK_NVIC_IRQ_Set(USART3_IRQn,0,0,ENABLE);//Ҫʵ�������жϣ�������������������ȼ�
		DK_USART_OPen_INT(3);//��USART3�Ľ����жϣ�Ȼ���ٴ�������жϹ���  
		memcpy(DispBuffer,"\x00\x39\x5c\x40\x00\x66\x7f\x6d\x00\x00",10);//Co 485						
	} 
	
	
	DispMainCode();//��ʾվ���
	Clr_WatchDog();

	InitSaveRecordSub(0);//��ʼ����¼ָ��
	SaveRecordBakSub();//�������Ѽ�¼�ı���
	bitRecordFull=RecordFullDiag();//����
	FindPosConsumCountSub();//�������һ����¼����ˮ��
	//Erase_Entire_Chip();	
	  // PlayNum(Consume_Ok );
	GPIO_SetBits(GPIOD,GPIO_Pin_15);
	
	
	Usart2ToPrint();//����2�����ڴ�ӡ��ģʽ��
	if(CommModeChar == 0)
	{
		COMSEL_485Mode();
	}else if(CommModeChar == 2)
	{
		SelWifiMode();
	}
	delay_ms(100);
	
	if(CommModeChar == 1)
	{
		CommModeChar = 1;
	}else
	{
		CommModeChar = 0;
	}
}

//	Ӳ���Լ�
void	SysIC_Test(void)
{

	uchar bb ,i;
	u8 Buffer[4];
	if (CV522Pcd_ResetState())//���Ի�RC531
	{
		DISP_ErrorSub(SYS_RC500_ERROR);
		PlayNum(SYS_RC500_ERROR);
	
		BeepOn(3);
		while (1);
	}
	if(Check_SPI_FLASH())
	{
		DISP_ErrorSub(SYS_FLASH_ERROR);
		BeepOn(3);
		while (1)
		{
			if(!Check_SPI_FLASH())
			break;
		}
	}
	
	if (Check_24C64(7000))
	{
		
		for(i=0;i<3;i++)
		{
			bb = Check_24C64(7000);
			if(!bb)
				break;
		}
		if(bb)
		{
			DISP_ErrorSub(SYS_24C64_ERROR); 
			BeepOn(3);
			PlayNum(SYS_24C64_ERROR);
			while (1);
		}
	}
	if (Check_24C256(30000))
	{
		for(i=0;i<3;i++)
		{
			bb = Check_24C256(30000);
			if(!bb)
				break;
		}
		if(bb)
		{
			DISP_ErrorSub(SYS_24C256_ERROR);;
			PlayNum(SYS_24C256_ERROR);
			BeepOn(3);
			while (1);
		}
	}
	
	bitStatus8563=0;
	bitPF8563Error=CheckPF8563();
	if (bitStatus8563)
	{
		DISP_ErrorSub(SYS_8563_ERROR);
		PlayNum(SYS_8563_ERROR);
 		BeepOn(3);
		while (1);		 			
	}

	Buffer[0] = 1;
	Buffer[1] = 2;
	Buffer[2] = 3;
	
	//Flash_Write_Bytes(PurseKind_Addr,Buffer,4);
	Flash_Rd_Bytes(PurseKind_Addr,Buffer,4);
	if(!memcmpself(Buffer,DefaultKind,4))
		Flag_NotDefault=1;
	else
		Flag_NotDefault=0;
	//Erase_Entire_Chip();	
}
