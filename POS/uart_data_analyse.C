#include 	"MyDefine.h"
#include	"ExternVariableDef.h"
#include	"ExternVoidDef.h"
#include	"char.h"
#include "485.H"
#include "gpio.H"
#include "SPI_FLASH.H"
#include "AT24C256.H"
#include "stm32f10x_flash.h"

void Force_Flash_Rd_Bytes(unsigned long addr, unsigned char *readbuff, unsigned short BlockSize);
void Force_Flash_Write_Bytes(u32 WriteAddr,u8* pBuffer,u16 NumByteToWrite)   ;
void	UART_ReceiveSub(void);
void	UART_SendSub(void);
uchar	ReceiveDatasDiag(void);//�����������
int memcmpself(unsigned char * data1,unsigned char *data2,int cnt);
void	Chg_BlkNameSub(uchar,ulong );//����/ɾ�� ������
void	SerialAscToHex(uchar *,uchar );
void	CalComdSymblDatas(uchar ,uchar * );

#define ApplicationAddress 0x08000000
#define DownFlagAddress    0x08060000
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
void Jmp_Loader(void)
{
	uint32_t JumpAddress;
	//__disable_irq() ;
	TIM_Cmd(TIM3, DISABLE);  //ʹ��TIMx����
	TIM_Cmd(TIM2, DISABLE);  //ʹ��TIMx����
	if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
	{ /* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) ApplicationAddress);
		Jump_To_Application();
	}
}

void clearoneRec();
extern uchar getbtrec;


void	UART_ReceiveSub(void)//�������ݴ���
{
	uchar		status,j;
	uchar		DatasBuffer[40];
	uchar		aaa[6];
	uchar    buf[1];
	uchar   Buffer[30];
	uchar 		LoadSum =0;
	uchar		i,PPage,MonthChar,DayChar,Nums;
	ulong		iii,SumMoney;
	uint32_t	ii,Addr;
	uint16_t    bigtosmalltemp,downflag;
	uint16_t tempbuf[50];
	if (!bitSerial_ReceiveEnd)
		return	;
	status=ReceiveDatasDiag();
	Receive_Data_Double ();//�����յ����ݴ�С��ת������

	bitSerial_ReceiveEnd=0;
	if (status==0xff)
	{
	    Init_Serial();
		return;
	}
	u485_TE; //
	bitSerial_SendRequest=1;
	SerialUnion.S_DatasStruct.UartStatus=0;
	switch (SerialUnion.S_DatasStruct.UartComd & 0xfff)
	{
		case	RD_ADDR_COMD://����վ���
			SerialUnion.S_DatasStruct.Datas[0]=MainCode>>8;
			SerialUnion.S_DatasStruct.Datas[1]=(uchar)MainCode;
			SerialUnion.S_DatasStruct.DatasLen=2;
			break;
		//====================================================================
	//	case	SET_USERCODE_COMD:
	//		SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case    DOWNLODER_COMD:
				{
					LoadModeFlag = 1;
					LoadModeResponse[0] = STX;
					LoadModeResponse[1]=SerialUnion.S_DatasStruct.UartReAddrCode>>8;
					LoadModeResponse[2]=SerialUnion.S_DatasStruct.UartReAddrCode%256;
					LoadModeResponse[3]=SerialUnion.S_DatasStruct.UartSeAddrCode>>8	;
					LoadModeResponse[4]=SerialUnion.S_DatasStruct.UartSeAddrCode%256;
					LoadModeResponse[5]=SerialUnion.S_DatasStruct.UartComd	;
					LoadModeResponse[6]=SerialUnion.S_DatasStruct.UartStatus;
					LoadModeResponse[7]=SerialUnion.S_DatasStruct.UartAddrH	;
					LoadModeResponse[8]=SerialUnion.S_DatasStruct.UartAddrL	;
					LoadModeResponse[9]=SerialUnion.S_DatasStruct.UartFrameNum;
					LoadModeResponse[10]=2;
					LoadModeResponse[11]='G';
					LoadModeResponse[12]='G';
		
					for(i=1;i<13;i++)
					{
						LoadSum+=LoadModeResponse[i];
					}
					LoadModeResponse[13]=ETX;
					LoadModeResponse[14]=LoadSum;
				}
			break;
		case	RD_USERCODE_COMD://�ϴ�ƥ����
			memcpy(DatasBuffer,MatchCode,4);
			DatasBuffer[4]=CalCheckSum(DatasBuffer,4);
			CalEncryptDatas(0,DatasBuffer,CommEncryptKey,SerialUnion.S_DatasStruct.Datas,5);//����
			SerialUnion.S_DatasStruct.DatasLen=5;
			SerialUnion.S_DatasStruct.UartComd|=0x8000;	//���ݼ���		
			break;
		case	SET_USERCODE_COMD://ƥ��������
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if ((SerialUnion.S_DatasStruct.UartComd & 0x8000) && SerialUnion.S_DatasStruct.DatasLen==5)
			{
				CalEncryptDatas(1,SerialUnion.S_DatasStruct.Datas,CommEncryptKey,DatasBuffer+1,5);//����
				if (!BytesCheckSum(DatasBuffer+1,5))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					DatasBuffer[0]=0xa0;
					DatasBuffer[5]=CalCheckSum(DatasBuffer,5);
					
				//	RdBytesFromAT24C64(UserCode_Addr,Buffer,6);
					tempbuf[0] = (DatasBuffer[1]) *256 + (uint16_t)DatasBuffer[2];
					tempbuf[1] = (DatasBuffer[3]) *256 + (uint16_t)DatasBuffer[4];
					memcpy(Buffer+1,MatchCode,4);
					if(memcmpself(Buffer+1,DatasBuffer+1,4)) 
					{
						writeSysParameterToFlash(UserCodeFlashAdd,tempbuf,4);
						bitUpdateParameter=1;	
						WrBytesToAT24C64(UserCode_Addr,DatasBuffer,6);
					}	
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			SerialUnion.S_DatasStruct.UartComd&=0xfff;	 										
			break;
		case	SET_RDCARDCODE_COMD://���ض�������
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if ((SerialUnion.S_DatasStruct.UartComd & 0x8000) && SerialUnion.S_DatasStruct.DatasLen==7)
			{
				CalEncryptDatas(1,SerialUnion.S_DatasStruct.Datas,CommEncryptKey,DatasBuffer+1,7);//����
				if (!BytesCheckSum(DatasBuffer+1,7))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					DatasBuffer[0]=0xa0;
					DatasBuffer[7]=CalCheckSum(DatasBuffer,7);
					
					//RdBytesFromAT24C64(ReadCardCode_Addr,Buffer,8);
					memcpy(Buffer+1,CardKeyCode,6);
					tempbuf[0] = (DatasBuffer[1]) *256 + (uint16_t)DatasBuffer[2];
					tempbuf[1] = (DatasBuffer[3]) *256 + (uint16_t)DatasBuffer[4];
					tempbuf[2] = (DatasBuffer[5]) *256 + (uint16_t)DatasBuffer[6];
					if(memcmpself(Buffer+1,DatasBuffer+1,6))
					{
						writeSysParameterToFlash(CardKeyCodeFlashAdd,tempbuf,6);
						bitUpdateParameter=1;	
						WrBytesToAT24C64(ReadCardCode_Addr,DatasBuffer,8);	
					}
									
					SerialUnion.S_DatasStruct.DatasLen=0;
					
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			SerialUnion.S_DatasStruct.UartComd&=0xfff;	 										
			break;
		case	RD_RDCARDCODE_COMD://������������
			memcpy(DatasBuffer,CardKeyCode,6);
			DatasBuffer[6]=CalCheckSum(DatasBuffer,6);
			CalEncryptDatas(0,DatasBuffer,CommEncryptKey,SerialUnion.S_DatasStruct.Datas,7);//����
			SerialUnion.S_DatasStruct.DatasLen=7;
			SerialUnion.S_DatasStruct.UartComd|=0x8000;	//���ݼ���					
			break;
		case	RD_COMMSECTOR_COMD://������������������	
			SerialUnion.S_DatasStruct.Datas[0]=CardSector;
			SerialUnion.S_DatasStruct.DatasLen=1;
			break;			
		case	SET_COMMSECTOR_COMD://���ù�������������
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==1)
			{
				SerialUnion.S_DatasStruct.UartStatus=0;
				DatasBuffer[0]=0xa0;
				DatasBuffer[1]=SerialUnion.S_DatasStruct.Datas[0];
				DatasBuffer[2]=CalCheckSum(DatasBuffer,2);
				
				//RdBytesFromAT24C64(UserSector_Addr,Buffer,3);
				Buffer[1] = CardSector;
				if(memcmpself(Buffer+1,DatasBuffer+1,1))
				{
					tempbuf[0] = (DatasBuffer[1]);
					writeSysParameterToFlash(UserSectorFlashAdd,tempbuf,2);
					WrBytesToAT24C64(UserSector_Addr,DatasBuffer,3);
					bitUpdateParameter=1;	
				}				
			}
			SerialUnion.S_DatasStruct.DatasLen=0;	
			break;
		case	SET_CALCARDKEY_COMD://����д���ļ�����Կ
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if ((SerialUnion.S_DatasStruct.UartComd & 0x8000) && SerialUnion.S_DatasStruct.DatasLen==9)
			{
				CalEncryptDatas(1,SerialUnion.S_DatasStruct.Datas,CommEncryptKey,DatasBuffer+1,9);//����
				if (!BytesCheckSum(DatasBuffer+1,9))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					DatasBuffer[0]=0xa0;
					DatasBuffer[9]=CalCheckSum(DatasBuffer,9);
					//RdBytesFromAT24C64(CalCardKey_Addr,Buffer,10);
					memcpy(Buffer+1,CalCardKey,8);
					if(memcmpself(Buffer+1,DatasBuffer+1,8))
					{
						tempbuf[0] = (DatasBuffer[1]) *256 + (uint16_t)DatasBuffer[2];
						tempbuf[1] = (DatasBuffer[3]) *256 + (uint16_t)DatasBuffer[4];
						tempbuf[2] = (DatasBuffer[5]) *256 + (uint16_t)DatasBuffer[6];
						tempbuf[3] = (DatasBuffer[7]) *256 + (uint16_t)DatasBuffer[8];
						writeSysParameterToFlash(CalCardKeyFlashAdd,tempbuf,8);
						WrBytesToAT24C64(CalCardKey_Addr,DatasBuffer,10);		
						bitUpdateParameter=1;	
					}					
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			SerialUnion.S_DatasStruct.UartComd&=0xfff;
			break;
		case	RD_CALCARDKEY_COMD://����д���ļ�����Կ
			memcpy(DatasBuffer,CalCardKey,8);
			DatasBuffer[8]=CalCheckSum(DatasBuffer,8);
			CalEncryptDatas(0,DatasBuffer,CommEncryptKey,SerialUnion.S_DatasStruct.Datas,9);//����
			SerialUnion.S_DatasStruct.DatasLen=9;
			SerialUnion.S_DatasStruct.UartComd|=0x8000;//���ݼ���
			break;
		//====================================================================
		case	POS_RST_COMD://POS����λ
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==6)
			{
				CalComdSymblDatas(POS_RST_COMD,DatasBuffer);
				if (!memcmpself(SerialUnion.S_DatasStruct.Datas,DatasBuffer,6))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					bitNeedRST=1;
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
			
		case	SET_BATCH_COMD://���������Ƿ���Ч
			if (SerialUnion.S_DatasStruct.DatasLen==32)
			{
				DatasBuffer[0]=0xa0;
				memcpy(DatasBuffer+1,SerialUnion.S_DatasStruct.Datas,32);
				DatasBuffer[33]=CalCheckSum(DatasBuffer,33);
				
				if(memcmpself(CardBatchEnable,DatasBuffer+1,32))
				{
					for(i=0;i<32;)
					{
						tempbuf[i/2] = (DatasBuffer[i+1]) *256 + (uint16_t)DatasBuffer[i+2];
						i+=2;
					}
					writeSysParameterToFlash(BatchEnableFlashAdd,tempbuf,32);
					WrBytesToAT24C64(BatchEnable_Addr,DatasBuffer,34);
					bitUpdateParameter=1;	
				}
			}
			else
				SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	RD_BATCH_COMD://���������Ƿ���Ч
			SerialUnion.S_DatasStruct.DatasLen=32;
			memcpy(SerialUnion.S_DatasStruct.Datas,CardBatchEnable,32);
	   		break;
		case	RD_MINMONEY_COMD://�ϴ��׽�
			SerialUnion.S_DatasStruct.Datas[0]=0;
			memcpy(SerialUnion.S_DatasStruct.Datas+1,CardMinBalance,3);
			SerialUnion.S_DatasStruct.DatasLen=4;			
			break;
		case	SET_MINMONEY_COMD://�׽�����
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==4 && !BCD_String_Diag(SerialUnion.S_DatasStruct.Datas,4))
			{
				SerialUnion.S_DatasStruct.UartStatus=0;
				DatasBuffer[0]=0xa0;
				memcpy(DatasBuffer+1,SerialUnion.S_DatasStruct.Datas+1,3);
				DatasBuffer[4]=CalCheckSum(DatasBuffer,4);
				
				tempbuf[0] = (DatasBuffer[1]) *256 + (uint16_t)DatasBuffer[2];
				tempbuf[1] = (DatasBuffer[3]) *256 ;
				writeSysParameterToFlash(CardMinBalanceFlashAdd,tempbuf,4);
				WrBytesToAT24C64(CardMinBalance_Addr,DatasBuffer,5);
				bitUpdateParameter=1;		
			}
			SerialUnion.S_DatasStruct.DatasLen=0;			
			break;
				case	RD_DAYLIMET_COMD://�ϴ����޶�
			SerialUnion.S_DatasStruct.Datas[0]=0;
			memcpy(SerialUnion.S_DatasStruct.Datas+1,DayLimetMoney,3);
			SerialUnion.S_DatasStruct.DatasLen=4;			
			break;
		case	SET_DAYLIMET_COMD://�������޶�
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==4 && !BCD_String_Diag(SerialUnion.S_DatasStruct.Datas,4))
			{
				SerialUnion.S_DatasStruct.UartStatus=0;
				DatasBuffer[0]=0xa0;
				memcpy(DatasBuffer+1,SerialUnion.S_DatasStruct.Datas+1,3);
				DatasBuffer[4]=CalCheckSum(DatasBuffer,4);
				
				tempbuf[0] = (DatasBuffer[1]) *256 + (uint16_t)DatasBuffer[2];
				tempbuf[1] = (DatasBuffer[3]) *256 ;
				writeSysParameterToFlash(DayLimetMoneyFlashAdd,tempbuf,4);
				WrBytesToAT24C64(DayLimetMoney_Addr,DatasBuffer,5);
				bitUpdateParameter=1;		
			}
			SerialUnion.S_DatasStruct.DatasLen=0;			
			break;
		case	RD_BLKNAME_TIME_COMD://������������Ч��
			SerialUnion.S_DatasStruct.Datas[0]=CardEnableMonths;
			SerialUnion.S_DatasStruct.DatasLen=1;
			break;
		case	SET_BLKNAMETIME_COMD://���غ�������Ч��
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==1)
			{
				SerialUnion.S_DatasStruct.UartStatus=0;
				DatasBuffer[0]=0xa0;
				DatasBuffer[1]=SerialUnion.S_DatasStruct.Datas[0];
				DatasBuffer[2]=CalCheckSum(DatasBuffer,2);
				
				tempbuf[0] = DatasBuffer[1];
				writeSysParameterToFlash(CardEnableMonthsFlasgAdd,tempbuf,2);
				WrBytesToAT24C64(CardEnableMonths_Addr,DatasBuffer,3);
				bitUpdateParameter=1;	
			}
			SerialUnion.S_DatasStruct.DatasLen=0;		
			break;
		case	CLR_PURSE_COMD://����Ǯ��
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==6)
			{
				CalComdSymblDatas(CLR_PURSE_COMD,DatasBuffer);
				if (!memcmpself(SerialUnion.S_DatasStruct.Datas,DatasBuffer,6))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					Erase_One_Sector(PurseKind_Addr);
					Erase_One_Sector(PurseEnable_Addr);
					memcpy(DatasBuffer,DefaultKind,4);
					Flash_Write_Bytes(PurseKind_Addr,DatasBuffer,4);
					Flag_NotDefault=1;
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
		
			break;
		case	SET_PURSE_COMD://����Ǯ��
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;	
			if (SerialUnion.S_DatasStruct.UartAddrL<100 && !(SerialUnion.S_DatasStruct.DatasLen%13))
			{
				SerialUnion.S_DatasStruct.UartStatus=0;	
				if ((SerialUnion.S_DatasStruct.UartAddrL+(SerialUnion.S_DatasStruct.DatasLen/13) )<101)
					PPage=SerialUnion.S_DatasStruct.DatasLen/13;
				else
					PPage=100-SerialUnion.S_DatasStruct.UartAddrL;	
				Addr=PurseEnable_Addr+(uint)SerialUnion.S_DatasStruct.UartAddrL*16;
				for (i=0;i<PPage;i++)
				{
				    DatasBuffer[0]=0xa0;
					DatasBuffer[1]=SerialUnion.S_DatasStruct.UartAddrL+i;
					memcpy(DatasBuffer+2,SerialUnion.S_DatasStruct.Datas+i*13,13);
					DatasBuffer[15]=CalCheckSum(DatasBuffer,15);
					Flash_Write_Bytes(Addr,DatasBuffer,16);
					Addr+=16;
				}
				Erase_One_Sector(PurseKind_Addr);
				Flag_NotDefault=0;
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	RD_TIME2_COMD://��ѯʱ��
			Read_Sysdate(SysTimeDatas.TimeString);
			memcpy(SerialUnion.S_DatasStruct.Datas,SysTimeDatas.TimeString,7);
			SerialUnion.S_DatasStruct.DatasLen=7;
			break;
		case	SET_TIME2_COMD://ʱ������
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==7 && !DiagTimeString(0,SerialUnion.S_DatasStruct.Datas) && !DiagTimeString(1,SerialUnion.S_DatasStruct.Datas+3))
			{
				SerialUnion.S_DatasStruct.UartStatus=0;
				Set_Sysdate(SerialUnion.S_DatasStruct.Datas);
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	CLR_BLKNUM_COMD://���������
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==6)
			{
				CalComdSymblDatas(CLR_BLKNUM_COMD,DatasBuffer);
				if (!memcmpself(SerialUnion.S_DatasStruct.Datas,DatasBuffer,6))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					Clr_PosSub(1,2);
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	ADD_BLKNUM_COMD://���Ӻ�����
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;	
			if (SerialUnion.S_DatasStruct.DatasLen && !(SerialUnion.S_DatasStruct.DatasLen%4))
			{
				status=SerialUnion.S_DatasStruct.DatasLen/4;
				for(i=0;i<status;i++)
				{
					iii=ChgBCDStringToUlong(SerialUnion.S_DatasStruct.Datas+i*4,4);
					if (iii<MAXCARDPRINTERNUM)
						Chg_BlkNameSub(0,iii);
				}
				SerialUnion.S_DatasStruct.UartStatus=0;
			}	
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	DEL_BLKNUM_COMD://ɾ��������
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;	
			if (SerialUnion.S_DatasStruct.DatasLen && !(SerialUnion.S_DatasStruct.DatasLen%4))
			{
				status=SerialUnion.S_DatasStruct.DatasLen/4;
				for(i=0;i<status;i++)
				{
					iii=ChgBCDStringToUlong(SerialUnion.S_DatasStruct.Datas+i*4,4);
					if (iii<MAXCARDPRINTERNUM)
						Chg_BlkNameSub(1,iii);
				}
				SerialUnion.S_DatasStruct.UartStatus=0;
			}	
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	DEL_ADD_BLKNUM_COMD:
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;	
			if (SerialUnion.S_DatasStruct.DatasLen && !(SerialUnion.S_DatasStruct.DatasLen%5))
			{
				status=SerialUnion.S_DatasStruct.DatasLen/5;
				for(i=0;i<status;i++)
				{	
					iii=ChgBCDStringToUlong(SerialUnion.S_DatasStruct.Datas+i*5+1,4);
					if (iii<MAXCARDPRINTERNUM)
					{
						if (SerialUnion.S_DatasStruct.Datas[i*5]==0x55)
							Chg_BlkNameSub(0,iii);
						if (SerialUnion.S_DatasStruct.Datas[i*5]==0xaa)
							Chg_BlkNameSub(1,iii);
					}	
				}
				SerialUnion.S_DatasStruct.UartStatus=0;
			}	
			SerialUnion.S_DatasStruct.DatasLen=0;						
			break;
		case	CLR_POSDATAS_COMD://���POS����
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==7)
			{
				CalComdSymblDatas(CLR_POSDATAS_COMD,DatasBuffer);
				if (!memcmpself(SerialUnion.S_DatasStruct.Datas+1,DatasBuffer,6))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					bitNeedRST=1;
					Disp_Clr_Ram();
					Clr_PosSub(1,SerialUnion.S_DatasStruct.Datas[0]);
					STX_FLAG =1;
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	RD_POSSTATUS_COMD://����POS״̬
			InitSaveRecordSub(0);
			SerialUnion.S_DatasStruct.Datas[0]=0;
			bigtosmalltemp = DoubleBigToSmall(RecordSum); //adlc
			memcpy(SerialUnion.S_DatasStruct.Datas+1,(uchar *)&bigtosmalltemp,2);
			bigtosmalltemp = DoubleBigToSmall(NoCollectRecordSum); //adlc
			memcpy(SerialUnion.S_DatasStruct.Datas+3,(uchar *)&bigtosmalltemp,2);
			bigtosmalltemp = DoubleBigToSmall(ReCollectRecordIndex);//adlc
			memcpy(SerialUnion.S_DatasStruct.Datas+5,(uchar *)&bigtosmalltemp,2);
			bigtosmalltemp = DoubleBigToSmall(SaveRecordIndex);//adlc
			memcpy(SerialUnion.S_DatasStruct.Datas+7,(uchar *)&bigtosmalltemp,2);
			SerialUnion.S_DatasStruct.DatasLen=9;
			break;
		case	SET_ENCRYPTKEY_COMD://���ش�����Կ
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==8)
			{
				SerialUnion.S_DatasStruct.UartStatus=0;
				DatasBuffer[0]=0xa0;
				memcpy(DatasBuffer+1,SerialUnion.S_DatasStruct.Datas,8);
				DatasBuffer[9]=CalCheckSum(DatasBuffer,9);
				
				memcpy(Buffer+1,CardKeyCode,6);
				tempbuf[0] = (DatasBuffer[1]) *256 + (uint16_t)DatasBuffer[2];
				tempbuf[1] = (DatasBuffer[3]) *256 + (uint16_t)DatasBuffer[4];
				tempbuf[2] = (DatasBuffer[5]) *256 + (uint16_t)DatasBuffer[6];
				tempbuf[3] = (DatasBuffer[7]) *256 + (uint16_t)DatasBuffer[8];
				writeSysParameterToFlash(CommEncryptKeyFlasAdd,tempbuf,8);
				WrBytesToAT24C64(Comm_EncryptKey_Addr,DatasBuffer,10);
				bitUpdateParameter=1;	
			}
			SerialUnion.S_DatasStruct.DatasLen=0;	
			break;
		case	RD_ENCRYPTKEY_COMD://����������Կ
			memcpy(SerialUnion.S_DatasStruct.Datas,CommEncryptKey,8);
			SerialUnion.S_DatasStruct.DatasLen=8;
			break;
		case	RD_CONSUMMONEY_COMD://�������Ѷ�
			if (!SerialUnion.S_DatasStruct.UartAddrL)//Aֵ
				SumMoney=Sys_SumConsumMoney;
			else if (SerialUnion.S_DatasStruct.UartAddrL==1)
			{//Lֵ
				SumMoney=0;
				ii=NoCollectRecordIndex;
				i=0;
				while (ii!=SaveRecordIndex)
				{
					Addr=(ulong)ii*RECORD_SIZE;
				//	PPage=*(1+(uchar *)&iii);
				//	memcpy((uchar *)&Addr,(2+(uchar *)&iii),2);
					Flash_Rd_Bytes(Addr,DatasBuffer,32);
					if (DatasBuffer[0]==0xa0 && !BytesCheckSum(DatasBuffer+1,31)&& !(DatasBuffer[1]&0x80))
						SumMoney+=ChgBCDStringToUlong(DatasBuffer+15,3);
					ii=(ii+1)%MAXRECORD_NUM;
					i++;
					if (!(i%64))
						SerialSendChar(STX);
				}
			}
			else if (SerialUnion.S_DatasStruct.UartAddrL==2 && SerialUnion.S_DatasStruct.DatasLen==2)
			{//Dֵ
				InitSaveRecordSub(0);
				SumMoney=0;
				ii=ReCollectRecordIndex;
				i=0;
				while (ii!=SaveRecordIndex)
				{
					Addr=(ulong)ii*RECORD_SIZE;
					//PPage=*(1+(uchar *)&iii);
					//memcpy((uchar *)&Addr,(2+(uchar *)&iii),2);
					Flash_Rd_Bytes(Addr,DatasBuffer,32);
					if (!DatasBuffer[0] || DatasBuffer[0]==0xa0)
					{
						if (!BytesCheckSum(DatasBuffer+1,31)&& !(DatasBuffer[1]&0x80))
						{
							ChgRecordDatasToTime(DatasBuffer+22,aaa);
							if ((!SerialUnion.S_DatasStruct.Datas[0]|| SerialUnion.S_DatasStruct.Datas[0]==aaa[1]) && 
								(!SerialUnion.S_DatasStruct.Datas[1]|| SerialUnion.S_DatasStruct.Datas[1]==aaa[2])	)
								SumMoney+=ChgBCDStringToUlong(DatasBuffer+15,3);
						}
					}
					i++;
					if (!(i%64))
						SerialSendChar(STX);
				}
			}
			else
				SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			SumMoney = FourBigToSmall(SumMoney);
			memcpy(SerialUnion.S_DatasStruct.Datas,(uchar *)&SumMoney,4);
			SumMoney = FourBigToSmall(SumMoney);
			SerialUnion.S_DatasStruct.DatasLen=4;
			break;
		case	CLR_NENU_COMD://��˵�
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==6)
			{
				CalComdSymblDatas(CLR_NENU_COMD,DatasBuffer);
				if (!memcmpself(SerialUnion.S_DatasStruct.Datas,DatasBuffer,6))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					Addr=MenuPrince_Addr;
					for (ii=0;ii<240*4;ii++)
					{
						if (!(ii%70))
						{
							SerialSendChar(STX); 
//							TCP_Send_STX();
						}						
						j=0xff;
						Clr_WatchDog();
						WrBytesToAT24C64(Addr++,&j,1);
						Clr_WatchDog();
					}
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	SET_MENU_COMD://���ò˵�
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;	
			if (SerialUnion.S_DatasStruct.UartAddrL<240 && !(SerialUnion.S_DatasStruct.DatasLen%3))
			{
				SerialUnion.S_DatasStruct.UartStatus=0;	
				if ((SerialUnion.S_DatasStruct.UartAddrL+(SerialUnion.S_DatasStruct.DatasLen/3) )<241)
					PPage=SerialUnion.S_DatasStruct.DatasLen/3;
				else
					PPage=241-SerialUnion.S_DatasStruct.UartAddrL;	
				Addr=MenuPrince_Addr+(uint)SerialUnion.S_DatasStruct.UartAddrL*4;
				for (i=0;i<PPage;i++)
				{
					memcpy(DatasBuffer,SerialUnion.S_DatasStruct.Datas+i*3,3);
					DatasBuffer[3]=CalCheckSum(DatasBuffer,3);
					WrBytesToAT24C64(Addr,DatasBuffer,4);
					memset(DatasBuffer,0,4);
					RdBytesFromAT24C64(Addr,DatasBuffer,4);
					Addr+=4;
					Clr_WatchDog();
				}
			}
			
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	RD_MENU_COMD://�����˵�
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;	
			if (SerialUnion.S_DatasStruct.UartAddrL<240)
			{
				SerialUnion.S_DatasStruct.UartStatus=0;	
				Addr=MenuPrince_Addr+(uint)SerialUnion.S_DatasStruct.UartAddrL*4;
				if ((241-SerialUnion.S_DatasStruct.UartAddrL)>32)
					status=32;
				else
					status=70-SerialUnion.S_DatasStruct.UartAddrL;
				PPage=0;
				for (i=0;i<status;i++)
				{
					Clr_WatchDog();
					RdBytesFromAT24C64((Addr+i*4),DatasBuffer,4);
					if (!BytesCheckSum(DatasBuffer,4) && !BCD_String_Diag(DatasBuffer,3))
					{
						memcpy(SerialUnion.S_DatasStruct.Datas,DatasBuffer,3);
						PPage+=3;	
					}
					else
						break;
				}
				if (PPage)
					SerialUnion.S_DatasStruct.DatasLen=PPage;
				else
					SerialUnion.S_DatasStruct.UartStatus=NoDatas_Error;
			}
			Clr_WatchDog();
			break;
		case	CLR_MENUNAME_COMD://�������
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==6)
			{
				CalComdSymblDatas(CLR_MENUNAME_COMD,DatasBuffer);
				if (!memcmpself(SerialUnion.S_DatasStruct.Datas,DatasBuffer,6))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					Erase_One_Sector(MenuName_Addr);
					Clr_WatchDog();
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	SET_MENUNAME_COMD://���ò���
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;	
			if (SerialUnion.S_DatasStruct.UartAddrL<70 && !(SerialUnion.S_DatasStruct.DatasLen%16))
			{
				SerialUnion.S_DatasStruct.UartStatus=0;	
				if ((SerialUnion.S_DatasStruct.UartAddrL+(SerialUnion.S_DatasStruct.DatasLen/16) )<71)
					PPage=SerialUnion.S_DatasStruct.DatasLen/16;
				else
					PPage=70-SerialUnion.S_DatasStruct.UartAddrL;	
				Addr=MenuName_Addr+(uint)SerialUnion.S_DatasStruct.UartAddrL*16;
				for (i=0;i<PPage;i++)
				{
					memcpy(DatasBuffer,SerialUnion.S_DatasStruct.Datas+i*16,16);
					Flash_Write_Bytes(Addr,DatasBuffer,16);
					Addr+=16;
					Clr_WatchDog();
				}
			}
			Clr_WatchDog();
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	CLR_SORTPRINCE_COMD://����۸񷽰�
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==6)
			{
				CalComdSymblDatas(CLR_SORTPRINCE_COMD,DatasBuffer);
				if (!memcmpself(SerialUnion.S_DatasStruct.Datas,DatasBuffer,6))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					Erase_One_Sector(SortPrince_Addr);
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	SET_SORTPRINCE_COMD://���ؼ۸񷽰�
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen && !(SerialUnion.S_DatasStruct.DatasLen%13))
			{
				SerialUnion.S_DatasStruct.UartStatus=0;	
				PPage=SerialUnion.S_DatasStruct.DatasLen/13;
				ii=SerialUnion.S_DatasStruct.UartAddrL+PPage;
				if (ii>200)
					PPage=(uint)200-SerialUnion.S_DatasStruct.UartAddrL;
				Addr=SortPrince_Addr+SerialUnion.S_DatasStruct.UartAddrL*16;
				for (i=0;i<PPage;i++)
				{
					DatasBuffer[0]=0xa0;
					memcpy(DatasBuffer+1,SerialUnion.S_DatasStruct.Datas+i*13,13);
					DatasBuffer[14]=CalCheckSum(DatasBuffer,14);
					Flash_Write_Bytes(Addr,DatasBuffer,15);
					Addr+=16;
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	RD_SORTPRINCE_COMD://�����۸񷽰�
			Addr=SortPrince_Addr+SerialUnion.S_DatasStruct.UartAddrL*16;
			if (((uint)256-SerialUnion.S_DatasStruct.UartAddrL)<8)
				PPage=(uint)256-SerialUnion.S_DatasStruct.UartAddrL;
			else
				PPage=8;
			SerialUnion.S_DatasStruct.DatasLen=0;
			for (i=0;i<PPage;i++)
			{
				Flash_Rd_Bytes(Addr,DatasBuffer,15);
				if (DatasBuffer[0]==0xa0 && !BytesCheckSum(DatasBuffer,15))
				{
					memcpy(SerialUnion.S_DatasStruct.Datas+i*13,DatasBuffer+1,13);
					SerialUnion.S_DatasStruct.DatasLen+=13;
				}
				else
					break;
			}
			break;

		case	RD_RECORD_COMD://�ϴ���¼
			ii=NoCollectRecordIndex;
			SerialSendNoCollectNum=0;
			if (SerialUnion.S_DatasStruct.DatasLen==1)
			{
				SerialUnion.S_DatasStruct.UartStatus=0;
				Nums=SerialUnion.S_DatasStruct.Datas[0];
				if (Nums>3)
					Nums=3;
				i=0;
				j=10;
				SerialUnion.S_DatasStruct.DatasLen=0;
				while( ( ii!=SaveRecordIndex ||  bitRecordFull ) && i<Nums && j)
				{
					Clr_WatchDog();	 //
					Addr=(ulong)ii*RECORD_SIZE;
					Force_Flash_Rd_Bytes(Addr,DatasBuffer,32);
					SerialSendNoCollectNum++;
					if (DatasBuffer[0]==0xa0 && !BytesCheckSum(DatasBuffer+1,31))
					{
						memcpy(SerialUnion.S_DatasStruct.Datas+SerialUnion.S_DatasStruct.DatasLen,DatasBuffer+1,31);
						SerialUnion.S_DatasStruct.DatasLen+=31;
						i++;
					}else
					{
						i = i;
					}
					ii=(ii+1)%MAXRECORD_NUM;
					j--;
				}
				if (!SerialUnion.S_DatasStruct.DatasLen)
				{
					if (ii==SaveRecordIndex)
						SerialUnion.S_DatasStruct.UartStatus=NoDatas_Error;//��¼�ɼ����
					else
					{
						InitSaveRecordSub(1);				
						SerialUnion.S_DatasStruct.UartStatus=Running_Status;//���ڲ�������		
					}
				}
			}
			break;
		case	DEL_RECORD_COMD://ɾ�����ϴ��ļ�¼
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==6)
			{
				CalComdSymblDatas(DEL_RECORD_COMD,DatasBuffer);
				if (!memcmpself(SerialUnion.S_DatasStruct.Datas,DatasBuffer,6))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					if (SerialSendNoCollectNum)
					{
						while (( NoCollectRecordIndex!=SaveRecordIndex || bitRecordFull) && SerialSendNoCollectNum )
						{
							Addr=(ulong)NoCollectRecordIndex*RECORD_SIZE;
							Flash_Rd_Bytes(Addr,DatasBuffer,32);
							if (DatasBuffer[0]==0xa0 && !BytesCheckSum(DatasBuffer+1,31))
							{
								bitHaveCollectRecord=1;	
								
								if (NoCollectRecordSum)			
									NoCollectRecordSum--;
							}
							else
								SerialSendChar(STX);
							DatasBuffer[0]=0;
							Force_Flash_Write_Bytes(Addr,DatasBuffer,1);
							SerialSendNoCollectNum--;
							NoCollectRecordIndex=(NoCollectRecordIndex+1)%MAXRECORD_NUM;
						}
					}
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	INIT_RECORD_PTR_COMD://��ʼ������ָ��
			InitSaveRecordSub(0);
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	RD_RERECORD_COMD://���ɼ�¼
			SerialSendReCollectNum=0;
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==3)
			{
				SerialUnion.S_DatasStruct.UartStatus=0;
				Nums=SerialUnion.S_DatasStruct.Datas[0];//�ɼ��ĸ���
				if (Nums>3)
					Nums=3;
				MonthChar=SerialUnion.S_DatasStruct.Datas[1];//�ɼ�����
				DayChar=SerialUnion.S_DatasStruct.Datas[2];//�ɼ�����
				ii=ReCollectRecordIndex;
				i=0;
				SerialUnion.S_DatasStruct.DatasLen=0;
				bitHaveReCollectRecord=0;
				j=10;
				while ((ii!=SaveRecordIndex ||bitRecordFull) && i<Nums && j)
				{
					Clr_WatchDog();
					//iii=(ulong)ii*RECORD_SIZE;
					Addr=(ulong)ii*RECORD_SIZE;
					//PPage=*(1+(uchar *)&iii);
					//memcpy((uchar *)&Addr,(2+(uchar *)&iii),2);
					Flash_Rd_Bytes(Addr,DatasBuffer,32);
					ChgRecordDatasToTime(DatasBuffer+22,aaa);
					if (  DatasBuffer[0]==0xa0 || ! DatasBuffer[0])
					{
						if (!BytesCheckSum(DatasBuffer+1,31))
						{
							if ((!MonthChar && !DayChar)|| ( MonthChar==aaa[1] && DayChar==aaa[2]))
							{
								memcpy(SerialUnion.S_DatasStruct.Datas+SerialUnion.S_DatasStruct.DatasLen,DatasBuffer+1,31);
								SerialUnion.S_DatasStruct.DatasLen+=31;
								i++;
							}
						}
					}
					ii=(ii+1)%MAXRECORD_NUM;
					SerialSendReCollectNum++;
					bitHaveReCollectRecord=1;
					j--;
				}
				if (!SerialUnion.S_DatasStruct.DatasLen)
				{
					if (ii==SaveRecordIndex)
						SerialUnion.S_DatasStruct.UartStatus=NoDatas_Error;//��¼�ɼ����
					else
						SerialUnion.S_DatasStruct.UartStatus=Running_Status;//���ڲ�������	
				}
			}
			break;
		case	DEL_RERECORD_COMD://������ָ��
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==6)
			{
				CalComdSymblDatas(DEL_RERECORD_COMD,DatasBuffer);
				if (!memcmpself(SerialUnion.S_DatasStruct.Datas,DatasBuffer,6))
				{
					SerialUnion.S_DatasStruct.UartStatus=0;
					if (bitHaveReCollectRecord)
					{
						bitHaveReCollectRecord=0;
						while (( ReCollectRecordIndex!=SaveRecordIndex || bitRecordFull)  &&  SerialSendReCollectNum )
						{
							ReCollectRecordIndex=(ReCollectRecordIndex+1)%MAXRECORD_NUM;
							SerialSendReCollectNum--;
					 	}
					}	
				}
			}
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
		case	LOAD_PROGAM_COMD://���س���
			SerialUnion.S_DatasStruct.UartStatus=ReDatas_Error;
			if (SerialUnion.S_DatasStruct.DatasLen==6)
			{
				CalComdSymblDatas(LOAD_PROGAM_COMD,DatasBuffer);
				if (!memcmpself(SerialUnion.S_DatasStruct.Datas,DatasBuffer,6))
				{
				
					bitNeedDownLoad=1;
					SerialUnion.S_DatasStruct.UartStatus=0;
				}
			}
			buf[0] = 0xab;
		//	WrBytesToAT24C64(LoadFlagAddr,buf,1);
			FLASH_Unlock();//����
			FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
			FLASH_ErasePage(DownFlagAddress);//������ҳ
			FLASH_ProgramHalfWord(DownFlagAddress,0x1234);
			downflag=(*(vu16*) DownFlagAddress);
			FLASH_Lock();//����
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
			
		case BTRECORD_COMD:  //�ϱ���¼
			clearoneRec();
			bitSerial_SendRequest = 0;
		getbtrec = 1;
			break;
		default:
			SerialUnion.S_DatasStruct.UartStatus=ReComd_Error;
			SerialUnion.S_DatasStruct.DatasLen=0;
			break;
	}
}
void	UART_SendSub(void)//�������ݴ���
{
	uchar	i;
	uint	Sum;
	uchar	CheckSum;
	ulong		iii;
	if (!bitSerial_SendRequest)
		return	;
	bitSerial_SendRequest=0;
	{
		u485_TE;
	//	SerialSendChar(0xff);
	//	SerialSendChar(0);

		SerialSendChar(STX);
		SerialUnion.S_DatasStruct.UartSeAddrCode=MainCode;
		SerialUnion.S_DatasStruct.UartReAddrCode=0;
		if (SerialUnion.S_DatasStruct.UartStatus)
			SerialUnion.S_DatasStruct.DatasLen=0;

		Receive_Data_Double ();//�����յ����ݴ�С��ת������
		Sum=Cal_CRC_Sub(SerialUnion.S_DatasBuffer,SerialUnion.S_DatasStruct.DatasLen+11);
		 	
		for (i=0;i<(SerialUnion.S_DatasStruct.DatasLen+11);i++)
			SerialAscToHex(&CheckSum,SerialUnion.S_DatasBuffer[i]);
		SerialSendChar(ETX);
		SerialAscToHex(&CheckSum,Sum/256);
		SerialAscToHex(&CheckSum,Sum%256);
		delay_ms(15);
	}
	Init_Serial();

	if (bitNeedRST)
		while (1);
	if (bitNeedDownLoad)
	{
		memcpy(DownLoadPragamSymbl,DownLoadDatas,6);
		bitSerial_SendRequest=0;//����Ҫ��Ӧ
		//EA=0;
		//JMP_Isp();
		while (1);
  	}
}
uchar	ReceiveDatasDiag(void)//�����������
{
	uchar	status;
		if (SerialReceiveLen<11)
			return	0xff;
		if (SerialUnion.S_DatasStruct.DatasLen!=SerialReceiveLen-11)
			return	0xff;

		if (SerialReCheckSum!=Cal_CRC_Sub(SerialUnion.S_DatasBuffer,SerialReceiveLen))
			return	0xff;
		else
			return	0;

}
void	SerialAscToHex(uchar * Sum,uchar aa )
{
	uchar	bb;
	bb=aa>>4;
	if (bb<10)
		bb+=0x30;
	else
		bb+=0x37;
	Sum[0]+=bb;
	SerialSendChar(bb);
	bb=aa&15;
	if (bb<10)
		bb+=0x30;
	else
		bb+=0x37;
	Sum[0]+=bb;
	SerialSendChar(bb);		
}
void	Chg_BlkNameSub(uchar bbit,ulong iii)//���Ӻ�����
{//bbit=0-���Ӻ�������1-���ٺ�����
	uint	Addr;
	uchar	 i,j;
	uchar	 st_data;
    //zjx_change1_mark
	Addr=(uint) (iii / 8);
    //-------------------
	i=iii%8;
	st_data=RdByte_24C256(Addr);
	j=1<<i;
	if (!bbit)
	{
		j=~j;
		st_data&=j;
	}
	else
		st_data|=j;
	WrByte_24C256(Addr,st_data);
}
void	CalComdSymblDatas(uchar Comd,uchar * Datas)
{
	uchar  i;
	for (i=0;i<6;i++)
		Datas[i]=Comd+0x11* i;
}

 

