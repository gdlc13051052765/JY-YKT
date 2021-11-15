#include "MyDefine.h"
#include "ExternVariableDef.h"
#include "ExternVoidDef.h"
#include "char.h"

uchar	RequestCard(void);
uchar	Detect_Card(void);
uchar   CheckCardPrinterNum(void);//�ο����²�����У��
uchar   SearchPurseBalnanceDatas(uchar,uchar, uchar *,uchar *);//����Ǯ��
uchar	CurrentConsumMoneyDiag(uchar Sort );//����������Ѷ����
uchar   ReadCardCommonDatas(void);
uchar	ReadCardBalanceDatas(uchar PurseNum);//�����
uchar	ReadCard_DaySumConsumMoney(void);//�����ۼ������Ѷ�
uchar   ConsumPocess(void);
uchar 	WriteCardLjdata(void);

//�����Ƿ��п�
uchar	RequestCard(void)
{
	return check_RFCPU_ResetInfo(CPU_RcLen,CPU_CommBuf);//����λ��Ϣ
}

//��⿨Ƭ�Ƿ�����Ȼ����
uchar	Detect_Card(void)
{
	uchar	status;	
/*	if(Bak_Sort)//cpu��
	{
		status=CPU_Get_Challenge(CPU_RcLen,CPU_CommBuf);
	} */
	//else//M1��
	{
		status=check_RFCPU_ResetInfo(CPU_RcLen,CPU_CommBuf);//����λ��Ϣ
		if(status)
			status=CARD_NOCARD;
		else
		{
			if(TypeA_Sort)
				status=CARD_SAME_ERROR;
				
			status=memcmpself(CardSerialNumBak,CardSerialNum,4);//Ψһ���к�
			if(status)
				status=CARD_SAME_ERROR;	
		}
	}	
	return	status;	
}

//�ο����²�����У��
uchar CheckCardPrinterNum(void)
{
//	uint16_t 	sw1sw2=0;
	uchar	status;
//    uchar   FileAID[2];
     
    //Bak_Sort--֮ǰ���ݵĿ�����
    status=RequestCard();
    if(status)
    	return  status;
    if(Bak_Sort!=TypeA_Sort)
    	return 	CARD_SAME_ERROR;
	
	status=memcmpself(CardSerialNumBak,CardSerialNum,4);//Ψһ���к�
	if(status)
		return	CARD_SAME_ERROR;
	
	if(!TypeA_Sort)
		return 0;
		
	//Ĭ�����ļ�
	//ѡ��һ��ͨ�ļ�
/*	FileAID[0]=0x10;
	FileAID[1]=0x01;
	sw1sw2=CPU_Select_File_AID(0,FileAID,CPU_RcLen,CPU_CommBuf);	
	if(sw1sw2!=0x9000)
		return	CARD_NOCARD;

   	sw1sw2=CPU_Verify_PIN(0,0,3,Sys_PinCode);//ϵͳ�������ʶ1��PINУ��
	if(sw1sw2!=0x9000)
		return	CARD_NOCARD;  */

	return	0;
}

//�����Ĺ�����Ϣ
uchar   ReadCardCommonDatas(void)
{
	uchar	status;
	
	Bak_Sort=TypeA_Sort;

/*	if(TypeA_Sort)//cpu��
		status=ReadCardCommonDatas_CPU();
	else//M1��	*/
		status=ReadCardCommonDatas_M1();
	if(status)
		return	status;	
	status=ReadCard_DaySumConsumMoney();	
	return	status;
}
//�����ۼ������Ѷ�
uchar	ReadCard_DaySumConsumMoney(void)
{
	uchar	status;	
	/*if(TypeA_Sort)//cpu��
		status=ReadCard_DaySumConsumMoney_CPU();
	else//M1�� */
		status=ReadCard_DaySumConsumMoney_M1();
	return	status;		
}
//�����
uchar	ReadCardBalanceDatas(uchar PurseNum)
{
	uchar	status;	
	/*if(TypeA_Sort)//cpu��
		status=ReadCardBalanceDatas_CPU(PurseNum); 
	else//M1�� */
		status=ReadCardBalanceDatas_M1(PurseNum);
	return	status;		
}


//-------------------------------------------------------------------
//PurseEnable--Ǯ����ʹ��˳��
//��aa��ʼ�ң��ҵ�д��PurseNum��PurseNumEndΪ����λ��
//PursesSectorΪ��������Ǯ������
//-------------------------------------------------------------------
uchar	SearchPurseBalnanceDatas(uchar Sort,uchar aa, uchar * PurseNum ,uchar * PurseNumEnd)//����Ǯ��
{
	uchar	i,j;
	uchar	status;
	uchar	Buffer[16];
	ulong	addr;
	if(Flag_NotDefault)
		memcpy(PurseEnable,"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09",10);//Ǯ���Ƿ���Ч
	else
	{
		addr=PurseEnable_Addr+(uint)Sort*16;
		Flash_Rd_Bytes(addr,Buffer,16);
	    if(Buffer[0]==0xa0 && Buffer[1]==Sort && !BytesCheckSum(Buffer,16))
		{
			memcpy(PurseEnable,Buffer+2,10);
		}
		else
			return NO_PURSE_ERROR;
	}
	status=NO_PURSE_ERROR;
	// memcpy(PurseEnable+3,"\x03",1);//Ǯ���Ƿ���Ч
	bitPurseEnd=0;
	for (i=aa;i<10;i++)
	{
		//485TCPģʽ�Ĺ���ѡ��
		if(CommModeChar == 1)
		{
//			Udp_Serverce();	//����uip�¼���������뵽�û������ѭ������ 	
		}
		else
		{
		 	UART_ReceiveSub();
			UART_SendSub();			
		}

		if (PurseEnable[i]<10)//Ǯ�������Ϸ�
		{
			j=PurseEnable[i];//Ǯ����
			if (PursesSector[j]!=0xff)//��������Ǯ������
			{					
				 status=ReadCardBalanceDatas(PursesSector[j]);
				 if(!status)
				 {
				    PurseNum[0]=j;		
					if (i==9)//���һ��Ǯ��
						bitPurseEnd=1;
					else
					{
						j=PurseEnable[i+1];//��һ��Ǯ����
						if (j>9 || PursesSector[j]==0xff)
							bitPurseEnd=1;
					}
					break;
			 	}
			}
		}
		Clr_WatchDog();
	}
	if (i<9)
		PurseNumEnd[0]=i+1;
	else
		PurseNumEnd[0]=0;
	return	status; 					
}

//���ѹ��̿��ƣ�����д����д�ۼ�
uchar   ConsumPocess(void)
{
	uchar	status;
	
	/*if(TypeA_Sort)//cpu��
		status=ConsumPocess_Cpu();//��mac2У�����
	else*/
		status=WriteBalanceToCard_M1(0,PursesSector[PurseUsingNum]);	

	return status;
	
}
int memcmpself(unsigned char * data1,unsigned char *data2,int cnt);
//����������Ѷ����
//��������������� ,CardConsumDate-�ϴε�����ʱ��
//����0-��Ч�ļ۸��������Ѵ�����CardConsumCount�����ۣ�CurrentConsumMoney;
uchar	CurrentConsumMoneyDiag(uchar Sort )
{
	uchar	i;
	uchar	Buffer[16];
	ulong	Addr;
	uchar	bitHaveFind=0;
	uchar	status;
	bitConsumZeroEnable=0;
	Addr=SortPrince_Addr;
	for (i=0;i<200;i++)
	{
		Flash_Rd_Bytes(Addr,Buffer,15);
 		if (Buffer[0]!=0xa0)
			return	CARD_LIMIT_NUM;
		if (Buffer[1]==Sort && !BytesCheckSum(Buffer,15))
		{
			status=memcmpself(SysTimeDatas.TimeString+3,Buffer+2,2);
			if (status!=0xff)
			{
				//status=memcmpself(SysTimeDatas.TimeString+3,Buffer+4,2);
				if(SysTimeDatas.TimeString[3]<Buffer[4])
				{
					bitHaveFind=1;//�ҵ��������ݺ�ʱ��εļ۸񷽰�
					break;
				}
				else
				{
					if(SysTimeDatas.TimeString[3]==Buffer[4])
					{
						if(SysTimeDatas.TimeString[4]<=Buffer[5])
						{
							bitHaveFind=1;//�ҵ��������ݺ�ʱ��εļ۸񷽰�
							break;	
						}	
					} 
				} 
				/*if (status!=1)
				{
					bitHaveFind=1;//�ҵ��������ݺ�ʱ��εļ۸񷽰�
					break;
				} */
			}
		}
		Addr+=16;
		Clr_WatchDog();
	}
	if (!bitHaveFind)
		return	CARD_LIMIT_NUM;
	status=1;
	if (memcmpself(ConsumCountDateTime,SysTimeDatas.TimeString+1,2))
		status=0;//�ϴ�����ʱ��͵�ǰʱ�䲻��ͬһ��
	else
	{
		status=memcmpself(ConsumCountDateTime+2,Buffer+2,2);//�ϴ�����ʱ���ڵ�ǰʱ��֮ǰ
		if(ConsumCountDateTime[2]<Buffer[2])
		{
			status=0;	
		}
		else
		{
			if(ConsumCountDateTime[2]==Buffer[2])
			{
				if(ConsumCountDateTime[3]<Buffer[3])
				{
				   status=0;
				}
				else
				{
					status=1;
				}
			}
			else
			{
				status=1;
			}
		} 
	/*	if (status==0xff)
			status=0;
		else
			status=1;  */
	}
	if (!status)
	{//�ϴ����Ѳ��ڱ�ʱ�����
		CurrentConsumMoney=ChgBCDStringToUlong(Buffer+6,3);
		memcpy(ConsumCountDateTime,SysTimeDatas.TimeString+1,4);//���¼ƴ�ʱ��
		CardDayConsumCount=1;
		if (!CurrentConsumMoney)
			bitConsumZeroEnable=1;
		return	0;
	}
	else
	{//�ϴ�����ʱ���ڱ�������ʱ����
		if (CardDayConsumCount<Buffer[9])
		{
			CurrentConsumMoney=ChgBCDStringToUlong(Buffer+6,3);
			memcpy(ConsumCountDateTime,SysTimeDatas.TimeString+1,4);//���¼ƴ�ʱ��
			CardDayConsumCount++;
			if (!CurrentConsumMoney)
				bitConsumZeroEnable=1;
			return	0;
		}
		else
		{
			i=CardDayConsumCount-Buffer[9];
			if (i<Buffer[13])
			{
				CurrentConsumMoney=ChgBCDStringToUlong(Buffer+10,3);
				memcpy(ConsumCountDateTime,SysTimeDatas.TimeString+1,4);//���¼ƴ�ʱ��
				CardDayConsumCount++;
				if (!CurrentConsumMoney)
					bitConsumZeroEnable=1;
				return	0;
			}
			else
				return	CARD_LIMIT_NUM;
		}
	}
}
uchar 	WriteCardLjdata(void)
{
	uchar	status;	
	/*if(Bak_Sort)//cpu��
		status=WriteCardLjdata_Cpu(1);
	else//M1��*/
		status=ReWriteCardSub_M1(1);
	return	status;	
}