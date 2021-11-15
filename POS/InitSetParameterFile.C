#include "MyDefine.h"
#include "ExternVariableDef.h"
#include "ExternVoidDef.h"
#include "char.h"
#include "stm32f10x_flash.h"
#include "includes.h"

 #define MainCodeAddress    0x08061000

	
void	SetCommModeSub(void);
void	SetConsumModeEnableSub(void);//设置消费方式允许位
void	LookSysPatameter(void);//察看系统参数,0-波特率,底金，日限额，使用的扇区号，版本号，消费记录条数等
void	SetSysDateTime(ulong);//设置日期时间
void	SetConsumMode(ulong);//设置消费模式
void	SetSysPrince(uchar,ulong);//设置单价
void	LookRecordDatas(ulong);//察看最近的消费记录	
void	LookRecordNum(uchar Sort);//察看消费记录条///
uchar	SelectDate(uchar * ptr);//选择消费日期
void	ChgUlongToLongDispBuffer(ulong iii,uchar * ptr);
//void	ChgInputToLedBuffer(uchar LedX,uchar * ptr,uchar Num)//将输入的字符转换为显示字符


void SetRemoteIp(void) //手动设置是否开启远端IP绑定
{

	uchar		bbit=1;
	uchar	st_data;
	ulong	InputKeyValue;
	uint16_t tempbuf[2];
	st_data=RemoteIPFlag;
	
	while (1)
	{

		if (bbit)
		{
			bbit=0;
			if (st_data)
				memcpy(DispBuffer,"\x50\x79\x3f\x78\x00\x00\x3f\x73\x79\x54",10);//Co 485
			else
			{
				memcpy(DispBuffer,"\x50\x79\x3f\x78\x00\x39\x38\x3f\x6d\x79",10);//Co 485	
			}
			LED_DispDatas_all(DispBuffer);
		}
		InputKeyValue=ScanKeySub(KEY_ESC|KEY_ENTER|KEY_ADD);
		if (InputKeyValue==KEY_ESC)
			return;
		if (InputKeyValue==KEY_ENTER)
			break;
		if (InputKeyValue==KEY_ADD)
		{
			st_data=(st_data+1)%2;
			bbit=1;
		}
		Clr_WatchDog();		
	}
	CommModeChar=st_data;
	InputBuffer[0]=0xa0;
	InputBuffer[1]=st_data;
	InputBuffer[2]=CalCheckSum(InputBuffer,2);
	WrBytesToAT24C64(RemoteFlagAddr,InputBuffer,3);
	
	tempbuf[0] = InputBuffer[1];
	writeSysParameterToFlash(RemoteFlagFlashAdd,tempbuf,2);
	//delay_ms(20);
}
void SetBeepOn(void) //手动设置蜂鸣器关比打开
{

	uchar		bbit=1;
	uchar	st_data;
	ulong	InputKeyValue;
	st_data = 1;
	while (1)
	{

		if (bbit)
		{
			bbit=0;
			if (st_data)
			{
				memcpy(DispBuffer,"\x7f\x79\x79\x73\x00\x00\x00\x00\x3f\x73",10);//Beep  	ON
				BeepFlag = 0;
			}
			else
			{
				memcpy(DispBuffer,"\x7f\x79\x79\x73\x00\x00\x00\x3f\x71\x71",10);//Beep   Off	
				BeepFlag = 1;
			}
			LED_DispDatas_all(DispBuffer);
		}
		InputKeyValue=ScanKeySub(KEY_ESC|KEY_ENTER|KEY_ADD);
		if (InputKeyValue==KEY_ESC)
			return;
		if (InputKeyValue==KEY_ENTER)
			break;
		if (InputKeyValue==KEY_ADD)
		{
			st_data=(st_data+1)%2;
			bbit=1;
		}
		Clr_WatchDog();		
	}
}

void	SetMainCodeSub(void)//手动设置站点号
{
	ulong	ReadKeyValue;
	uchar	Count=0;
	uchar	Buffer[5];
	uchar	st_data;
	uchar	LedX;
	uchar	i;
	ulong	iii;
	uint MainCodeBak;
	
	LedX=5;
	Count=5;	
	ChgIntToStrings(MainCode,Buffer);
	DispBuffer[0]=0x77;//A
	DispBuffer[1]=0x5e;//d
	DispBuffer[2]=0x5e;//d
	DispBuffer[3]=0x50;//r
	for (i=0;i<5;i++)
		DispBuffer[LedX+i]=Disp0_9String[Buffer[i]];
	LED_DispDatas_all(DispBuffer);
	while (1)
	{
		LED_NumFalsh_Disp(LedX+5-Count,1,0x10000);
		ReadKeyValue=ScanKeySub(KEY_0_9 |KEY_ESC | KEY_ENTER);
		if (ReadKeyValue==KEY_ESC)
			return;
		else if (ReadKeyValue==KEY_ENTER)
			break;
		else if (ReadKeyValue & KEY_0_9)
		{
			if (Count)
			{
				st_data=ChgKeyValueToChar(ReadKeyValue);
				Buffer[5-Count]=st_data;							
				DispBuffer[LedX+5-Count]=Disp0_9String[st_data];
				Forbidden=1;
				LED_OneByte_Disp(LedX+5-Count,DispBuffer[LedX+5-Count]);
				Forbidden=0;
				Count--;

				if (!Count)					
					Count=5;
			}
		}
		Clr_WatchDog();
	}
	iii=ChgStringsToInt(Buffer);
	st_data=1;
	if (iii<65536 && iii)
		st_data=0;
	if (!st_data)
	{
		MainCode=(uint)ChgStringsToInt(Buffer);
		bitSetDateEnable=1;
		Buffer[0]=0xa0;
		Buffer[1]=* (uchar *)&MainCode;
		Buffer[2]=* (1+ (uchar *)&MainCode);
		Buffer[3]=CalCheckSum(Buffer,3);
		WrBytesToAT24C64(MainCode_Addr,Buffer,4);
		{
//			FLASH_Unlock();//解锁
//			MainCodeBak=(*(vu16*) 0x8061000);
//			FLASH_Lock();//锁定
//			if((MainCodeBak!=MainCode)||(!MainCode))
//			{
//				FLASH_Unlock();//解锁
//				FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
//				FLASH_ErasePage(MainCodeAddress);//擦除整页
//				FLASH_ProgramHalfWord(MainCodeAddress,MainCode);
//				FLASH_Lock();//锁定
//			}			
			writeSysParameterToFlash(MainCodeAddress,&MainCode,2);	
		//	MainCode = 0;
		}

		FLASH_Unlock();//解锁
		MainCode=(*(vu16*) 0x8061000);
		FLASH_Lock();//锁定

//		BeepOn(1);
	}
	else
	{
	//	BeepOn(3);
//		BeepClose();
	}
	i=1;
	DispBuffer[0]=0x73;//P
	DispBuffer[1]=0x77;//A
	DispBuffer[2]=0x6d;//S
	DispBuffer[3]=0x6d;//S
	DispBuffer[4]=0x00;
	DispBuffer[9]=0x00;
	while (1)	
	{
		ReadKeyValue=ScanKeySub(KEY_ADD|KEY_ESC | KEY_ENTER);
		if (ReadKeyValue==KEY_ESC)
			return;
		else if (ReadKeyValue==KEY_ENTER)
			break;
		else if (ReadKeyValue==KEY_ADD)
		{
			if(bitPinEnable)
			{
				bitPinEnable = 0;	
			}
			else
			{
				bitPinEnable = 1;
			}
			i=1;		
		}
		if (i)
		{
			i=0;
			if (!bitPinEnable)
			{
				DispBuffer[5]=0x39;//C
				DispBuffer[6]=0x38;//L
				DispBuffer[7]=0x3f;//O
				DispBuffer[8]=0x6d;//S.	
				LED_DispDatas_all(DispBuffer);
			}
			else
			{
				DispBuffer[5]=0x3f;//O
				DispBuffer[6]=0x73;//P
				DispBuffer[7]=0x79;//E
				DispBuffer[8]=0x37;//N.
				LED_DispDatas_all(DispBuffer);
			}
		}
		Clr_WatchDog();	
	}
	Buffer[0]=0xa0;	
	if (bitPinEnable)
		Buffer[1]=0x55;
	else
		Buffer[1]=0;
	Buffer[2]=CalCheckSum(Buffer,2);
	WrBytesToAT24C64(PinEnable_Addr,Buffer,3);
	bitUpdateParameter=1;
	ReadSysParameter(1);	
}
void	SetCommModeSub(void)
{
	uchar		bbit=1;
	uchar	st_data;
	ulong	InputKeyValue;
	uint16_t tempbuf[2];
	
	st_data=CommModeChar;
	
	while (1)
	{

		if (bbit)
		{
			bbit=0;
			if (!st_data)
				memcpy(DispBuffer,"\x00\x39\x5c\x40\x00\x66\x7f\x6d\x00\x00",10);//Co 485
			else if(st_data == 1)
			{
				//if(st_data==1)
				{
					memcpy(DispBuffer,"\x00\x39\x5c\x40\x00\x78\x39\x73\x00\x00",10);//Co tCP
				}
				
			}else
			{
				memcpy(DispBuffer,"\x00\x39\x5c\x40\x00\x3E\x30\x30\x71\x00",10);//Co tCP
			}
			LED_DispDatas_all(DispBuffer);
		}
		InputKeyValue=ScanKeySub(KEY_ESC|KEY_ENTER|KEY_ADD);
		if (InputKeyValue==KEY_ESC)
			return;
		if (InputKeyValue==KEY_ENTER)
			break;
		if (InputKeyValue==KEY_ADD)
		{
			st_data=(st_data+1)%3;
			bbit=1;
		}
		Clr_WatchDog();		
	}
	CommModeChar=st_data;
	InputBuffer[0]=0xa0;
	InputBuffer[1]=st_data;
	InputBuffer[2]=CalCheckSum(InputBuffer,2);
	WrBytesToAT24C64(CommModeChar_Addr,InputBuffer,3);
	tempbuf[0] = InputBuffer[1];
	writeSysParameterToFlash(CommModeCharFlashAdd,tempbuf,2);
	delay_ms(20);
}


void	DispBuTieBits(uchar bbit)	
{
	if(!bbit)
		//memcpy(DispBuffer,"\x76\x77\x3e\x79\x00\x00\x37\x79\x78\x00",10);//have net
		memcpy(DispBuffer,"\x5c\x37\x00\x00\x00\x00\x00\x00\x00\x00",10);//on
	else
		//memcpy(DispBuffer,"\x00\x37\x5c\x00\x00\x00\x37\x79\x78\x00",10);//no net
		memcpy(DispBuffer,"\x5c\x71\x71\x00\x00\x00\x00\x00\x00\x00",10);//no net
	LED_DispDatas_all(DispBuffer);
}

uchar 	ConsumStaCodeSymbl[4]={0xc1,0xc2,0xc3,0xc4};

void	SetBuTIECode(void)//设置是否允许补贴
{
	uchar		Buffer[4];
	ulong	Key_Long;
	uchar	bbit;
	bbit=canbutie;
	DispBuTieBits(bbit);
	while (1)
	{
		LED_NumFalsh_Disp(0,4,0xa000);
		Key_Long=ScanKeySub(KEY_ESC|KEY_ENTER|KEY_ADD);
		if (Key_Long==KEY_ESC)
			return;
		if (Key_Long==KEY_ENTER)
			break;
		if (Key_Long==KEY_ADD)
		{
			if(bbit)
			{
				bbit =0;
			}
			else
				bbit =1;
			DispBuTieBits(bbit);	
		}
		Clr_WatchDog();
	}
	if (bbit)
		memset(Buffer,0xff,4);
	else
		memcpy(Buffer,ConsumStaCodeSymbl,4);
	WrBytesToAT24C64(CanBuTie_Addr,Buffer,4);
	
	BeepOn(2);
	bitUpdateParameter=1;
	ReadSysParameter(0);
}

void	SetWifiModeSub(void)
{
	uchar		bbit=1;
	uchar	st_data;
	ulong	InputKeyValue;
	//st_data=CommModeChar;
	st_data = 0;
	while (1)
	{

		if (bbit)
		{
			bbit=0;
			if (st_data)
				memcpy(DispBuffer,"\x50\x79\x6d\x79\x78\x00\x3f\x73\x79\x54",10);//OPEN
			else
			{				
					memcpy(DispBuffer,"\x50\x79\x6d\x79\x78\x39\x38\x3f\x6d\x79",10);//CLOSE				
			}
			LED_DispDatas_all(DispBuffer);
		}
		InputKeyValue=ScanKeySub(KEY_ESC|KEY_ENTER|KEY_ADD);
		if (InputKeyValue==KEY_ESC)
			return;
		if (InputKeyValue==KEY_ENTER)
			break;
		if (InputKeyValue==KEY_ADD)
		{
			st_data=(st_data+1)%2;
			bbit=1;
		}
		Clr_WatchDog();		
	}
	Reset_Wifi_Flag=st_data;

	delay_ms(20);
}

void	SetConsumModeEnableSub(void)//设置消费方式允许位
{
	uchar	Buffer[4];
	ulong	Key_Long;
	uchar	i;
	uchar	st_data=0;
	uchar	Num=0;
	uint16_t tempbuf[2];
	DispConsumModeEnable(0,ConsumModeEnable);
	ConsumModeEnable &= ~(1<<CONSUM_SIMPLE);
	while (1)
	{
		Key_Long=ScanKeySub(KEY_ESC|KEY_ENTER|KEY_SIMPLE | KEY_NUM | KEY_RATION| KEY_MONEY|KEY_ADD);
		if (Key_Long)
		{
			if (Key_Long==KEY_ESC)
				return;
			if (Key_Long==KEY_ENTER)
			{
				if (!Num)
					Num=1;
				else
					break;
				DispConsumModeEnable(Num,ConsumModeEnable);	 
			}
			else
			{
				if (Key_Long==KEY_SIMPLE)
					i=CONSUM_SIMPLE;
			  	else if (Key_Long==KEY_NUM)
					i=CONSUM_NUM;
			  	else if (Key_Long==KEY_RATION)
					i=CONSUM_RATION;
				else  if (Key_Long==KEY_MONEY)
					i=CONSUM_MONEY;
				else if (Key_Long==KEY_ADD)
				{
					if (!Num)
						i=4;
					else
						i=5;
				}
				st_data=1<<i;
				if (ConsumModeEnable & st_data)
					ConsumModeEnable&=~st_data;
				else
					ConsumModeEnable|=st_data;
				DispConsumModeEnable(Num,ConsumModeEnable);
			}
		}
		//485TCP模式的功能选择
		if(CommModeChar == 1)
		{
//			Udp_Serverce();	//处理uip事件，必须插入到用户程序的循环体中 	
		}
		else
		{
		 	UART_ReceiveSub();
			UART_SendSub();			
		}	 
		Clr_WatchDog();		
	}
	
	ConsumModeEnable &= ~(1<<CONSUM_SIMPLE);
	
	Buffer[0]=0xa0;
	Buffer[1]=ConsumModeEnable;
	Buffer[2]=CalCheckSum(Buffer,2);
	tempbuf[0] = (Buffer[1]);
	writeSysParameterToFlash(ConsumModeEnableFlashAdd,tempbuf,2);
	WrBytesToAT24C64(ConsumModeEnable_Addr,Buffer,3);
	BeepOn(1);

	//==============================
	i=1;
	DispBuffer[0]=0x73;//P
	DispBuffer[1]=0x77;//A
	DispBuffer[2]=0x6d;//S
	DispBuffer[3]=0x6d;//S
	DispBuffer[4]=0x00;
	DispBuffer[9]=0x00;
	while (1)	
	{
		Key_Long=ScanKeySub(KEY_ADD|KEY_ESC | KEY_ENTER);
		if (Key_Long==KEY_ESC)
			return;
		else if (Key_Long==KEY_ENTER)
		{
			bitSetDateEnable=1;
			break;
		}
		else if (Key_Long==KEY_ADD)
		{
			if(bitPinEnable)
			{
				bitPinEnable = 0;	
			}
			else
			{
				bitPinEnable = 1;
			}
			i=1;		
		}
		if (i)
		{
			i=0;
			if (!bitPinEnable)
			{
				DispBuffer[5]=0x39;//C
				DispBuffer[6]=0x38;//L
				DispBuffer[7]=0x3f;//O
				DispBuffer[8]=0x6d;//S.	
				LED_DispDatas_all(DispBuffer);
			}
			else
			{
				DispBuffer[5]=0x3f;//O
				DispBuffer[6]=0x73;//P
				DispBuffer[7]=0x79;//E
				DispBuffer[8]=0x37;//N.
				LED_DispDatas_all(DispBuffer);
			}
		}
		Clr_WatchDog();
	}
	Buffer[0]=0xa0;	
	if (bitPinEnable)
		Buffer[1]=0x55;
	else
		Buffer[1]=0;
	Buffer[2]=CalCheckSum(Buffer,2);
	WrBytesToAT24C64(PinEnable_Addr,Buffer,3);
	bitUpdateParameter=1;
	ReadSysParameter(0);	

}

void	LookSysPatameter(void )//察看系统参数
{
	if (!bitDispFlash)
		return;
	bitDispFlash=0;
	if (InputCase==0x50)
		LookRecordNum(0);
	else if (InputCase==0x51)
		LookRecordNum(1);
	else if (InputCase==0x52)
	{
		if (!CommModeChar || CommModeChar == 2)
			memcpy(DispBuffer,"\x7c\x77\x3e\x5e\x00\x06\x6f\x5b\x3f\x3f",10);
		else//TCPIP模式
			memcpy(DispBuffer,"\x00\x39\x5c\x40\x00\x78\x39\x73\x00\x00",10);
		LED_DispDatas_all(DispBuffer);
		
	}
	else if (InputCase==0x53)
		LookRecordNum(3);
	else if (InputCase==0x54)
		LookRecordNum(4);
	else if (InputCase==0x55)
		LookRecordNum(5);
	else if (InputCase==0x56)
		DispMinBalance();//显示底金
	else if (InputCase==0x57)				
		DispMainCode();//站点
	else if (InputCase==0x58)
	{
		//sec--02
		memcpy(DispBuffer,"\x6d\x79\x39\x40\x40\x00\x00\x00\x00\x00",10);
		DispBuffer[5]=Disp0_9String[CardSector/10];
		DispBuffer[6]=Disp0_9String[CardSector%10];

		LED_DispDatas_all(DispBuffer);
	}
	else if (InputCase==0x59)
		Disp_Ver(0);
	else if (InputCase==0x5a)
		Disp_Ver(1);
	else if (InputCase==0x5b)
		Disp_Ver(2);
	else if (InputCase==0x5c)
		InputCase++;
	else if (InputCase==0x5d)
		DispNetDatas(LedTcpIndex-1);
	else if (InputCase==0x5e)
		LookRecordNum(2);	
}
void	SetSysDateTime(ulong InputKeyValue)//设置日期时间
{
	uchar		bbit=0;	
	uchar	st_data;
	uchar	status;
	uchar	Position1[7]={0,2,3,5,6,8,9};
	if (InputCase==0x20)
	{
		bitHaveInputDatas=0;
		InputCount=1;
		memset(InputBuffer,0,8);
		InputCase=0x21;//时间
		bbit=1;
	}
	if (InputKeyValue==KEY_ENTER)
	{
		if ( bitSetDateEnable )
		{
			bitHaveInputDatas=0;
			Read_Sysdate(SysTimeDatas.TimeString);
			status=1;
			if (InputCase==0x21)
			{
				if (!DiagTimeString(1,InputBuffer))
					status=0;
		  	}
			else
			{
				if (!DiagTimeString(0,InputBuffer))
					status=0;				
			}
			if (!status)
			{
				memcpy(SysTimeDatas.TimeString+(0x22-InputCase)* 3,InputBuffer,3);
				Set_Sysdate(SysTimeDatas.TimeString);
				BeepOn(1);
				if (InputCase==0x21)
					InputCase=0x22;
				else
				{
					bitSetDateEnable=0;
					InputCase=0;
					return;
				}
			}
			else
			{
				if (InputCase==0x21)
					InputCase=0x20;
				else
					InputCase=0x22;					
				BeepOn(3);
			}
		}
		else
		{
			if (InputCase==0x21)
				InputCase=0x22;
			else
			{
				bitSetDateEnable=0;
				InputCase=0;
				return;
			}
		}
		memcpy(InputBuffer,SysTimeDatas.TimeString+(0x22-InputCase)* 3,3);	
		InputCount=1;
		bbit=1;
	}
	else 
	{
		if (bitSetDateEnable && (InputKeyValue & KEY_0_9))
		{
			bbit=1;
			bitHaveInputDatas=1;
			st_data=ChgKeyValueToChar(InputKeyValue);
			if (InputCount%2)
				InputBuffer[(InputCount-1)/2]=( InputBuffer[(InputCount-1)/2]&0x0f )+(st_data<<4);
			else
				InputBuffer[(InputCount-1)/2]=(InputBuffer[(InputCount-1)/2]&0xf0)+st_data;
			InputCount++;
			InputCount=InputCount%7;
			if (!InputCount)
				InputCount=1;
		}
	}
	if (!bitHaveInputDatas)
	{
		bbit=1;
		//bitHaveInputDatas=1;
		memcpy(InputBuffer,SysTimeDatas.TimeString+(0x22-InputCase)* 3,3);
	}
	if (bbit)
	{	
	   	if (InputCase==0x21)
			DispTime(InputBuffer);
	  	else
			DispDate(InputBuffer);
	}
	if(bitSetDateEnable)
		LED_NumFalsh_Disp(Position1[InputCount],1,0x05);	 //
}

void	SetConsumMode(ulong	lll)
{
	uchar	i;
	if (lll==KEY_SIMPLE)
		i=CONSUM_SIMPLE;
	else if (lll==KEY_NUM)
		i=CONSUM_NUM;
	else if (lll==KEY_RATION)
		i=CONSUM_RATION;
	else
		i=CONSUM_MONEY;
	if (ConsumModeEnable & (1<<i))
		ConsumMode=i;
	else
		BeepOn(3);
}

void	SetSysPrince(uchar bitMode,ulong	 InputKeyValue)//设置简易\菜号及单价
{
	uchar		bbit=0;
	ulong	Addr;
	uchar	st_data;
	uchar	RdBuffer[5];
	ulong	iii;
	uchar	aaa[8];
	DispBuffer[10]=0x10;
	Forbidden=1;
	LED_DispDatas(1,DispBuffer);//显示菜单
	Forbidden=0;
	if (!bitMode)
	{//菜单价格
		if (InputCase==0x30)
		{
			bbit=1;
			InputCount=0;
			InputNum=0;
			bitHaveInputDatas=0;
			InputCase=0x31;
		}
	}
	else
	{//简易消费价格
		if (InputCase==0x40)
		{
			bbit=1;
			InputCount=0;
			InputNum=0;
			bitHaveInputDatas=0;
			InputCase=0x41;
		}
	}
	if (InputKeyValue==KEY_ENTER)
	{
		InputCase|=2;
		if (bitHaveInputDatas)
		{
			bitHaveInputDatas=0;
			if (!bitMode)
				Addr=MenuPrince_Addr;
			else
				Addr=SimplePrince_Addr;
			iii=ChgInputToUlong(InputBuffer,6);
			ChgUlongToBCDString(iii,RdBuffer,3);
			RdBuffer[3]=CalCheckSum(RdBuffer,3);
			WrBytesToAT24C64(Addr+InputNum*4,RdBuffer,4);
			BeepOn(1);
		}
		InputNum++;
		if (InputNum>=InputMaxNum)
			InputNum=0;	
		bbit=1;
	}
	if (bbit)
	{
		if (!bitMode)
		{//菜号方式
			Addr=MenuPrince_Addr;
			InputMaxNum=0;
			for (st_data=0;st_data<240;st_data++)
			{
				RdBytesFromAT24C64(Addr,RdBuffer,4);
				if (!BytesCheckSum(RdBuffer,4) && !BCD_String_Diag(RdBuffer,3))
					InputMaxNum++;
				else
					break;	
				Addr+=4;	
			}
			Addr=MenuPrince_Addr;
			DispBuffer[0]=0xd4;//n.
			DispBuffer[1]=Disp0_9String[InputNum/10];
			DispBuffer[2]=Disp0_9String[InputNum%10];
		}
		else
		{//简易方式
			Addr=SimplePrince_Addr;

			DispBuffer[0]=Disp0_9String[(InputNum+1)/10];
			DispBuffer[1]=Disp0_9String[(InputNum+1)%10];
			DispBuffer[2]=0x0f;//]
			InputMaxNum=12;
		}
		if (InputNum>InputMaxNum )
			InputNum=0;
		RdBytesFromAT24C64(Addr+InputNum*4,RdBuffer,4);
		if (!BytesCheckSum(RdBuffer,4) && !BCD_String_Diag(RdBuffer,3))
			memcpy(InputBuffer,RdBuffer,3);
	  	else
			memset(InputBuffer,0,3);
		iii=ChgBCDStringToUlong(InputBuffer,3);	
		ChgUlongToLongDispBuffer(iii,aaa);
		DispBuffer[3]=0;	
		memcpy(DispBuffer+4,aaa+2,6);
		bbit=0;
		InputCount=0;
		bitInPutPoint=0;
		memset(InputBuffer,0xff,6);	
		bitHaveInputDatas=0;
		LED_DispDatas_all(DispBuffer);
	}
	if (InputNum<=InputMaxNum)
	{
		if (InputKeyValue & KEY_0_9 )
		{
			InputCase|=2;
			if (InputCount<6)
			{
				st_data=0;
				if (bitInPutPoint && InputCount>=3 )
				{//小数点后保留2为
					if ( InputBuffer[InputCount-3]&0x80 )
						st_data=1;
				}
				if (!st_data)
				{
					bitHaveInputDatas=1;
					st_data=ChgKeyValueToChar(InputKeyValue);	
					InputBuffer[InputCount++]=st_data;
	 				if (InputCount==2 && !bitInPutPoint)
					{
						if (!InputBuffer[0])
						{
							memset(InputBuffer,0xff,6);
							InputBuffer[0]=st_data;
							InputCount=1;
						}
					}
					if (InputCount>=5 && !bitInPutPoint)
					{//整数保留4位
						memset(RdBuffer,0xff,6);
						memcpy(RdBuffer,InputBuffer+1,4);
						memcpy(InputBuffer,RdBuffer,6);
						InputCount=4;
						FormatBuffer(InputBuffer,&InputCount);
					}
				}
				DispBuffer[4]=0;
				ChgInputToLedBuffer(4,InputBuffer,6);
			}
			else
				BeepOn(3);
		}
		else if (InputKeyValue==KEY_POINT)
		{
			InputCase|=2;
			if (!bitInPutPoint)
			{
				bitInPutPoint=1;
				if (InputCount)
					InputBuffer[InputCount-1]|=0x80;
				else
				{
					InputBuffer[0]=0x80;	
					InputCount=1;
				}
				ChgInputToLedBuffer(4,InputBuffer,6);
			}
		}
	}
}


void	LookRecordDatas(ulong InputKeyValue)//察看最近的消费记录
{
	uchar		bbit=0;
//	uint	ii=0;
	ulong	Index=0;
	ulong	iii;
	uchar	Buffer[32];
	uchar	PPage,i;
	ulong	Addr;
//	uint	Num=0;
	uchar	aaa[8];
	if (!RecordSum)
	{
		if (InputCase==0x60)
		{
			BeepOn(3);
			DispBuffer[0]=0; 
			DispBuffer[1]=0; 		
			DispBuffer[2]=0x37;//N
			DispBuffer[3]=0x3f;//0
			DispBuffer[4]=0;//
			DispBuffer[5]=0x50;//r
			DispBuffer[6]=0x79;//E
			DispBuffer[7]=0x39;//c
			DispBuffer[8]=0; 
			DispBuffer[9]=0; 			 
		}
		InputCase=0x61;
		LED_DispDatas_all(DispBuffer);
		return;
	}
	if (InputCase==0x60)
	{
		bbit=1;
		InputBuffer[0]=0;
		InputBuffer[1]=0;
		InputCase=0x61;
	}
	if (InputKeyValue==KEY_ADD)
	{
		bbit=1;
		InputBuffer[1]++;
		InputBuffer[1]=InputBuffer[1]%5;
	}
	else if (InputKeyValue==KEY_ENTER)
	{
		InputBuffer[0]++;
		InputBuffer[1]=0;
		bbit=1;
	}
	if (!bbit)
		return;
	if (InputBuffer[0]>=RecordSum)
		InputBuffer[0]=0;
	if (!SaveRecordIndex)
		Index=MAXRECORD_NUM-1;
	else
		Index=SaveRecordIndex-1;		
	for (i=0;i<InputBuffer[0];i++)
	{
		if (Index)
			Index=Index-1;
	   	else
			Index=MAXRECORD_NUM-1;		
	}
	Addr=(ulong)Index*RECORD_SIZE;
//	PPage=*(1+(uchar *)&iii);
//	memcpy((uchar *)&Addr,(2+(uchar *)&iii),2);
	Flash_Rd_Bytes(Addr,Buffer,32);
	switch(InputBuffer[1])
	{
		case	0://显示纪录序号
			if (!(Buffer[1]&0x80))
			{
				DispBuffer[0]=0x39;//[
				DispBuffer[4]=0x0f;//]
			}
			else
			{
				DispBuffer[0]=0x40;//[
				DispBuffer[4]=0x40;//]
			}
			DispBuffer[1]=Disp0_9String[InputBuffer[0]/100];
			PPage=InputBuffer[0]%100;
			DispBuffer[2]=Disp0_9String[PPage/10];
			DispBuffer[3]=Disp0_9String[PPage%10];

			memset(DispBuffer+5,0,5);
			break;
		case	1://显示卡号
			iii=ChgBCDStringToUlong(Buffer+8,3);
			ChgUlongToLongDispBuffer(iii,aaa);
			aaa[5]&=0x7f;
			DispBuffer[0]=0x54;//n.
			DispBuffer[1]=0x5c;//0
			DispBuffer[2]=0x40;//-
		 	memcpy(DispBuffer+3,aaa+2,6);
			DispBuffer[9]=0;
			break;
   		case	2://显示月日时分
			ChgRecordDatasToTime(Buffer+22,aaa);
			DispBuffer[0]=0x78;//t
			DispBuffer[1]=0x40;//t
			PPage=BCDToHex(aaa[1]);
			DispBuffer[2]=Disp0_9String[PPage/10];
			DispBuffer[3]=Disp0_9String[PPage%10];//月
			DispBuffer[3]|=0x80;
			PPage=BCDToHex(aaa[2]);
			DispBuffer[4]=Disp0_9String[PPage/10];
			DispBuffer[5]=Disp0_9String[PPage%10];//日
			DispBuffer[5]|=0x80;
			PPage=BCDToHex(aaa[3]);
			DispBuffer[6]=Disp0_9String[PPage/10];
			DispBuffer[7]=Disp0_9String[PPage%10];//时
			DispBuffer[7]|=0x80;
			PPage=BCDToHex(aaa[4]);
			DispBuffer[8]=Disp0_9String[PPage/10];
			DispBuffer[9]=Disp0_9String[PPage%10];//分
			break;
		case	3://消费额
			iii=ChgBCDStringToUlong(Buffer+15,3);
			DispBuffer[0]=0x39;//C
			DispBuffer[1]=0x5c; 
			DispBuffer[2]=0x40; 
			ChgUlongToLongDispBuffer(iii,aaa);
			DispBuffer[3]=0x40;//.
			memcpy(DispBuffer+4,aaa+2,6);
			break;
		case	4://新余额
			DispBuffer[0]=0x7c;//B
			DispBuffer[1]=0x40; 
			iii=ChgBCDStringToUlong(Buffer+18,4);
			ChgUlongToLongDispBuffer(iii,aaa);
			DispBuffer[1]=0x40;//.
			memcpy(DispBuffer+2,aaa,8);
			break;
		default:
			break;
	}
	LED_DispDatas_all(DispBuffer);
}

void	ChgInputToLedBuffer(uchar LedX,uchar * ptr,uchar Num)//将输入的字符转换为显示字符
{
	uchar	st_data,i;
	uchar	j=0;
	for (i=0;i<Num;i++)
	{
		st_data=ptr[i];
	 	if (st_data==0xff)
			j++;
	}
	memset(DispBuffer+LedX,0,Num);
	if (j<Num)
	{
		for (i=0;i<(Num-j);i++)
		{
			st_data=ptr[i];
			DispBuffer[LedX+j+i]=Disp0_9String[st_data&0x0f];
			if (st_data&0x80)
				DispBuffer[LedX+j+i]=DispBuffer[LedX+j+i]|0x80;
		}
	}
	LED_DispDatas_all(DispBuffer);
}

void	ChgUlongToLongDispBuffer(ulong iii,uchar * ptr)
{
	ulong	jjj=10000000;
	uchar	i;
	uchar	st_data;
	uchar	bbit=0;
	memset(ptr,0,8);
	for (i=0;i<6;i++)
	{
		st_data=iii/jjj;
		if (st_data || bbit)
		{
			ptr[i]=Disp0_9String[st_data];
			bbit=1;
		}
		iii=iii%jjj;
		jjj=jjj/10;
	}
	if (!bbit)
		ptr[5]=Disp0_9String[0];
	ptr[5]|=0x80;
	iii=iii%100;
	st_data=iii/10;
	ptr[6]=Disp0_9String[st_data];
	st_data=iii%10;
	ptr[7]=Disp0_9String[st_data];
}

uchar	SelectDate(uchar * ptr)//选择消费日期
{
	uchar	st_data;
	uchar	bbit=1;
	uchar	Count=0;
	uchar 	Fx[4]={5,6,8,9};
	ulong	Long_Value;
	DispBuffer[0]=0x6d;//S
	DispBuffer[1]=0x08;//_
	DispBuffer[2]=0x5e;//d
	DispBuffer[3]=0x08;//_
	DispBuffer[7]=0x40;//-
	ptr[0]=SysTimeDatas.S_Time.MonthChar;
	ptr[1]=SysTimeDatas.S_Time.DayChar;
	while (1)
	{
		if (bbit)
		{
			bbit=0;
			st_data=ptr[0]>>4;
			DispBuffer[Fx[0]]=Disp0_9String[st_data];
			st_data=ptr[0]&0x0f;
			DispBuffer[Fx[1]]=Disp0_9String[st_data];
			st_data=ptr[1]>>4;
			DispBuffer[Fx[2]]=Disp0_9String[st_data];
			st_data=ptr[1]&0x0f;
			DispBuffer[Fx[3]]=Disp0_9String[st_data];
			LED_DispDatas_all(DispBuffer);
		}
		LED_NumFalsh_Disp(Fx[Count],1,0x10000);
		Long_Value=ScanKeySub(KEY_0_9 | KEY_ENTER|KEY_ESC|KEY_MUL);
		if (Long_Value)
		{
			if (Long_Value==KEY_ESC)
				return	1;
			else if (Long_Value==KEY_ENTER||Long_Value==KEY_MUL )
				return	0;
		 	else
			{
				st_data=ChgKeyValueToChar(Long_Value);
				if (!Count)
				{
					ptr[0]&=0x0f;
					st_data<<=4;
					ptr[0]+=st_data;
			  	}
				else if (Count==1)
				{
					ptr[0]&=0xf0;
					ptr[0]+=st_data;
			  	}
				else if (Count==2)
				{
					st_data<<=4;
					ptr[1]&=0x0f;
					ptr[1]+=st_data;
			   	}
				else if (Count==3)
				{
					ptr[1]&=0xf0;
					ptr[1]+=st_data;
				}
				bbit=1;
				Count++;
				Count=Count%4;
			}
		}
		//485TCP模式的功能选择
		if(CommModeChar == 1)
		{
//			Udp_Serverce();		
		}
		else
		{
		 	UART_ReceiveSub();
			UART_SendSub();			
		}
		Clr_WatchDog();
	}
}

void	LookRecordNum(uchar Sort)//察看消费记录条数
{
//	uchar		bbit=0;
	ulong	SumMoney=0;
	ulong	ii;
	ulong	Addr;
	uchar	LedX;
	uchar	Buffer[32];
	uchar	aaa[6];
	uchar	PPage;
	uchar	MonthChar,DayChar;
	if (Sort==3)
	{
		DispBuffer[0]=0x50;//r
		DispBuffer[1]=0x79;//E
		DispBuffer[2]=0x6D;//S
		DispBuffer[3]=0x08;//-
		Addr=RecordSum;		
		DispBuffer[4]=8;
		ChgIntToDispBuffer(Addr,DispBuffer+5);
	}
	else if (Sort==4)
	{

		DispBuffer[0]=0x50;//r
		DispBuffer[1]=0x79;//E
		DispBuffer[2]=0x37;//N
		DispBuffer[3]=0x08;//-
		Addr=NoCollectRecordSum;
		DispBuffer[4]=8;
		ChgIntToDispBuffer(Addr,DispBuffer+5);
	}
	else if (Sort==5)
	{
		DispBuffer[0]=0x6d;//S
		DispBuffer[1]=0x77;//A
		DispBuffer[2]=0x3e;//V
		DispBuffer[3]=0x08;//_
		DispBuffer[4]=8;
		ChgIntToDispBuffer(SaveRecordIndex,DispBuffer+5);
	}
	else if (Sort==0)
	{//A值：//开机后累计消费额
		DispBuffer[0]=0x77;//A.
		DispBuffer[1]=0x40; 
		ChgUlongToLongDispBuffer(Sys_SumConsumMoney,Buffer);
		DispBuffer[1]=8;
		memcpy(DispBuffer+2,Buffer,8);										
	}
	else if (Sort==1)
	{//L值：全部未采集消费额之和
		DispBuffer[0]=0x6d;//S
		DispBuffer[1]=0x39;//C
		DispBuffer[2]=0x77;//A
		DispBuffer[3]=0x37;//n	
		memset(DispBuffer+4,0,6);
		LED_DispDatas_all(DispBuffer);
		SumMoney=0;
		LedX=0;
		for (ii=0;ii<MAXRECORD_NUM;ii++)
		{
			//iii=(ulong)ii*RECORD_SIZE;
			Addr=(ulong)ii*RECORD_SIZE;
			//PPage=*(1+(uchar *)&iii);
			//memcpy((uchar *)&Addr,(2+(uchar *)&iii),2);
			Flash_Rd_Bytes(Addr,Buffer,32);
			if (Buffer[0]==0xa0 && !BytesCheckSum(Buffer+1,31)&& !(Buffer[1]&0x80))
			//if (Buffer[0]==0xa0)
				SumMoney+=ChgBCDStringToUlong(Buffer+15,3);
			UART_ReceiveSub();
			UART_SendSub();
			Clr_WatchDog();
			if (!(ii%200))
			{
	   			LedX++;
				LedX=LedX%10;
				for (PPage=0;PPage<10;PPage++)
				{
					if (PPage<LedX)
						DispBuffer[PPage]|=0x80;
					else
						DispBuffer[PPage]&=0x7f;	
				}
			}
	 	}
		bitHaveKey=0;
		BeepOn(1);
		DispBuffer[0]=0x38;//L
		DispBuffer[1]=0x40;
		ChgUlongToLongDispBuffer(SumMoney,Buffer);
		DispBuffer[1]=8;
		memcpy(DispBuffer+2,Buffer,8);			
	}
	else  if (Sort==2)
	{//D值
		PPage=SelectDate(Buffer);//选择消费日期
		if (PPage)
			return;
		DispBuffer[0]=0x6d;//S
		DispBuffer[1]=0x39;//C
		DispBuffer[2]=0x77;//A
		DispBuffer[3]=0x37;//n	
		memset(DispBuffer+4,0,6);
		LED_DispDatas_all(DispBuffer);
		SumMoney=0;
		LedX=0;
		MonthChar=Buffer[0];
		DayChar=Buffer[1];
		for (ii=0;ii<MAXRECORD_NUM;ii++)
		{
		//	iii=(ulong)ii*RECORD_SIZE;
			Addr=(ulong)ii*RECORD_SIZE;
		//	PPage=*(1+(uchar *)&iii);
		//	memcpy((uchar *)&Addr,(2+(uchar *)&iii),2);
			Flash_Rd_Bytes(Addr,Buffer,32);
			if (Buffer[0]==0xa0|| !Buffer[0])
			{
				if (!BytesCheckSum(Buffer+1,31)&& !(Buffer[1]&0x80))
				{
					ChgRecordDatasToTime(Buffer+22,aaa);
					if(aaa[0] == SysTimeDatas.S_Time.YearChar)
					{
					if (aaa[1]==MonthChar || !MonthChar)
					{
						if (aaa[2]==DayChar || !DayChar )
							SumMoney+=ChgBCDStringToUlong(Buffer+15,3);
					}
				}
				}
			}
			//485TCP模式的功能选择
			if(CommModeChar == 1)
			{
//				Udp_Serverce();	//处理uip事件，必须插入到用户程序的循环体中 	
			}
			else
			{
		 		UART_ReceiveSub();
				UART_SendSub();			
			}
			Clr_WatchDog();
			if (!(ii%200))
			{
				LedX++;
				LedX=LedX%10;
				for (PPage=0;PPage<10;PPage++)
				{
					if (PPage<LedX)
						DispBuffer[PPage]|=0x80;
					else
						DispBuffer[PPage]&=0x7f;	
				}
			}
		}
		bitHaveKey=0;
		BeepOn(1);
		DispBuffer[0]=0x5e;//d
		DispBuffer[1]=0x40;//d
		ChgUlongToLongDispBuffer(SumMoney,Buffer);
		DispBuffer[1]=8;
		memcpy(DispBuffer+2,Buffer,8);	
	}
	LED_DispDatas_all(DispBuffer);			
}



