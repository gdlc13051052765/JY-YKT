#include 	"MyDefine.h"
#include	"ExternVariableDef.h"
#include	"ExternVoidDef.h"
#include	"485.h"
#include    "includes.h"

void	InitSaveRecordSub(uchar);//初始化指针
uchar	RecordBlockDiag(uchar);//00 -无记录 ;11-满全部未采; 22-全部为复采记录
void	SaveRecordSub(uchar);//存储消费记录
uchar	RecordFullDiag(void);//记录存储是否已满诊断
uchar	RecordDatasToBuffer(uchar,uchar *);//存储记录的内容写入缓存
void	ChgTimeToRecordDatas(uchar * ,uchar * );
void	ChgRecordDatasToTime(uchar * ,uchar * );
void	SaveRecordBakSub(void);//保存错误记录的备份
void	ClrConsumDatasBak(void);//擦除错误记录
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
				Flash_Write_Bytes(Addr,data,RECORD_SIZE);//写入
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
		if (status) //写失败 或者不是全FF
		{
			Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
			if(Buffer[0] == 0xFF) //说明有跳变
			{
				memset(Buffer,0,RECORD_SIZE);
				Flash_Write_Bytes(Addr,Buffer,RECORD_SIZE);//写入
			} //其他情况不应该擦除，跳过即可
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
			Flash_Write_Bytes(Addr,Buffer,1);//写入
			return;
		}
		
		Addr += RECORD_SIZE;
		
		if(Addr >= (BT_RECORD_START+0X1000))
		{
			
			return;
		}
		
	}
}


//取消本次消费的存储记录
uint	ClearSaveRecord(uchar bbit)//取消存储消费记录
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
				Flash_Write_Bytes(Addr,Buffer_SST25VF016,RECORD_SIZE);//写入
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
//存储消费记录
void	SaveRecordSub(uchar bbit)//存储消费记录
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
		ClrConsumDatasBak();//擦除存储记录备份
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
				Flash_Write_Bytes(Addr,Buffer_SST25VF016,RECORD_SIZE);//写入
				Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
			//	USART_PUTS_Arry(1,Buffer,RECORD_SIZE);//adlc测试时候使用

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
		if (status) //写失败 或者不是全FF
		{
			Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
			if(Buffer[0] == 0xFF) //说明有跳变
			{
				memset(Buffer,0,RECORD_SIZE);
				Flash_Write_Bytes(Addr,Buffer,RECORD_SIZE);//写入
			} //其他情况不应该擦除，跳过即可
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

extern uchar	PurseBTNumNew[2];//新的补贴序列号
u8 isclearbt = 0;

uchar	RecordDatasToBuffer(uchar bbit,uchar * ptr)//存储记录的内容写入缓存
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
	
	
	ptr[3]=PurseUsingNum;//钱包代号
	memcpy(ptr+4,CardSerialNumBak,4);//卡的唯一序列号
	memcpy(ptr+8,CardPrinterNum+1,3);//印刷编号

	if(!ConsumChar)
	{
		if (ConsumMode==CONSUM_NUM)
		{
			if (MenuSort>70)
				MenuSort=0;	
			ptr[2]=MenuSort;
		}
		else if (ConsumMode==CONSUM_RATION)
			ptr[2]=0;//菜号
		else
			ptr[2]=0xfe;//菜号
		
		if(SumMoney)//非限次
		{
			ptr[3]=PurseUsingNum;//钱包代号

			memcpy(ptr+11,OldBalance,4);//原余额

			ChgUlongToBCDString(SumMoney,aaa,4);
			memcpy(ptr+15,aaa+1,3);//消费额

			iii=ChgBCDStringToUlong(OldBalance,4);
			iii-=SumMoney;
			ChgUlongToBCDString(iii,ptr+18,4);//新余额

			ii=GetU16_HiLo(PurseConsumCount)+1;
			ii = DoubleBigToSmall(ii);
			memcpy(ptr+26,(uchar *)&ii,2);//钱包的消费次数
			
			
			PosConsumCount = DoubleBigToSmall(PosConsumCount);	//adlc
			memcpy(ptr+28,(uchar *)&PosConsumCount,2);//POS的流水号
			PosConsumCount = DoubleBigToSmall(PosConsumCount);	//adlc
		}
		else
		{
			if(ConsumMode==CONSUM_RATION && !CurrentConsumMoney) {
				ptr[3]=PurseUsingNum;//钱包代号

				memcpy(ptr+11,OldBalance,4);//原余额

				ChgUlongToBCDString(SumMoney,aaa,4);
				memcpy(ptr+15,aaa+1,3);//消费额

				iii=ChgBCDStringToUlong(OldBalance,4);
				iii-=SumMoney;
				ChgUlongToBCDString(iii,ptr+18,4);//新余额

				ii=GetU16_HiLo(PurseConsumCount)+1;
				ii = DoubleBigToSmall(ii);
				ii = 0;
				memcpy(ptr+26,(uchar *)&ii,2);//钱包的消费次数
				
				
				PosConsumCount = DoubleBigToSmall(PosConsumCount);	//adlc
				memcpy(ptr+28,(uchar *)&PosConsumCount,2);//POS的流水号
				PosConsumCount = DoubleBigToSmall(PosConsumCount);	//adlc
			} else {
				ptr[3]=0;//钱包代号
				memset(ptr+11,0,11);
				memset(ptr+26,0,4);
			}
		}
	} else {
		ptr[2]=0;
		memcpy(ptr+11,OldBalance,4);//原余额
		ChgUlongToBCDString(SumMoney,aaa,4); 
		memcpy(ptr+15,aaa+1,3);//消费额

		memcpy(ptr+18,NewBalance,4);//新余额
		
		ii=GetU16_HiLo(PurseConsumCount);
		//if (SumMoney)
		ii++;
		
		if(isclearbt) ii++;
		
		ii = DoubleBigToSmall(ii);
		memcpy(ptr+26,(uchar *)&ii,2);//钱包的消费次数
		
		if (ConsumChar==4)
			memcpy(ptr+28,PurseContrlNum,2);//POS的流水号
		else
			memcpy(ptr+28,PurseBTNumNew,2);//新的POS的流水号
		
	}
	bitPF8563Error=CheckPF8563();//读出时钟并监测合法性
	//if (bitPF8563Error)
	memcpy(CardConsumTime,SysTimeDatasBak.TimeString,6);
	ChgTimeToRecordDatas(CardConsumTime,ptr+22);
 
	ptr[30]=0xff;
	ptr[31]=CalCheckSum(ptr+1,30);
	return	0;
}

void	SaveRecordBakSub(void)//保存消费记录的备份
{
	ulong		Addr;
//	uchar		PPage;
//	ulong		iii;
	uint		ii;
	uchar		st_data;
	uchar		Buffer_39SF020[RECORD_SIZE];
	uchar		Buffer[RECORD_SIZE];
	for (ii=0;ii<RECORDNUM_BLOCK;ii++)
	{//判断暂存区是否有记录
		Addr=(ulong)SAVERECORD_BAK_USEBLOCK*0x1000+(ulong)ii*RECORD_SIZE;//SST020最后一块（4K）
        Flash_Rd_Bytes(Addr,Buffer_39SF020,RECORD_SIZE);
		if (Buffer_39SF020[0]==0xa0 && !BytesCheckSum(Buffer_39SF020+1,RECORD_SIZE-1))
		{   
		   if(Buffer_39SF020[1]==0x90) //累计失败
		   		Buffer_39SF020[1]=0;
		   Addr=(ulong)SaveRecordIndex * RECORD_SIZE;
		   Flash_Rd_Bytes(Addr,Buffer,RECORD_SIZE);
		   st_data=memcmpself(Buffer,AllFF,32);
		   if (!st_data)
		   Flash_Write_Bytes(Addr,Buffer_39SF020,RECORD_SIZE);//写入
		   InitSaveRecordSub(0);//重新定位采集指针
		   ClrConsumDatasBak();//擦除存储记录备份
		   break;//只有一条
		}
		else if (Buffer_39SF020[0]==0xff)//空记录时直接跳出
			break;
	}

}
void	FindPosConsumCountSub(void)//查找最后一条记录的流水号
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
		PosConsumCount=0;//新flash流水号从0开始记
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
}uchar		RecordFullDiag(void)//记录存储是否已满诊断	
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
 	//if ((st_data&0x7f)<0x70) //保证有一块是全FF 这样可以找记录时找全F
	//	return	0;
	BlockNum=SaveRecordIndex/RECORDNUM_BLOCK;
	BlockNum=(BlockNum+1)%RECORD_BLOCK_NUM;
	Addr=(ulong)BlockNum*0x1000;
	status=0;
	for (ii=0;ii<RECORDNUM_BLOCK;ii++)
	{
		Force_Flash_Rd_Bytes(Addr,&st_data,1);
		if (st_data==0xa0) //只要有一个 未采 就算是下一块有记录
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
	if (status)	//下一块都不是未采
	{
		Addr=(ulong)BlockNum*0x1000;
		Erase_One_Sector(Addr);
		ReBlockNum=ReCollectRecordIndex/RECORDNUM_BLOCK;
		if (BlockNum==ReBlockNum)//如果复采指针正好包含在被擦除的块内，重新定位复采指针,
		{
			InitSaveRecordSub(0);
			SerialSendReCollectNum=0;
		}
	}
	return	0;
}

void InitSaveRecordSub(uchar bbit) //初始化指针
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
	

	RecordSum=0;//记录总数
	NoCollectRecordSum=0;//没有采集的记录总数
	NoCollectRecordIndex=0;//没有采集的记录指针
	SaveRecordIndex=0;//存储记录指针
	if (!bbit)
		ReCollectRecordIndex=0;//已经采集的记录指针
	
	for(i=0;i<RECORD_BLOCK_NUM;i++)
	{
		status=RecordBlockDiag(i);
		if(status == 0) //找到全FF的块
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
			if(j == RECORDNUM_BLOCK) //确定这块是全FF
			{
				endBlock = i + 1;
				endBlock = endBlock % RECORD_BLOCK_NUM;
				break;
			}
		}
	}

	if(i == RECORD_BLOCK_NUM) //没有找到全FF的块
	{
		endBlock = 0;
	}

	NowPos = endBlock;
	NowPos *= RECORDNUM_BLOCK;


	//开始找未采记录指针、复采记录指针、存储记录指针
	for(i=0;i<MAXRECORD_NUM;i++)
	{
		Addr = NowPos * RECORD_SIZE;
		Force_Flash_Rd_Bytes(Addr, tmpdata, 1);
		if(tmpdata[0] != 0xFF)
		{
			if(!bbit) //需要初始化复采
			{
				if(isFindNoReCollect == 0)
				{
					isFindNoReCollect = 1;
					ReCollectRecordIndex = NowPos;
				}
			}

			if(tmpdata[0] == 0xa0) //找到了记录
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
	
	//开始找存储指针
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


void	InitSaveRecordSubbak(uchar bbit )//初始化指针
{
 	uchar  			i;
//	uchar 			PPage;
	ulong			Addr,jj;
	uchar			st_data;
	uchar			status;
	uchar			EndBlock;
	uchar			StartBlock;
	uchar 		bitFindReIndex=0; //复采记录指针
	uchar		bitFindNoIndex=0;//未采指针
	uchar		bitFindSymbl=0; 
	RecordSum=0;//记录总数
	NoCollectRecordSum=0;//没有采集的记录总数
	NoCollectRecordIndex=0;//没有采集的记录指针
	if (!bbit)
		ReCollectRecordIndex=0;//已经采集的记录指针
	SaveRecordIndex=0;//存储记录指针
	for (i=0;i<RECORD_BLOCK_NUM;i++)
	{//找出记录的起始块
		status=RecordBlockDiag(i);
		if ( (status&0xf0)&& !(status&0x0f))
		{//该块数据有记录但未满。下一块为起始记录的位置
			bitFindSymbl=1;
			EndBlock=i;
			break;
		}
		else if ((status&0xf0) && (status&0x0f))
		{//如果该块已满，诊断下一块的记录
			status=RecordBlockDiag((i+1)%RECORD_BLOCK_NUM);
			if (!status)
			{//如果下一个记录为空,则下一块为起始记录起始块
				bitFindSymbl=1;
				EndBlock=i;
				break;
			}
		}
	}
	if (!bitFindSymbl)//不存在记录，返回
	{//没有找到有记录的块
		status=RecordBlockDiag(0);	
		if (!status)//如果第一块为空，则没有存储记录
			return;//无记录
		else//如果第一块为满, 
		{
			for (i=0;i<RECORD_BLOCK_NUM;i++)
			{//如果某块含有未采记录，下块为采集过的记录，则该快为起始块
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
				EndBlock=RECORD_BLOCK_NUM-1;//如果全部为未采记录或采集过的记录，第1快为开始
		}		
	}
	//记录存储指针
	st_data=RecordBlockDiag(EndBlock);
	if ((st_data&0xf0) && (st_data&0x0f) )
	{//如果最后一块的记录已满，下块的起始地址为存储指针
		if (EndBlock==RECORD_BLOCK_NUM-1)
			SaveRecordIndex=0;//如果记录出口为最后一块的最后一条记录，则记录存储指针为0
		else
			SaveRecordIndex=(EndBlock+1)*RECORDNUM_BLOCK;//否则为下一块的第一条		
	}
	else
	{//当前块没有满，找出下次存储记录的索引号
		SaveRecordIndex=EndBlock*RECORDNUM_BLOCK;	
		Addr=(ulong)EndBlock*0x1000;
		for (jj=0;jj<RECORDNUM_BLOCK;jj++)
		{
			Flash_Rd_Bytes(Addr,&st_data,1);//块的第一条记录
			Addr+=RECORD_SIZE;
			if (st_data==0xff)
			{//找到存储记录的出口
				SaveRecordIndex+=jj;
				break;
			}
		}	
	}
	NoCollectRecordIndex=SaveRecordIndex;
///////////////////////运行到此，找到记录的出口，即下次存储记录的指针//////////////////////?
	StartBlock=(EndBlock+1)%RECORD_BLOCK_NUM;//下块为记录的入口
	for (i=0;i<RECORD_BLOCK_NUM;i++)
	{//从记录的开始位置检索
		status=RecordBlockDiag(StartBlock);	
		switch (status)
		{
			case	0x10:// 开始未采,后为空
				if (!bitFindReIndex)
				{//如果未找到复采入口，置该位置为复采入口
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				if (!bitFindNoIndex)
				{//如果未找到采集入口，置该位置为采集入口
					bitFindNoIndex=1;
					NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				Addr=(ulong)StartBlock*0x1000;
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//块的第一条记录
					Addr+=RECORD_SIZE;
					if (st_data!=0xff)
					{
						RecordSum++;//记录总数
						NoCollectRecordSum++;//未采记录总数	
					}
					else
						break;
				}
				break;
			case	0x11://开始和最后都是未采集
				if (!bitFindReIndex)
				{//如果未找到复采入口，置该位置为复采入口
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				if (!bitFindNoIndex)
				{//如果未找到采集入口，置该位置为采集入口
					bitFindNoIndex=1;
					NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				RecordSum+=RECORDNUM_BLOCK;
				NoCollectRecordSum+=RECORDNUM_BLOCK;
				break;
			case	0x20: //开始已采集，最后无记录 
				if (!bitFindReIndex)
				{//如果未找到复采入口，置该位置为复采入口
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}
				Addr=(ulong)StartBlock*0x1000;			
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//块的第一条记录
					Addr+=RECORD_SIZE;
					if (st_data!=0xff)
					{
						RecordSum++;//记录总数
						if (st_data==0xa0)
						{
							NoCollectRecordSum++;
							if (!bitFindNoIndex)
							{//如果未找到采集入口，置该位置为采集入口
								bitFindNoIndex=1;
								NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK+jj;	
							}	
						}
				   	}
					else
						break;
				}
				break;
			case	0x21://开始已采集，最后未采集
				if (!bitFindReIndex)
				{//如果未找到复采入口，置该位置为复采入口
					bitFindReIndex=1;
					if (!bbit)
						ReCollectRecordIndex=StartBlock*RECORDNUM_BLOCK;	
				}			
				RecordSum+=RECORDNUM_BLOCK;
			//	Addr=(StartBlock&15)*0x1000;
				Addr=(ulong)StartBlock*0x1000;
				for (jj=0;jj<RECORDNUM_BLOCK;jj++)	
				{
					Flash_Rd_Bytes(Addr,&st_data,1);//块的第一条记录
					Addr+=RECORD_SIZE;
					if (st_data==0xa0)
					{
						NoCollectRecordSum++;
						if (!bitFindNoIndex)
						{//如果未找到采集入口，置该位置为采集入口
							bitFindNoIndex=1;
							NoCollectRecordIndex=StartBlock*RECORDNUM_BLOCK+jj;	
						}
					}					
				}
				break;
			case	0x22://第一条和最后一条都采集过
				if (!bitFindReIndex)
				{//如果未找到复采入口，置该位置为复采入口
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
//以上程序找到了开门记录的记录总数，复采记录初试指针，和最后一条消费记录
}	
uchar	RecordBlockDiag(uchar BlockNum)
//该块记录存储情况 对于39SF020--为0-63块
//00 -无记录 ;11-满全部未采; 2-复采记录
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
	aa=BCDToHex(Timeptr[0]);//年
	aa<<=2;
	bb=BCDToHex(Timeptr[1]);//月
	bb&=0x0f;
	ptr[0]=aa+(bb>>2);
	aa=BCDToHex(Timeptr[2]);//日
	ptr[1]=bb<<6;
	aa<<=1;
	ptr[1]+=aa;
	aa=BCDToHex(Timeptr[3]);//时
	if (aa>=16)
		ptr[1]++;
	aa&=0x0f;
	ptr[2]=aa<<4;
	aa=BCDToHex(Timeptr[4]);//分
	ptr[2]+=(aa>>2);
	bb=BCDToHex(Timeptr[5]);//秒
	ptr[3]=aa<<6;
	ptr[3]+=bb;		
}
void	ChgRecordDatasToTime(uchar * ptr ,uchar * Timeptr )
{
	uchar	aa,bb;
	aa=ptr[0];
	Timeptr[0]=HexToBCD(aa>>2);//年
	aa&=3;
	aa<<=2;
	bb=ptr[1];
	aa+=(bb>>6);
	Timeptr[1]=HexToBCD(aa);//月
	aa=bb>>1;
	aa&=0x1f;
	Timeptr[2]=HexToBCD(aa);//日
	bb&=1;
	bb<<=4;
	aa=ptr[2];
	aa>>=4;
	Timeptr[3]=HexToBCD(bb+aa);//时
	aa=ptr[2];
	aa&=0x0f;
	aa<<=2;
	bb=ptr[3];
	aa+=(bb>>6);
	Timeptr[4]=HexToBCD(aa);//分
	Timeptr[5]=HexToBCD(bb&0x3f);
}



