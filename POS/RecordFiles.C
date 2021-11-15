#include 	"MyDefine.h"
#include	"ExternVariableDef.h"
#include	"ExternVoidDef.h"
#include	"485.h"
#include    "includes.h"

void	InitSaveRecordSub(uchar);//��ʼ��ָ��
uchar	RecordBlockDiag(uchar);//00 -�޼�¼ ;11-��ȫ��δ��; 22-ȫ��Ϊ���ɼ�¼
void	SaveRecordSub(uchar);//�洢���Ѽ�¼
uchar	RecordFullDiag(void);//��¼�洢�Ƿ��������
uchar	RecordDatasToBuffer(uchar,uchar *);//�洢��¼������д�뻺��
void	ChgTimeToRecordDatas(uchar * ,uchar * );
void	ChgRecordDatasToTime(uchar * ,uchar * );
void	SaveRecordBakSub(void);//��������¼�ı���
void	ClrConsumDatasBak(void);//���������¼
extern uchar isHaveBTRecord ;

void saveBTRecord(uchar * data)
{
	ulong		Addr = BT_RECORD_START;
	uchar		Buffer[32];
	uchar status;
	uchar i;
	//BT_RECORD_START
	
	while(1)
	{
		Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
		status=memcmpself(Buffer,AllFF,RECORD_SIZE);
		if (!status)
		{
			status=1;
			for (i=0;i<5;i++)
			{
				Flash_Write_Bytes(Addr,data,RECORD_SIZE);//д��
				Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);

				status=memcmpself(Buffer,data,RECORD_SIZE);
				if (!status)
				{
					break;
				}
			}
			if (!status)
				break;
		}
		if (status) //дʧ�� ���߲���ȫFF
		{
			Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
			if(Buffer[0] == 0xFF) //˵��������
			{
				memset(Buffer,0,RECORD_SIZE);
				Flash_Write_Bytes(Addr,Buffer,RECORD_SIZE);//д��
			} //���������Ӧ�ò�������������
		}
		
		Addr += RECORD_SIZE;
		
		if(Addr >= (BT_RECORD_START+0X1000))
		{
			return ;
		}
	}
	
	isHaveBTRecord = 1;
}


uchar getRecord(uchar * data)
{
	ulong		Addr = BT_RECORD_START;
	uchar		Buffer[32];
	uchar status;
	uchar i;
	uchar neederaseflash = 0;
	
	while(1)
	{
		Force_Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
		
		status=memcmpself(Buffer,AllFF,RECORD_SIZE);
		if (status)
		{
			neederaseflash = 1;
		}
		
		if(Buffer[0] == 0xa0 && !BytesCheckSum(Buffer+1,31))
		{
			memcpy(data,Buffer,32);
			return 1;
		}
		
		Addr += RECORD_SIZE;
		
		if(Addr >= (BT_RECORD_START+0X1000))
		{
			if(neederaseflash == 1)
			{
				Erase_One_Sector(Addr);
			}
			
			return 0;
		}
		
	}
}

void clearoneRec()
{
	ulong		Addr = BT_RECORD_START;
	uchar		Buffer[32];
	uchar status;
	uchar i;
	uchar neederaseflash = 0;
	
	while(1)
	{
		Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
		
		status=memcmpself(Buffer,AllFF,RECORD_SIZE);
		if (status)
		{
			neederaseflash = 1;
		}
		
		if(Buffer[0] == 0xa0 && !BytesCheckSum(Buffer+1,31))
		{
			Buffer[0] = 0;
			Flash_Write_Bytes(Addr,Buffer,1);//д��
			return;
		}
		
		Addr += RECORD_SIZE;
		
		if(Addr >= (BT_RECORD_START+0X1000))
		{
			
			return;
		}
		
	}
}


//ȡ���������ѵĴ洢��¼
uint	ClearSaveRecord(uchar bbit)//ȡ���洢���Ѽ�¼
{
	ulong		Addr;
	uchar		i;
//	ulong		iii;
	uchar		Buffer_SST25VF016[32];
	uchar		Buffer[32];
	uchar		status;

	while(1)
	{	
		if(SaveRecordIndex)
		{
			Addr=((ulong)SaveRecordIndex-1)	*RECORD_SIZE;
		}
		else
		{
			SaveRecordIndex = MAXRECORD_NUM-1;
		}
	//	RecordDatasToBuffer(bbit,Buffer_SST25VF016);
		Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
	//	status=memcmpself(Buffer,Buffer_SST25VF016,RECORD_SIZE);
		if (Buffer[0]== 0xa0)
		{
			memset(Buffer_SST25VF016,0,RECORD_SIZE)	;
			status=1;
			for (i=0;i<5;i++)
			{
				Flash_Write_Bytes(Addr,Buffer_SST25VF016,RECORD_SIZE);//д��
				Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);

				status=memcmpself(Buffer,Buffer_SST25VF016,RECORD_SIZE);
				if (!status)
				{
					PosConsumCount--;
					break;
				}
			}
			if (!status)
				break;
			else
				return PSAM_MAC1_FAIL;
		}
		else
		{
			return PSAM_MAC1_FAIL;
		}
		Clr_WatchDog();	
	}
	InitSaveRecordSub(1);
	bitNeedDiagRecordFull=1;
	return 0 ;	
}

extern u8 ConsumChar;
//�洢���Ѽ�¼
void	SaveRecordSub(uchar bbit)//�洢���Ѽ�¼
{
	ulong		Addr;
	uchar		i;
//	ulong		iii;
	uchar		Buffer_SST25VF016[32];
	uchar		Buffer[32];
	uchar		status;
	
	if(!ConsumChar)
		ClrConsumDatasBak();
	
	if (RecordDatasToBuffer(bbit,Buffer_SST25VF016))
		return;
	if(WriteCardSta)//2/3 
	{
		ClrConsumDatasBak();//�����洢��¼����
		WriteCardSta=0;
	}
	if(!ConsumChar)
	{
		bitConsumZeroEnable=0;
		Sys_SumConsumMoney+=SumConsumMoney;
		bitNeedPrinter=1;
		SumPrintMoney+=SumConsumMoney;
	}
	while(1)
	{
		Addr=(ulong)SaveRecordIndex	*RECORD_SIZE;
		Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
		status=memcmpself(Buffer,AllFF,RECORD_SIZE);
		if (!status)
		{
			status=1;
			for (i=0;i<5;i++)
			{
				Flash_Write_Bytes(Addr,Buffer_SST25VF016,RECORD_SIZE);//д��
				Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
			//	USART_PUTS_Arry(1,Buffer,RECORD_SIZE);//adlc����ʱ��ʹ��

				status=memcmpself(Buffer,Buffer_SST25VF016,RECORD_SIZE);
				if (!status)
				{
					PosConsumCount++;
					break;
				}
			}
			if (!status)
				break;
		}
		if (status) //дʧ�� ���߲���ȫFF
		{
			Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
			if(Buffer[0] == 0xFF) //˵��������
			{
				memset(Buffer,0,RECORD_SIZE);
				Flash_Write_Bytes(Addr,Buffer,RECORD_SIZE);//д��
			} //���������Ӧ�ò�������������
			SaveRecordIndex++;
			SaveRecordIndex=SaveRecordIndex%MAXRECORD_NUM;	
			if (RecordFullDiag())
				return;
		}
		Clr_WatchDog();	
	}
	InitSaveRecordSub(1);
	bitNeedDiagRecordFull=1;	
}

extern uchar	PurseBTNumNew[2];//�µĲ������к�
u8 isclearbt = 0;

uchar	RecordDatasToBuffer(uchar bbit,uchar * ptr)//�洢��¼������д�뻺��
{
	uchar	aaa[4];
	uint	ii;
	ulong	iii;
	ulong	SumMoney;
	if (!bbit)
		SumMoney=SumConsumMoney;
	else
	{
		if(!ConsumChar)
			SumMoney=CurrentConsumMoney;
	   else
			SumMoney=SumConsumMoney;
	}
	if (!SumMoney && !bitConsumZeroEnable && !ConsumChar && ConsumMode!=CONSUM_RATION)
		return 1;	

	ptr[1]=ConsumChar;
	if (bbit)
		ptr[1]|=0x80;
	
	ptr[0]=0xa0;	
	
	
	ptr[3]=PurseUsingNum;//Ǯ������
	memcpy(ptr+4,CardSerialNumBak,4);//����Ψһ���к�
	memcpy(ptr+8,CardPrinterNum+1,3);//ӡˢ���

	if(!ConsumChar)
	{
		if (ConsumMode==CONSUM_NUM)
		{
			if (MenuSort>70)
				MenuSort=0;	
			ptr[2]=MenuSort;
		}
		else if (ConsumMode==CONSUM_RATION)
			ptr[2]=0;//�˺�
		else
			ptr[2]=0xfe;//�˺�
		
		if(SumMoney)//���޴�
		{
			ptr[3]=PurseUsingNum;//Ǯ������

			memcpy(ptr+11,OldBalance,4);//ԭ���

			ChgUlongToBCDString(SumMoney,aaa,4);
			memcpy(ptr+15,aaa+1,3);//���Ѷ�

			iii=ChgBCDStringToUlong(OldBalance,4);
			iii-=SumMoney;
			ChgUlongToBCDString(iii,ptr+18,4);//�����

			ii=GetU16_HiLo(PurseConsumCount)+1;
			ii = DoubleBigToSmall(ii);
			memcpy(ptr+26,(uchar *)&ii,2);//Ǯ�������Ѵ���
			
			
			PosConsumCount = DoubleBigToSmall(PosConsumCount);	//adlc
			memcpy(ptr+28,(uchar *)&PosConsumCount,2);//POS����ˮ��
			PosConsumCount = DoubleBigToSmall(PosConsumCount);	//adlc
		}
		else
		{
			if(ConsumMode==CONSUM_RATION && !CurrentConsumMoney) {
				ptr[3]=PurseUsingNum;//Ǯ������

				memcpy(ptr+11,OldBalance,4);//ԭ���

				ChgUlongToBCDString(SumMoney,aaa,4);
				memcpy(ptr+15,aaa+1,3);//���Ѷ�

				iii=ChgBCDStringToUlong(OldBalance,4);
				iii-=SumMoney;
				ChgUlongToBCDString(iii,ptr+18,4);//�����

				ii=GetU16_HiLo(PurseConsumCount)+1;
				ii = DoubleBigToSmall(ii);
				ii = 0;
				memcpy(ptr+26,(uchar *)&ii,2);//Ǯ�������Ѵ���
				
				
				PosConsumCount = DoubleBigToSmall(PosConsumCount);	//adlc
				memcpy(ptr+28,(uchar *)&PosConsumCount,2);//POS����ˮ��
				PosConsumCount = DoubleBigToSmall(PosConsumCount);	//adlc
			} else {
				ptr[3]=0;//Ǯ������
				memset(ptr+11,0,11);
				memset(ptr+26,0,4);
			}
		}
	} else {
		ptr[2]=0;
		memcpy(ptr+11,OldBalance,4);//ԭ���
		ChgUlongToBCDString(SumMoney,aaa,4); 
		memcpy(ptr+15,aaa+1,3);//���Ѷ�

		memcpy(ptr+18,NewBalance,4);//�����
		
		ii=GetU16_HiLo(PurseConsumCount);
		//if (SumMoney)
		ii++;
		
		if(isclearbt) ii++;
		
		ii = DoubleBigToSmall(ii);
		memcpy(ptr+26,(uchar *)&ii,2);//Ǯ�������Ѵ���
		
		if (ConsumChar==4)
			memcpy(ptr+28,PurseContrlNum,2);//POS����ˮ��
		else
			memcpy(ptr+28,PurseBTNumNew,2);//�µ�POS����ˮ��
		
	}
	bitPF8563Error=CheckPF8563();//����ʱ�Ӳ����Ϸ���
	//if (bitPF8563Error)
	memcpy(CardConsumTime,SysTimeDatasBak.TimeString,6);
	ChgTimeToRecordDatas(CardConsumTime,ptr+22);
 
	ptr[30]=0xff;
	ptr[31]=CalCheckSum(ptr+1,30);
	return	0;
}

void	SaveRecordBakSub(void)//�������Ѽ�¼�ı���
{
	ulong		Addr;
//	uchar		PPage;
//	ulong		iii;
	uint		ii;
	uchar		st_data;
	uchar		Buffer_39SF020[RECORD_SIZE];
	uchar		Buffer[RECORD_SIZE];
	for (ii=0;ii<RECORDNUM_BLOCK;ii++)
	{//�ж��ݴ����Ƿ��м�¼
		Addr=(ulong)SAVERECORD_BAK_USEBLOCK*0x1000+(ulong)ii*RECORD_SIZE;//SST020���һ�飨4K��
        Flash_Rd_Bytes(Addr,Buffer_39SF020,RECORD_SIZE);
		if (Buffer_39SF020[0]==0xa0 && !BytesCheckSum(Buffer_39SF020+1,RECORD_SIZE-1))
		{   
		   if(Buffer_39SF020[1]==0x90) //�ۼ�ʧ��
		   		Buffer_39SF020[1]=0;
		   Addr=(ulong)SaveRecordIndex * RECORD_SIZE;
		   Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
		   st_data=memcmpself(Buffer,AllFF,32);
		   if (!st_data)
		   Flash_Write_Bytes(Addr,Buffer_39SF020,RECORD_SIZE);//д��
		   InitSaveRecordSub(0);//���¶�λ�ɼ�ָ��
		   ClrConsumDatasBak();//�����洢��¼����
		   break;//ֻ��һ��
		}
		else if (Buffer_39SF020[0]==0xff)//�ռ�¼ʱֱ������
			break;
	}

}
void	FindPosConsumCountSub(void)//�������һ����¼����ˮ��
{
//	ulong	iii;
//	uchar	PPage;
	ulong	Addr;
	uint	ii;
	uchar	Buffer[32];
	if (!SaveRecordIndex)
		ii=MAXRECORD_NUM-1;
	else
		ii=SaveRecordIndex-1;
	Addr=(ulong)ii*RECORD_SIZE;
	//PPage=*(1+(uchar *)&iii);
	//memcpy((uchar *)&Addr,(2+(uchar *)&iii),2);	
	Flash_Rd_Bytes(Addr,Buffer,32);
	if (Buffer[0]!=0xff && !BytesCheckSum(Buffer+1,31))
	{
		memcpy((uchar *)&PosConsumCount,Buffer+28,2);
		PosConsumCount = DoubleBigToSmall(PosConsumCount);//ADlc
	 }
	else
		PosConsumCount=0;//��flash��ˮ�Ŵ�0��ʼ��
	PosConsumCount++;
}
void	ClrConsumDatasBak(void)
{
	uint	ii;
	ulong	Addr;
	uchar	st_data;
	for (ii=0;ii<RECORDNUM_BLOCK;ii++)
	{
		Addr=(ulong)SAVERECORD_BAK_USEBLOCK*0x1000+(ulong)ii*RECORD_SIZE;
		Flash_Rd_Bytes(Addr,&st_data,1);	
		if (st_data==0xa0)
		{
			st_data=0;
			Flash_Write_Bytes(Addr,&st_data,1);
		}	
		else if (st_data==0xff)
			break;
		Addr+=RECORD_SIZE;
	}
}uchar		RecordFullDiag(void)//��¼�洢�Ƿ��������	
{
	uchar		st_data;
//	uchar		PPage;
	ulong		Addr;
	uchar		BlockNum;
	uchar		ReBlockNum;
	uint		ii;
	uchar		status;
	SaveRecordIndex=SaveRecordIndex%MAXRECORD_NUM;
	st_data=(uchar)SaveRecordIndex;//adlc
 	//if ((st_data&0x7f)<0x70) //��֤��һ����ȫFF ���������Ҽ�¼ʱ��ȫF
	//	return	0;
	BlockNum=SaveRecordIndex/RECORDNUM_BLOCK;
	BlockNum=(BlockNum+1)%RECORD_BLOCK_NUM;
	Addr=(ulong)BlockNum*0x1000;
	status=0;
	for (ii=0;ii<RECORDNUM_BLOCK;ii++)
	{
		Force_Flash_Rd_Bytes(Addr,&st_data,1);
		if (st_data==0xa0) //ֻҪ��һ�� δ�� ��������һ���м�¼
		{
			status=1;
			return	1;
		}
		else
		{
			if (st_data!=0xff)
				status=2;
		}
		Addr+=RECORD_SIZE;
	}
	if (status)	//��һ�鶼����δ��
	{
		Addr=(ulong)BlockNum*0x1000;
		Erase_One_Sector(Addr);
		ReBlockNum=ReCollectRecordIndex/RECORDNUM_BLOCK;
		if (BlockNum==ReBlockNum)//�������ָ�����ð����ڱ������Ŀ��ڣ����¶�λ����ָ��,
		{
			InitSaveRecordSub(0);
			SerialSendReCollectNum=0;
		}
	}
	return	0;
}

void InitSaveRecordSub(uchar bbit) //��ʼ��ָ��
{
	int i=0,j=0;
	uchar status;
	ulong Addr;
	uchar tmpdata[5];
	int endBlock = 0;
	ulong startfindpose = 0;
	uchar isFindNoReCollect = 0;
	uchar isFindNoCollect = 0;
	uchar isFindSaveIndex = 0;
	ulong NowPos = 0;
	

	RecordSum=0;//��¼����
	NoCollectRecordSum=0;//û�вɼ��ļ�¼����
	NoCollectRecordIndex=0;//û�вɼ��ļ�¼ָ��
	SaveRecordIndex=0;//�洢��¼ָ��
	if (!bbit)
		ReCollectRecordIndex=0;//�Ѿ��ɼ��ļ�¼ָ��
	
	for(i=0;i<RECORD_BLOCK_NUM;i++)
	{
		status=RecordBlockDiag(i);
		if(status == 0) //�ҵ�ȫFF�Ŀ�
		{
			for(j=0;j<RECORDNUM_BLOCK;j++)
			{
				Addr = (((ulong)i) * 0x1000) + ((ulong)j) * RECORD_SIZE;
				Force_Flash_Rd_Bytes(Addr, tmpdata, 1);
				if(tmpdata[0] != 0xFF)
				{
					break;
				}
			}
			if(j == RECORDNUM_BLOCK) //ȷ�������ȫFF
			{
				endBlock = i + 1;
				endBlock = endBlock % RECORD_BLOCK_NUM;
				break;
			}
		}
	}

	if(i == RECORD_BLOCK_NUM) //û���ҵ�ȫFF�Ŀ�
	{
		endBlock = 0;
	}

	NowPos = endBlock;
	NowPos *= RECORDNUM_BLOCK;


	//��ʼ��δ�ɼ�¼ָ�롢���ɼ�¼ָ�롢�洢��¼ָ��
	for(i=0;i<MAXRECORD_NUM;i++)
	{
		Addr = NowPos * RECORD_SIZE;
		Force_Flash_Rd_Bytes(Addr, tmpdata, 1);
		if(tmpdata[0] != 0xFF)
		{
			if(!bbit) //��Ҫ��ʼ������
			{
				if(isFindNoReCollect == 0)
				{
					isFindNoReCollect = 1;
					ReCollectRecordIndex = NowPos;
				}
			}

			if(tmpdata[0] == 0xa0) //�ҵ��˼�¼
			{
				if(isFindNoCollect == 0)
				{
					isFindNoCollect = 1;
					NoCollectRecordIndex = NowPos;
				}
				NoCollectRecordSum++;
			}
			
			if(tmpdata[0] == 0 || tmpdata[0] == 0xa0)
			{
				RecordSum++;
			}
		}
		
		NowPos++;
		NowPos %= MAXRECORD_NUM;
		
		
	}

	if(endBlock == 0)
	{
		endBlock = RECORD_BLOCK_NUM - 1;
	}else
	{
		endBlock--;
	}
	
	//��ʼ�Ҵ洢ָ��
	NowPos = endBlock;
	NowPos *= RECORDNUM_BLOCK;
	if(NowPos == 0)
	{
		NowPos = MAXRECORD_NUM - 1;
	}else
	{
		NowPos -= 1;
	}
	
	
	for(i=0;i<RECORDNUM_BLOCK;i++)
	{
		Addr = NowPos * RECORD_SIZE;
		Force_Flash_Rd_Bytes(Addr, tmpdata, 1);
		if(tmpdata[0] == 0xFF)
		{
			isFindSaveIndex = 1;
			SaveRecordIndex = NowPos;
			//break;
		}else
		{
			break;
		}
		NowPos--;
	}

	if(isFindSaveIndex == 0)
	{
		NowPos = endBlock;
		NowPos *= RECORDNUM_BLOCK;
		SaveRecordIndex = NowPos;
	}
	
	if(isFindNoCollect == 0)
	{
		NoCollectRecordIndex = SaveRecordIndex;
	}
	
}


void	InitSaveRecordSubbak(uchar bbit )//��ʼ��ָ��
{
 	uchar  			i;
//	uchar 			PPage;
	ulong			Addr,jj;
	uchar			st_data;
	uchar			status;
	uchar			EndBlock;
	uchar			StartBlock;
	uchar 		bitFindReIndex=0; //���ɼ�¼ָ��
	uchar		bitFindNoIndex=0;//δ��ָ��
	uchar		bitFindSymbl=0; 
	RecordSum=0;//��¼����
	NoCollectRecordSum=0;//û�вɼ��ļ�¼����
	NoCollectRecordIndex=0;//û�вɼ��ļ�¼ָ��
	if (!bbit)
		ReCollectRecordIndex=0;//�Ѿ��ɼ��ļ�¼ָ��
	SaveRecordIndex=0;//�洢��¼ָ��
	for (i=0;i<RECORD_BLOCK_NUM;i++)
	{//�ҳ���¼����ʼ��
		status=RecordBlockDiag(i);
		if ( (status&0xf0)&& !(status&0x0f))
		{//�ÿ������м�¼��δ������һ��Ϊ��ʼ��¼��λ��
			bitFindSymbl=1;
			EndBlock=i;
			break;
		}
		else if ((status&0xf0) && (status&0x0f))
		{//����ÿ������������һ��ļ�¼
			status=RecordBlockDiag((i+1)%RECORD_BLOCK_NUM);
			if (!status)
			{//�����һ����¼Ϊ��,����һ��Ϊ��ʼ��¼��ʼ��
				bitFindSymbl=1;
				EndBlock=i;
				break;
			}
		}
	}
	if (!bitFindSymbl)//�����ڼ�¼������
	{//û���ҵ��м�¼�Ŀ�
		status=RecordBlockDiag(0);	
		if (!status)//�����һ��Ϊ�գ���û�д洢��¼
			return;//�޼�¼
		else//�����һ��Ϊ��, 
		{
			for (i=0;i<RECORD_BLOCK_NUM;i++)
			{//���ĳ�麬��δ�ɼ�¼���¿�Ϊ�ɼ����ļ�¼����ÿ�Ϊ��ʼ��
				status=RecordBlockDiag(i);	
				if (status==0x11 || status==0x21)
				{
					status=RecordBlockDiag((i+1)%RECORD_BLOCK_NUM);	
					if (status==0x22)
					{
						bitFindSymbl=1;
						EndBlock=i;
						break;
					}
				}
			}
			if (!bitFindSymbl)
				EndBlock=RECORD_BLOCK_NUM-1;//���ȫ��Ϊδ�ɼ�¼��ɼ����ļ�¼����1��Ϊ��ʼ
		}		
	}
	//��¼�洢ָ��
	st_data=RecordBlockDiag(EndBlock);
	if ((st_data&0xf0) && (st_data&0x0f) )
	{//������һ��ļ�¼�������¿����ʼ��ַΪ�洢ָ��
		if (EndBlock==RECORD_BLOCK_NUM-1)
			SaveRecordIndex=0;//�����¼����Ϊ���һ������һ����¼�����¼�洢ָ��Ϊ0
		else
			SaveRecordIndex=(EndBlock+1)*RECORDNUM_BLOCK;//����Ϊ��һ��ĵ�һ��		
	}
	else
	{//��ǰ��û�������ҳ��´δ洢��¼��������
		SaveRecordIndex=EndBlock*RECORDNUM_BLOCK;	
		Addr=(ulong)EndBlock*0x1000;
		for (jj=0;jj<RECORDNUM_BLOCK;jj++)
		{
			Flash_Rd_Bytes(Addr,&st_data,1);//��ĵ�һ����¼
			Addr+=RECORD_SIZE;
			if (st_data==0xff)
			{//�ҵ��洢��¼�ĳ���
				SaveRecordIndex+=jj;
				break;
			}
		}	
	}
	NoCollectRecordIndex=SaveRecordIndex;
///////////////////////���е��ˣ��ҵ���¼�ĳ��ڣ����´δ洢��¼��ָ��//////////////////////?
	StartBlock=(EndBlock+1)%RECORD_BLOCK_NUM;//�¿�Ϊ��¼�����
	for (i=0;i<RECORD_BLOCK_NUM;i++)
	{//�Ӽ�¼�Ŀ�ʼλ�ü���
		status=RecordBlockDiag(StartBlock);	
		switch (status)
		{
			case	0x10:// ��ʼδ��,��Ϊ��
				if (!bitFindReIndex)
				{//���δ�ҵ�������ڣ��ø�λ��Ϊ�������
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				if (!bitFindNoIndex)
				{//���δ�ҵ��ɼ���ڣ��ø�λ��Ϊ�ɼ����
					bitFindNoIndex=1;
					NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				Addr=(ulong)StartBlock*0x1000;
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//��ĵ�һ����¼
					Addr+=RECORD_SIZE;
					if (st_data!=0xff)
					{
						RecordSum++;//��¼����
						NoCollectRecordSum++;//δ�ɼ�¼����	
					}
					else
						break;
				}
				break;
			case	0x11://��ʼ�������δ�ɼ�
				if (!bitFindReIndex)
				{//���δ�ҵ�������ڣ��ø�λ��Ϊ�������
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				if (!bitFindNoIndex)
				{//���δ�ҵ��ɼ���ڣ��ø�λ��Ϊ�ɼ����
					bitFindNoIndex=1;
					NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				RecordSum+=RECORDNUM_BLOCK;
				NoCollectRecordSum+=RECORDNUM_BLOCK;
				break;
			case	0x20: //��ʼ�Ѳɼ�������޼�¼ 
				if (!bitFindReIndex)
				{//���δ�ҵ�������ڣ��ø�λ��Ϊ�������
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				Addr=(ulong)StartBlock*0x1000;			
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//��ĵ�һ����¼
					Addr+=RECORD_SIZE;
					if (st_data!=0xff)
					{
						RecordSum++;//��¼����
						if (st_data==0xa0)
						{
							NoCollectRecordSum++;
							if (!bitFindNoIndex)
							{//���δ�ҵ��ɼ���ڣ��ø�λ��Ϊ�ɼ����
								bitFindNoIndex=1;
								NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK+jj;	
							}	
						}
				   	}
					else
						break;
				}
				break;
			case	0x21://��ʼ�Ѳɼ������δ�ɼ�
				if (!bitFindReIndex)
				{//���δ�ҵ�������ڣ��ø�λ��Ϊ�������
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}			
				RecordSum+=RECORDNUM_BLOCK;
			//	Addr=(StartBlock&15)*0x1000;
				Addr=(ulong)StartBlock*0x1000;
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)	
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//��ĵ�һ����¼
					Addr+=RECORD_SIZE;
					if (st_data==0xa0)
					{
						NoCollectRecordSum++;
						if (!bitFindNoIndex)
						{//���δ�ҵ��ɼ���ڣ��ø�λ��Ϊ�ɼ����
							bitFindNoIndex=1;
							NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK+jj;	
						}
					}					
				}
				break;
			case	0x22://��һ�������һ�����ɼ���
				if (!bitFindReIndex)
				{//���δ�ҵ�������ڣ��ø�λ��Ϊ�������
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				RecordSum+=RECORDNUM_BLOCK;
				break;
			default:
				break;											
		}
		StartBlock=(StartBlock+1)%RECORD_BLOCK_NUM;
	}
//���ϳ����ҵ��˿��ż�¼�ļ�¼���������ɼ�¼����ָ�룬�����һ�����Ѽ�¼
}	
uchar	RecordBlockDiag(uchar BlockNum)
//�ÿ��¼�洢��� ����39SF020--Ϊ0-63��
//00 -�޼�¼ ;11-��ȫ��δ��; 2-���ɼ�¼
{
	ulong		Addr;
	uchar		st_data;
	uchar		status=0;

	Addr=(ulong)BlockNum*0x1000;
	Flash_Rd_Bytes(Addr,&st_data,1);
	if (st_data==0xa0)
		status=0x10;
	else if (st_data==0xff)
		status=0;
	else
		status=0x20;
	Addr=Addr+0x1000-RECORD_SIZE;
	Flash_Rd_Bytes(Addr,&st_data,1);
	if (st_data==0xa0)
		status+=1;
	else if (st_data==0xff)
		status&=0xf0;
	else
		status+=2;
	return	status;	
}

void	ChgTimeToRecordDatas(uchar * Timeptr,uchar * ptr)
{
	uchar	aa,bb;
	aa=BCDToHex(Timeptr[0]);//��
	aa<<=2;
	bb=BCDToHex(Timeptr[1]);//��
	bb&=0x0f;
	ptr[0]=aa+(bb>>2);
	aa=BCDToHex(Timeptr[2]);//��
	ptr[1]=bb<<6;
	aa<<=1;
	ptr[1]+=aa;
	aa=BCDToHex(Timeptr[3]);//ʱ
	if (aa>=16)
		ptr[1]++;
	aa&=0x0f;
	ptr[2]=aa<<4;
	aa=BCDToHex(Timeptr[4]);//��
	ptr[2]+=(aa>>2);
	bb=BCDToHex(Timeptr[5]);//��
	ptr[3]=aa<<6;
	ptr[3]+=bb;		
}
void	ChgRecordDatasToTime(uchar * ptr ,uchar * Timeptr )
{
	uchar	aa,bb;
	aa=ptr[0];
	Timeptr[0]=HexToBCD(aa>>2);//��
	aa&=3;
	aa<<=2;
	bb=ptr[1];
	aa+=(bb>>6);
	Timeptr[1]=HexToBCD(aa);//��
	aa=bb>>1;
	aa&=0x1f;
	Timeptr[2]=HexToBCD(aa);//��
	bb&=1;
	bb<<=4;
	aa=ptr[2];
	aa>>=4;
	Timeptr[3]=HexToBCD(bb+aa);//ʱ
	aa=ptr[2];
	aa&=0x0f;
	aa<<=2;
	bb=ptr[3];
	aa+=(bb>>6);
	Timeptr[4]=HexToBCD(aa);//��
	Timeptr[5]=HexToBCD(bb&0x3f);
}


