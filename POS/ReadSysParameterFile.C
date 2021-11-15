#include "MyDefine.h"
#include "ExternVariableDef.h"
#include "ExternVoidDef.h"
#include "stm32f10x_flash.h"


extern uchar 	ConsumStaCodeSymbl[4];



uint16_t FLASH_ReadHalfWord(uint32_t address)
 {
   return *(__IO uint16_t*)address; 
 }
 
//往参数扇区写数据
static void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{    
	uint16_t dataIndex;
 FLASH_Unlock();         //?????

 FLASH_ErasePage(MainCodeAddress);//??????
 
 for(dataIndex=0;dataIndex<countToWrite;dataIndex++)
 {
	 FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
 }
 
 FLASH_Lock();//?????
}
//写参数到mcu flash
static uint16_t tempbuf[256];
void writeSysParameterToFlash(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{
	//uint16_t tempbuf[256];
	uint16_t dataIndex;
	uint32_t addDev =0;
	//备份数据
   for(dataIndex=0;dataIndex<256;dataIndex++)
   {
     tempbuf[dataIndex]=FLASH_ReadHalfWord(MainCodeAddress+dataIndex*2);
   }
	 addDev = (startAddress - MainCodeAddress)/2;
	 memcpy(tempbuf +addDev,writeData,countToWrite);
	 FLASH_WriteMoreData(MainCodeAddress,tempbuf,256);
	 
	 for(dataIndex=0;dataIndex<256;dataIndex++)
   {
     tempbuf[dataIndex]=FLASH_ReadHalfWord(MainCodeAddress+dataIndex*2);
   }
}


void	ReadSysParameter(uchar bbit)
{
	uchar	Buffer[40];
	uchar	i;
	uint MainCodeBak;
	ulong aaa;
	uint16_t buf[256];
	
//	memset(buf,0x56,256);
//	writeSysParameterToFlash(MainCodeAddress,buf,256);
//	memset(buf,0xee,256);
//	writeSysParameterToFlash(MainCodeAddress,buf,256);
//	uchar	cbit=0;
	if (!bitUpdateParameter)
		return	;
	bitUpdateParameter=0;

	if(!bbit)//zjx_change10_080602
	{
		//------------------------------------------------------
	    //经过download后，擦除站点
		RdBytesFromAT24C64(MainCode_Addr,Buffer,4);
		if (!BytesCheckSum(Buffer,4)&& Buffer[0]==0xa0)
		{
			memcpy((uchar *)&MainCode,(1+(uchar *)&Buffer),2);//adlc

			FLASH_Unlock();//解锁
			MainCodeBak=(*(vu16*) 0x8061000);
			FLASH_Lock();//锁定
//			if((MainCodeBak!=MainCode)||(!MainCode))
//			{
//				FLASH_Unlock();//解锁
//				FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
//				FLASH_ErasePage(MainCodeAddress);//擦除整页
//				FLASH_ProgramHalfWord(MainCodeAddress,MainCode);
//				FLASH_Lock();//锁定
//			}			
		//	MainCode = 0;
			
			tempbuf[0] = MainCode;
			writeSysParameterToFlash(MainCodeAddress,tempbuf,2);
		}

		FLASH_Unlock();//解锁
		MainCode=(*(vu16*) 0x8061000);
		FLASH_Lock();//锁定
			//WrBytesToAT24C64(MainCode_Addr,"\xff\xff\xff\xff",4);
		//------------------------------------------------------
    CardSector=2;//应用的扇区 
		memcpy(CardKeyCode,"\xa0\xa1\xa2\xa3\xa4\xa5",6);//读卡密码
		memcpy(CalCardKey,"\x11\x22\x33\x44\x55\x66\x77\x88",8);//卡密钥

		memcpy(CardMinBalance,"\x00\x00\x00",3);//卡底金
		CardEnableMonths=24; //黑名单有效月
		ConsumModeEnable=0x0f;//消费方式允许

		memset(CardBatchEnable,0,32);//批次是否有效
		CardBatchEnable[0]=1;// 默认0批次有效

		bitPinEnable=0;//超过限额输入密码

		memcpy(CommEncryptKey,"\x11\x22\x33\x44\x55\x66\x77\x88",8); //传输密钥;
		CommModeChar=0;
	}

//	RdBytesFromAT24C64(UserSector_Addr,Buffer,3);//应用的扇区
//	if (!BytesCheckSum(Buffer,3) && Buffer[0]==0xa0)
//		CardSector=Buffer[1];
	CardSector = FLASH_ReadHalfWord(UserSectorFlashAdd);

	//RdBytesFromAT24C64(UserCode_Addr,Buffer,6);//匹配字
	buf[0] = FLASH_ReadHalfWord(UserCodeFlashAdd);
	buf[1] = FLASH_ReadHalfWord(UserCodeFlashAdd +2);
	MatchCode[0] = buf[0]>>8 ;
	MatchCode[1] = buf[0] ;
	MatchCode[2] = buf[1]>>8 ;
	MatchCode[3] = buf[1] ;
//	if (!BytesCheckSum(Buffer,6) &&  Buffer[0]==0xa0)
//		memcpy(MatchCode,Buffer+1,4);
	
//	RdBytesFromAT24C64(ReadCardCode_Addr,Buffer,8);
////	if (!BytesCheckSum(Buffer,8)&& Buffer[0]==0xa0)
//		memcpy(CardKeyCode,Buffer+1,6);//公共山区卡密码
	buf[0] = FLASH_ReadHalfWord(CardKeyCodeFlashAdd);
	buf[1] = FLASH_ReadHalfWord(CardKeyCodeFlashAdd +2);
	buf[2] = FLASH_ReadHalfWord(CardKeyCodeFlashAdd +4);
	CardKeyCode[0] = buf[0]>>8 ;
	CardKeyCode[1] = buf[0] ;
	CardKeyCode[2] = buf[1]>>8 ;
	CardKeyCode[3] = buf[1] ;
	CardKeyCode[4] = buf[2]>>8 ;
	CardKeyCode[5] = buf[2] ;

//	RdBytesFromAT24C64(CalCardKey_Addr,Buffer,10);	
//	if (!BytesCheckSum(Buffer,10)&& Buffer[0]==0xa0)
//		memcpy(CalCardKey,Buffer+1,8);	//卡密钥

	buf[0] = FLASH_ReadHalfWord(CalCardKeyFlashAdd);
	buf[1] = FLASH_ReadHalfWord(CalCardKeyFlashAdd +2);
	buf[2] = FLASH_ReadHalfWord(CalCardKeyFlashAdd +4);
	buf[3] = FLASH_ReadHalfWord(CalCardKeyFlashAdd +6);
	CalCardKey[0] = buf[0]>>8 ;
	CalCardKey[1] = buf[0] ;
	CalCardKey[2] = buf[1]>>8 ;
	CalCardKey[3] = buf[1] ;
	CalCardKey[4] = buf[2]>>8 ;
	CalCardKey[5] = buf[2] ;
	CalCardKey[6] = buf[3]>>8 ;
	CalCardKey[7] = buf[3] ;

//	RdBytesFromAT24C64(CardMinBalance_Addr,Buffer,5);
//	if (!BytesCheckSum(Buffer,5) && Buffer[0]==0xa0)
//		memcpy(CardMinBalance,Buffer+1,3);//卡底金

	buf[0] = FLASH_ReadHalfWord(CardMinBalanceFlashAdd);
	buf[1] = FLASH_ReadHalfWord(CardMinBalanceFlashAdd+2);
	CardMinBalance[0] = buf[0]>>8 ;
	CardMinBalance[1] = buf[0] ;
	CardMinBalance[2] = buf[1]>>8 ;
	
//	RdBytesFromAT24C64(DayLimetMoney_Addr,Buffer,5);
//	if (!BytesCheckSum(Buffer,5) && Buffer[0]==0xa0)
//	{
//		memcpy(DayLimetMoney,Buffer+1,3);//日限额
//		aaa = ChgBCDStringToUlong(DayLimetMoney,3);//日限额
//		if(aaa)
//			DayLimetFlag = 1;
//		else
//			DayLimetFlag = 0;
//	}

	buf[0] = FLASH_ReadHalfWord(DayLimetMoneyFlashAdd);
	buf[1] = FLASH_ReadHalfWord(DayLimetMoneyFlashAdd+2);
	DayLimetMoney[0] = buf[0]>>8 ;
	DayLimetMoney[1] = buf[0] ;
	DayLimetMoney[2] = buf[1]>>8 ;
	aaa = ChgBCDStringToUlong(DayLimetMoney,3);//日限额
	if(aaa)
		DayLimetFlag = 1;
	else
		DayLimetFlag = 0;

//	RdBytesFromAT24C64(CardEnableMonths_Addr,Buffer,3);//黑名单有效月
//	if (!BytesCheckSum(Buffer,3) && Buffer[0]==0xa0)
//		CardEnableMonths=Buffer[1];
	
	CardEnableMonths = FLASH_ReadHalfWord(CardEnableMonthsFlasgAdd);
	
//	RdBytesFromAT24C64(ConsumModeEnable_Addr,Buffer,3);//消费方式允许
	ConsumModeEnable = FLASH_ReadHalfWord(ConsumModeEnableFlashAdd);
//	if (!BytesCheckSum(Buffer,3) && Buffer[0]==0xa0)
//		ConsumModeEnable=Buffer[1];
//	else
//	{
//		ConsumModeEnable |= ((1 << 4) | (1 << 5));
//		Buffer[0]=0xa0;
//		Buffer[1]=ConsumModeEnable;
//		Buffer[2]=CalCheckSum(Buffer,2);
//		tempbuf[0] = (Buffer[1]);
//		writeSysParameterToFlash(ConsumModeEnableFlashAdd,tempbuf,2);
//		//WrBytesToAT24C64(ConsumModeEnable_Addr,Buffer,3);
//	}

	if (!bbit)
    {//第一次初试化，对消费方式进行初试化
	    ConsumMode=CONSUM_MONEY;//默认的消费方式
	    if (!(ConsumModeEnable&8))
		{//不允许金额消费
		    for (i=0;i<4;i++)
		    {
			    if (ConsumModeEnable & (1<<i))
				{	
				    ConsumMode=i;
					break;	
				}	
			}
		}		
	}
    
//	RdBytesFromAT24C64(BatchEnable_Addr,Buffer,34);//批次是否有效
//	if (!BytesCheckSum(Buffer,34) && Buffer[0]==0xa0)
//		memcpy(CardBatchEnable,Buffer+1,32);
	for(i=0;i<16;i++)
	{
		buf[i] = FLASH_ReadHalfWord(BatchEnableFlashAdd +i*2);
		CardBatchEnable[i*2] = buf[i]>>8 ;
		CardBatchEnable[i*2+1] = buf[i] ;
	}
	
	RdBytesFromAT24C64(PinEnable_Addr,Buffer,3);//超过限额输入密码
	if (!BytesCheckSum(Buffer,3) && Buffer[0]==0xa0 )
	{
		if (Buffer[1]==0x55)
			bitPinEnable=1;	
	}

//	RdBytesFromAT24C64(Comm_EncryptKey_Addr,Buffer,10); //传输密钥;
//	if (!BytesCheckSum(Buffer,10) && Buffer[0]==0xa0)
//		memcpy(CommEncryptKey,Buffer+1,8);
	
	buf[0] = FLASH_ReadHalfWord(CommEncryptKeyFlasAdd);
	buf[1] = FLASH_ReadHalfWord(CommEncryptKeyFlasAdd +2);
	buf[2] = FLASH_ReadHalfWord(CommEncryptKeyFlasAdd +4);
	buf[3] = FLASH_ReadHalfWord(CommEncryptKeyFlasAdd +6);
	CommEncryptKey[0] = buf[0]>>8 ;
	CommEncryptKey[1] = buf[0] ;
	CommEncryptKey[2] = buf[1]>>8 ;
	CommEncryptKey[3] = buf[1] ;
	CommEncryptKey[4] = buf[2]>>8 ;
	CommEncryptKey[5] = buf[2] ;
	CommEncryptKey[6] = buf[3]>>8 ;
	CommEncryptKey[7] = buf[3] ;
		               
	RdBytesFromAT24C64(CommModeChar_Addr,Buffer,3);	  //设置通信模式
	if (!BytesCheckSum(Buffer,3) && Buffer[0]==0xa0 )
	{
		CommModeChar=Buffer[1];	
		
	}
	else
		CommModeChar = FLASH_ReadHalfWord(CommModeCharFlashAdd);

	RdBytesFromAT24C64(RemoteFlagAddr,Buffer,3);	  //设置开启绑定远端IP
	if (!BytesCheckSum(Buffer,3) && Buffer[0]==0xa0 )
	{
		RemoteIPFlag=Buffer[1];	
	}
	else
		RemoteIPFlag = FLASH_ReadHalfWord(RemoteFlagFlashAdd);
	
	
	RdBytesFromAT24C64(CanBuTie_Addr,Buffer,4);
	if (!(memcmp(Buffer,ConsumStaCodeSymbl,4)))
		canbutie=0;
	else
		canbutie=1;
	
//	RdBytesFromAT24C64(Rebate_Flag_Addr,Buffer,2);	  //读取是否下载过折扣方案
//	if ( (Buffer[0]==0xa0)&(Buffer[1]==0xaa))
//	{
//		Rebate_YesOrNo_Flag=1;	
//	}

//波特率
	if (!bbit)
	{
		if (!CommModeChar)
		{
//			DK_USART_Init(9600, 1);		
		}
	}		
}


//读出身份对应的折扣值
uchar Read_Rebate(void)
{
	uchar status = 0x64;
	return status;
	//status =AT24C64_ReadOneByte(Rebate_Addr+CardIdentity);
//	if(status!=0xff)
//		return status;
//	else
//		return 0xff;
	
}
