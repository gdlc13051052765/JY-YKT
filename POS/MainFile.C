#include 	"MyDefine.h"
#include	"ExternVariableDef.h"
#include	"ExternVoidDef.h"
#include	"char.h"
#include "485.H"
#include "gpio.H"
#include "SPI_FLASH.H"
#include "AT24C256.H"
#include "stm32f10x_flash.h"

 uchar	CheckPassWord(uchar * PinChar,uchar Num);
 int memcmpself(unsigned char * data1,unsigned char *data2,int cnt);

void	Clr_PosDatas(void)
{
	uchar	status;
	uchar	Count=0;
	uchar	st_data;
	uchar	Buffer[6];
	ulong	iii=0;
	Read_Sysdate(SysTimeDatas.TimeString);
	Buffer[0]=0x30+(SysTimeDatas.S_Time.DayChar>>4);
	Buffer[1]=0x30+(SysTimeDatas.S_Time.DayChar&0x0f);
 	Buffer[2]=0x30+(SysTimeDatas.S_Time.HourChar>>4);
	Buffer[3]=0x30+(SysTimeDatas.S_Time.HourChar&0x0f);
	status=MainCode%100;
	Buffer[4]=0x30+(status/10);
	Buffer[5]=0x30+(status%10);
	status=CheckPassWord(Buffer,6);
	if (status)
		return;
	DispBuffer[0]=0x6d;//S
	DispBuffer[1]=0x38;//L
	DispBuffer[2]=0x40;//-
	DispBuffer[3]=0x40;//-
	DispBuffer[4]=0x40;//-
	memset(DispBuffer+5,0,5);
	LED_DispDatas_all(DispBuffer);
	memset(Buffer,0,3);
	while (1)
	{
		LED_NumFalsh_Disp(2+Count,1,0x10000);
		iii=ScanKeySub(KEY_0|KEY_1|KEY_ENTER|KEY_ESC);
		if (iii)
		{
			if (iii==KEY_ESC)
				return;
			else if (iii==KEY_ENTER)
				break;
			else if (iii==KEY_0)
			{
				DispBuffer[2+Count]=Disp0_9String[0];
				Forbidden=1;
				LED_OneByte_Disp(2+Count,DispBuffer[2+Count]);
				Forbidden=0;
				Buffer[Count++]=0;
				Count=Count%3;
				
			}
			else
			{	
				DispBuffer[2+Count]=Disp0_9String[1];
				Forbidden=1;
				LED_OneByte_Disp(2+Count,DispBuffer[2+Count]);
				Forbidden=0;
				Buffer[Count++]=1;
				Count=Count%3;			 
		  	}
	
		}
		Clr_WatchDog();		
	}
	st_data=0;
	if (Buffer[0]==1)
		st_data|=1;
	if (Buffer[1]==1)
		st_data|=2;
	if (Buffer[2]==1)
		st_data|=4;
	Disp_Clr_Ram();	
	if (st_data&7)
	{
		Clr_PosSub(0,st_data);
		while(1);
	}
}

//密码认证
uchar	CheckPassWord(uchar * PinChar,uchar Num)
{
	uchar	Buffer[6];
	uchar	Count=0;
	ulong	Long_Value;
	uchar	status;
	memset(DispBuffer,0,10);
	DispBuffer[0]=0x73;//P
	DispBuffer[1]=0x30;//I
	DispBuffer[2]=0x37;//n
	while (1)
	{
		Long_Value=	ScanKeySub(KEY_ANY);
		if (!Long_Value)
			break;	
		Clr_WatchDog();
	}	
	BeepOn(2);
	LED_DispDatas_all(DispBuffer);
	while (1)
	{
		Long_Value=ScanKeySub(KEY_ENTER|KEY_ESC|KEY_0_9);
		if (Long_Value)
		{
			if (Long_Value==KEY_ESC)
				return 1;
			else if (Long_Value==KEY_ENTER)
			{
				if (Count==Num)
				{
					status=memcmpself(Buffer,PinChar,Num);
					return	status;
				}
				else
					return	1;
			}
			else
			{
				if (Count<Num)
				{
					DispBuffer[3+Count]=0x40;
					Forbidden=1;
					LED_OneByte_Disp(3+Count,0x40);
					Forbidden=0;
					Buffer[Count]=ChgKeyValueToChar(Long_Value)+0x30;
					Count++;
				}
				else
					return	1;
			}
		}
		Clr_WatchDog();	
	}
}
void	Clr_PosSub(uchar bbit,uchar Sort)
{
	uchar	Buffer[64];
	uchar	XBuffer[25];
	uint	i;
	ulong	ii;
	ulong Addr = 0;
	uint16_t tempbuf[24];
	
//	FLASH_Unlock();//解锁
//	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
//	FLASH_ErasePage(MainCodeAddress);//擦除整页
//	FLASH_ProgramHalfWord(MainCodeAddress,MainCode);
//	FLASH_Lock();//锁定				
//	writeSysParameterToFlash(MainCodeAddress,&MainCode,2);	
//	tempbuf[0] = CommModeChar;
//	writeSysParameterToFlash(CommModeCharFlashAdd,tempbuf,2);
	
	
	if (Sort&1)
	{
		Forbidden=1;
		RdBytesFromAT24C64(TcpCardDatas_Addr,XBuffer,24);
		memset(Buffer,0xff,32);
		for (i=0;i<300-32;i++)
		{
			AT24CXX_WriteSome(Addr,0xff);
			Addr+=16;
			//if (bbit && !(i%8))
			{
//				if (CommModeChar)
//					UdpSendStx();
//				else
					SerialSendChar(STX);
			}
			Clr_WatchDog();	
		}

		WrBytesToAT24C64(TcpCardDatas_Addr,XBuffer,24);//网络参数
		for(i=0;i<22;)
		{
			tempbuf[i/2] = (XBuffer[i+1]) *256 + (uint16_t)XBuffer[i+2];
			i+=2;
		}
		writeSysParameterToFlash(TcpCardDatasFlashAdd,tempbuf,22);

		Buffer[0]=0xa0;
		Buffer[1]=CommModeChar;
		Buffer[2]=CalCheckSum(Buffer,2);
		WrBytesToAT24C64(CommModeChar_Addr,Buffer,3);//通讯方式
		tempbuf[0] = Buffer[1];
		writeSysParameterToFlash(CommModeCharFlashAdd,tempbuf,2);

		Buffer[0]=0xa0;
		Buffer[1]=* (uchar *)&MainCode;
		Buffer[2]=* (1+ (uchar *)&MainCode);
		Buffer[3]=CalCheckSum(Buffer,3);
		WrBytesToAT24C64(MainCode_Addr,Buffer,4);//重写站点号	

		Forbidden=0;
	}
	Addr =0;
	if (Sort&2)
	{	
		memset(Buffer,0xff,64);		
		for (ii=0;ii<2024;ii++)
		{
			//AT24CXX_Write(ii*64,Buffer,64);
			WrByte_24C256_Some(Addr,0xff);
			Addr+=16;
				Clr_WatchDog();
			//if (bbit && !(ii%8))
			{
//				if (CommModeChar)
//					UdpSendStx();
//				else
					SerialSendChar(STX);
			}
			Clr_WatchDog();	
		}
	}
	if (Sort&4)
	{
		Erase_Entire_Chip();
		NeedResartFlag = 1;
	}
		PlayNum(Clr_Pos );//设备清空成功
	//Text_24C256();
	//Text_24C64();
}
