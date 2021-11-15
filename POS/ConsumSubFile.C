#include "MyDefine.h"
#include "ExternVariableDef.h"
#include "ExternVoidDef.h"
#include "char.h"

void	ConsumSub(void);//消费主程序
int memcmpself(unsigned char * data1,unsigned char *data2,int cnt);
void	SetConsumMode(ulong);
uchar	InputSumConsumMoney(uchar,ulong);//消费模式
uchar	KeyBoardSetSysParamenter(uchar,ulong);//通过键盘设置系统参数
uchar	CardBalanceDiag(void);//卡余额诊断
uchar	CheckPinSub(ulong);//PIN认证
void	PrintConsumDatas(uchar);
void	Init_Printer(void);
void	ChgMoneyToPrintBuffer(uchar ,ulong ,uchar * );
void	SaveConsumDatasToBak(uchar);

u8 beeponcnt =0;

u8 isOnJdq = 0;
u8 timeJDQcnt = 0;
u8 useTimeParseNum = 0;
uchar	Card_BT_Sub();//补贴函数

void	ConsumSub(void)//消费
{
uchar	status;
	ulong	ReadKeyValue,iii;	
	ReadKeyValue=ScanKeySub(KEY_ANY);//读按键
	
	switch(ConsumCase)
	{
		case	0:
			beeponcnt = 0;
			PinCheckSta=0xff;       //限额消费PIN校验状态标识
			bitBeepError=0;         //单次报警标识
			InputCase=0;            //键盘状态控制字 
			bitHaveReadBalance=0;   //读余额标识
			SumConsumMoney=0;       //总消费
			CurrentConsumMoney=0;   //消费额
			memset(CardSerialNumBak,0,4);//消费参数初试化
			bitHaveLedError=0;      //单次错误显示标识
			PinCount=0;             //PIN计数
			SelectPurseNum=0;       //应用钱包号
			bitHaveSeclectPurse=0;  //钱包已选择标识
			WriteCardSta=0;         //写卡状态
			Flag_BakRecode=0;		//1，备份记录已写
			ConsumCase=1;
			EscComsumeSaveRecordFlag = 0;
			memset(dispBalance,0,4);
			memset(OldBalance,0,4);
			useTimeParseNum = 0;
			
			break;			
		case	1:		
 			bitPF8563Error=CheckPF8563();//读出时钟并监测合法性
			if (bitPF8563Error)
			{//时钟有问题
				ConsumCase=1;
				if ((InputCase&0xf0)!=0x20)
				{
					DISP_ErrorSub(TIME_ERROR);
					bitSetDateEnable=1;
					//if(!BeepDelayTimeCount)
						//BeepOn(1);
				}
				status=KeyBoardSetSysParamenter(1,ReadKeyValue);//设置时间
			}
			else
 			{ 
				if (bitNeedPrinter && SumPrintMoney)
				{//打印
					bitNeedPrinter=0;
					PrintConsumDatas(0);
				}				
				if (!bitHaveReadBalance && !bitHaveLedError)//无卡无错误显示
				{
					status=RequestCard();//卡复位信息
				
					if(!status)//有卡
					{//选择一卡通文件，读公共信息和累计文件
							
						//Card_BT_Sub();
						
					  status=ReadCardCommonDatas_M1();
						if(!status)
						{
							status = Card_BT_Sub();
								status=ReadCard_DaySumConsumMoney();	
						}
						if(!status)
						{
							//Card_Rebate = Read_Rebate();//读出折扣值No_Use
						
							bitCommStatus=0;
							//找消费钱包号，pin校验读出余额
						    status=SearchPurseBalnanceDatas(CardIdentity,0,&PurseUsingNum,&SelectPurseNum);
						}
						else
							bitCommStatus=1;
					}
					
					StatusBak=status;//标记1  
				}
				else//卡信息已读或出错
				{
				    //利用卡的激活特性，虚拟判断卡是否存在
					status=Detect_Card();
    				if (status)
					{
						//有消费输入，拔卡报警
						if ((InputCase&0xf0)==0x70 && bitHaveReadBalance)
						{
							BeepOn(4);
							DispSumConsumMoney(ConsumMode,0xff,0);
						}
						bitHaveReadBalance=0;
						SelectPurseNum=0;
						ConsumCase = 0;
					}
					else //有卡在，但可能有错误
					{
						//向下切换钱包的操作
						status=StatusBak;
						if ( ReadKeyValue==KEY_ENTER && !InputCase && !bitCommStatus)
						{
							if (bitHaveReadBalance||bitHaveLedError) //自动状态下出错可切换
							{
								bitHaveReadBalance=0;
								bitHaveLedError=0;
								useTimeParseNum = SelectPurseNum;
								status=SearchPurseBalnanceDatas(CardIdentity,SelectPurseNum,&PurseUsingNum,&SelectPurseNum);
								if (bitPurseEnd)
									SelectPurseNum=0;
			 			    }			
						}
					}					
				}			
				if (status!=CARD_NOCARD)
				{
					PinCheckSta=0xff;
					memcpy(CardSerialNumBak,CardSerialNum,4);
					SumPrintMoney=0;//打印额清零
					if (!status)
					{
						if (!bitHaveReadBalance) //显示
						{
							InputCase=0;
							bitHaveReadBalance=1;
							bitHaveSeclectPurse=1;
							if(ConsumMode==CONSUM_RATION)//自动
							{
								ConsumCase=6;//自动消费流程
								bitHaveLedError=0;
								return;
							}
							Disp_Balance(OldBalance);
							if(ShuRuMoney_Err)
							{
								PlayNum(22);
								ShuRuMoney_Err = 0;
							}
							BeepOn(1);
						}
						bitHaveLedError=0;
					}
					else
					{
						bitHaveReadBalance=0;
						if (!bitHaveLedError) //报错
						{
							StatusBak=status;//此处报出"标记1"错误
							bitHaveLedError=1;//错误标志
							DISP_ErrorSub(status);
							PlayNum(status);//语音报错
							BeepOn(4);
					 	}
					}							
				}
				else
				{//无卡
					SelectPurseNum=0;
					if (ReadKeyValue==KEY_ADD && SumPrintMoney && !InputCase)
						PrintConsumDatas(1);
					if (bitHaveReadBalance)
					{
						if ((InputCase&0xf0)==0x70)
							BeepOn(4);
						InputCase=0;
						
					}
					if (bitHaveLedError)
					{
						if((InputCase&0xf0)==0x70)
							InputCase=0;
						else
							InputCase&=0xf0;
					}
					bitHaveLedError=0;
					bitHaveReadBalance=0;
					bitHaveSeclectPurse=0;
				}
				status=KeyBoardSetSysParamenter(bitHaveReadBalance|bitHaveLedError,ReadKeyValue);//输入消费金额
				if (!status && CurrentConsumMoney && (ConsumMode!=CONSUM_RATION))//有消费额输入
				{
					SumPrintMoney=0;
					MenuSort=InputMenuSort;
					InputCase=0;
					DispSumConsumMoney(ConsumMode,InputMenuSort,CurrentConsumMoney);//显示消费金额
					ConsumCase=2;//进入消费流程
				} 
			}
			break;	
		case	2://已经输入金额,开始寻卡消费
			if (!bitHaveSeclectPurse)//没有选择钱包
			{
				beeponcnt++;
				if(beeponcnt >= 10)
				{
					beeponcnt=0;
					BeepOn(1);	
				}
				
				
				status=RequestCard();//卡复位信息
				if(!status)//有卡
				{//选择一卡通文件，读公共信息和累计文件
				
				    status=ReadCardCommonDatas();
					if(!status)//找消费钱包号，pin校验读出余额						
					    status=SearchPurseBalnanceDatas(CardIdentity,useTimeParseNum,&PurseUsingNum,&SelectPurseNum);
				}

				if (!status)
				{
					PinCheckSta=0xff;
					memcpy(CardSerialNumBak,CardSerialNum,4);//备份序列号
				}
			}
			else//已经选择钱包，读出相应的余额
			{	
				status=Detect_Card();//侦测是否有卡
			}
			if (status)//无卡或有错
			{
				if (status==CARD_NOCARD)
				{
					if (bitHaveLedError)//清除错误
						DispSumConsumMoney(ConsumMode,0xff,CurrentConsumMoney);//显示消费金额
					bitHaveLedError=0;
					bitHaveSeclectPurse=0;//清钱包读标记
					Led_Wait_Disp();//zjx_change_20110905
			  }
				else
				{
					if (!bitHaveLedError) //报错
					{
						bitHaveLedError=1;
						DISP_ErrorSub(status);
						PlayNum(status);//语音报错
						BeepOn(4);	
						ConsumCase=8;					
					}
				}
				if (ReadKeyValue==KEY_ESC) //取消
				{
					InputCase=0;
					ConsumCase=1;
				}
			}
			else//读正确
			{
				bitHaveSeclectPurse=1;
				bitHaveReadBalance=1;
				//折扣处理
				CurrentConsumMoney*=Card_Rebate;
			 	CurrentConsumMoney=(CurrentConsumMoney+50)/100;//四舍五入处理
				Flag_BakRecode=0;
				ConsumCase=3;
			}
			break;
		case	3://消费额诊断
			status=CardBalanceDiag();
		
			if(status == CARD_LITTLEMONEY && ConsumMode==CONSUM_RATION)
			{
				status=SearchPurseBalnanceDatas(CardIdentity,SelectPurseNum,&PurseUsingNum,&SelectPurseNum);
				if(!status)
				{
					status=CardBalanceDiag();
				}
			}
		
			if(status)//卡的余额诊断
			{
				if (status==CARD_CONSUM_OVER && bitPinEnable)
				{
					if (PinCheckSta==1)//连续消费过程中前次已经输入过了，本次可以跳过
					{
						memcpy(CardConsumTime,SysTimeDatas.TimeString,6);//保存消费时间
						WriteCardSta=0;//写卡状态
						ConsumCase=4;
						return;
					}
					status=Detect_Card();
					if (!status)//卡存在，锁定PinCheckSta=3
					{
						status=CheckPinSub(ReadKeyValue);//PIN认证
						if (!status)
						{
							memcpy(CardConsumTime,SysTimeDatas.TimeString,6);//保存消费时间
							WriteCardSta=0;//写卡状态
							ConsumCase=4;
						}
					}
					else//无卡
					{
						DelayReadCardTimeCount=200;
						ConsumCase=0;//重新开始
						DispSumConsumMoney(ConsumMode,0xff,0);
					}
				}
				else//其他错误
				{
					if (!bitHaveLedError)
					{
						bitHaveLedError=1;
						DISP_ErrorSub(status);
						PlayNum(status);//语音报错
						BeepOn(4);
					}
					ConsumCase=8;//等待取卡
				}
			}
			else//通过诊断
 		    {
				memcpy(CardConsumTime,SysTimeDatas.TimeString,6);//保存消费时间
				WriteCardSta=0;//写卡状态
				ConsumCase=4;
 		    }
			break;
		case	4://开始扣款操作
		 	status=ConsumPocess();//消费
			if (!status)
			{
				bitHaveLedError=0;
				SumConsumMoney=CurrentConsumMoney;
				if(bitConsumZeroEnable)//限次消费					
					Disp_Hello();//待测
				else
					Disp_Balance(NewBalance);//显示
				
				memcpy(dispBalance,NewBalance,4);
			    BeepOn(1);//蜂鸣
					PlayNum(Consume_Ok );
				
				if(isOnJdq)
				{
					timeJDQcnt = 4;
					GPIO_SetBits(GPIOA,GPIO_Pin_0);
				}
				
			//	SaveConsumDatasToBak(0);//保存记录写入缓存	
				if(ConsumMode==CONSUM_RATION)//自动
				{
					SaveRecordSub(0);//保存记录
				}
				EscComsumeSaveRecordFlag = 1;//是否保存记录标志位
				//ad by lc			   
				if(ConsumMode==CONSUM_RATION)
				 	ConsumCase=8;//拔卡
				else
				{
					bitHaveSeclectPurse=0; //置钱包未读
				 	ConsumCase=7;//继续消费
				}					
			}
			else if (status==CARD_NOCARD) //拔出
			{
				WriteCardSta=1;
				bitHaveLedError=0;
				PlayNum(Consume_Err );
				ConsumCase=5;//后续工作处理
			}
			else if(status==CONSUM_PROCE_FAIL)//交易失败
			{
				WriteCardSta=2;
				bitHaveLedError=0;
				PlayNum(Consume_Err );
				ConsumCase=5;//后续工作处理
			}
			else if(status==CPU_WRITEPURSE_FAIL)//累计失败
			{
				WriteCardSta=3;
				bitHaveLedError=0;
				PlayNum(Consume_Err );
				ConsumCase=5;//后续工作处理
			}
			else if(status==PSAM_CHECK_MAC2)//MAC2
			{
				BeepOn(4);
				WriteCardSta=4;	
				PlayNum(Consume_Err );				
				ConsumCase=5;
			}
			else //非拔卡类错、mac1错===>循坏写入
			{
				if (!bitHaveLedError)
				{
					bitHaveLedError=1;
					DISP_ErrorSub(status);
					BeepOn(4);
				}
				if (ReadKeyValue==KEY_ESC) 
				{
					 //交易没有完成，不用写记录
					 BeepOn(4);
					 ConsumCase=8;//等待拔卡
				} 
			}
			break; 
 		case	5://写过程的拔卡处理
            status=CheckCardPrinterNum();
            if(!status)
            {
                if(WriteCardSta==3 )//CPU当前目录"一卡通"
									status=WriteCardLjdata();//写累计数据				
                else   
								{
									status=ReWriteCardSub_M1(0);
								}
            }//同一卡

						if (!status)
						{
								bitHaveLedError=0;
								SumConsumMoney=CurrentConsumMoney;
                if(bitConsumZeroEnable)//限次消费					
                    Disp_Hello();//待测
                else
                    Disp_Balance(NewBalance);//显示
                BeepOn(1);//蜂鸣	
								if(isOnJdq)
								{
									timeJDQcnt = 4;
									GPIO_SetBits(GPIOA,GPIO_Pin_0);
								}
								PlayNum(Consume_Ok );
                //SaveRecordS5ub(0);//保存记录
								//ad by lc
                if(ConsumMode==CONSUM_RATION)
								{
                  ConsumCase=8;//拔卡
									SaveRecordSub(0);//保存记录
								}
                else
                {
                    bitHaveSeclectPurse=0; //置钱包未读
                    ConsumCase=7;//继续消费
                }				
						}
					else//无卡或换卡或其他错误
					{				
						if(status==GET_PROVE_FAIL)//"认证码失败"
						{
							WriteCardSta=0;
							ConsumCase=4;//重新
							bitHaveLedError=0;
							return;
						}
						else if(status==CONSUM_PROCE_FAIL)//交易失败
							WriteCardSta=2;
						else if(status==CPU_WRITEPURSE_FAIL)//累计失败
							WriteCardSta=3;
						else if(status==PSAM_CHECK_MAC2)//MAC2
							WriteCardSta=4;
						else
						{			
								if (!bitHaveLedError)
							{
								bitHaveLedError=1;
								if(status==CARD_NOCARD)//无卡
								{
									BeepCotrl=0;
									DISP_WriteError(CardPrinterNum);//提示放卡
									if(!Flag_BakRecode)//防止同一消费中多次拔卡，备份多次写入
									{
										SaveConsumDatasToBak(1);
										Flag_BakRecode=1;
									}		
								}
								else
								{
									DISP_ErrorSub(status);
									PlayNum(status);
									BeepOn(4);
								}
							}

							if(status==CARD_NOCARD)//无卡
							{			
								if(!(BeepCotrl%20))
									BeepOn(1);//循环报警
								BeepCotrl++;
								if(BeepCotrl>=200) 
									BeepCotrl=0;
							
								if (ReadKeyValue==KEY_ESC)//非换卡 
								{
									 BeepOn(4);
									 if(WriteCardSta) //不确定状态和累计错
										SaveRecordSub(1);//问题记录处理,错误记录已擦除
									 ConsumCase=8;//等待拔卡
								}
							}
						}
					}
			break;
		case    6://自动消费取金额部分，此部分独立为以后升级留有余地
			status=InputSumConsumMoney(CONSUM_RATION,0);//消费模式
			if(status)
			{
				DISP_ErrorSub(status);
				BeepOn(4);
				ConsumCase=8;//等待拔卡
			}
			else
			{
				Flag_BakRecode=0;
				if(bitConsumZeroEnable)//限次消费
				{
					memcpy(CardConsumTime,SysTimeDatas.TimeString,6);//保存消费时间
					WriteCardSta=0;//写卡状态
					ConsumCase=4;//进入消费
				}
				else
				 	ConsumCase=3;//卡诊断
			}
			break;
		case	7:
			status=Detect_Card();
//			if(!bitHaveSeclectPurse && !status)
//			{   //此处为隐藏时间处理
//				status=ReadCard_DaySumConsumMoney();//读累计额
//				if(!status)
//					status=ReadCardBalanceDatas(PursesSector[PurseUsingNum]);//直接读余额
//				if(!status)
//					bitHaveSeclectPurse=1;
//			}
			  			
			if (!status)
			{
				if ( !InputCase && ReadKeyValue==KEY_ESC && CurrentConsumMoney )
				{
					ConsumCase=9;//取消本次消费
					DISP_ErrorSub(SYS_CONSUM_ESC);
				}
			  else
			  {
//			  	if(EscComsumeSaveRecordFlag)
//				{
//					SaveRecordSub(0);//保存记录
//					EscComsumeSaveRecordFlag = 0;
//				}
				status=0xff;
				if (!InputCase)//输入入口
				{
					if (ConsumMode==CONSUM_SIMPLE)
					{
						if (ReadKeyValue&(KEY_0_9|KEY_POINT|KEY_ADD))
						{
							InputCase=0x70;
							CurrentConsumMoney=0;
							//SaveRecordSub(0);//保存记录
					 	}
					}
					else if (ConsumMode==CONSUM_NUM)
					{
						if ((ReadKeyValue& KEY_0_9) && (ReadKeyValue!=KEY_0))
						{
							InputCount=0;
							InputMaxNum=0;
							InputCase=0x70;
							CurrentConsumMoney=0;
						//	SaveRecordSub(0);//保存记录
					 	}
					}
					else 
					{
						if (ReadKeyValue& (KEY_0_9|KEY_POINT))
						{
							InputCount=0;
							InputMaxNum=0;
							InputCase=0x70;
							CurrentConsumMoney=0;
						//	SaveRecordSub(0);//保存记录
					 	}								
					}
				}
			    if ((InputCase &0xf0)==0x70)//输入过程,包容首次输入的部分
				{
					if (ReadKeyValue==KEY_ESC)
					{
						InputCase=0x70;
                        InputNum=0;
						InputCount=0;
						CurrentConsumMoney=0;
						Disp_Balance(NewBalance);
					}
					else
						status=InputSumConsumMoney(ConsumMode,ReadKeyValue);
			  	}
				if (!status && CurrentConsumMoney)//输入结束
				{
					SaveRecordSub(0);//保存记录
					MenuSort=InputMenuSort;
					SumConsumMoney=0;
					InputCase=0;
					DispSumConsumMoney(ConsumMode,0xff,CurrentConsumMoney);//显示消费金额
					ConsumCase=2;//进入消费流程
				}
			  }
			}
			else//无卡或其他错误
			{
				SaveRecordSub(0);//保存记录
				if(status==CARD_NOCARD)
				{
					if ((InputCase &0xf0)==0x70)
						BeepOn(4);
				}
				else //其他错误报警
				{
					DISP_ErrorSub(status);//直接跳出，不必判断bitHaveLedError
					PlayNum(status);
					BeepOn(4);
				} 
				ConsumCase=8;
			}			
			break; 
 		case	8://等待取卡
			status=RequestCard();
			if (status)
			{	
				DelayReadCardTimeCount=200;
				ConsumCase=0;
				DispSumConsumMoney(ConsumMode,0xff,0);
			}	
			break; 
		case    9:
		   status=RequestCard();
			if (!status)
			{
				status=memcmpself(CardSerialNum,CardSerialNumBak,4);
				if (!status)
				{
					if (ReadKeyValue==KEY_ENTER)
					{			
					//	bitWrite_Add=1;
						status=WriteBalanceToCard_M1(1,PursesSector[PurseUsingNum]);
						if (!status)
						{
							SumConsumMoney=0;

							iii=ChgBCDStringToUlong(NewBalance,4);
							iii+=CurrentConsumMoney;
							ChgUlongToBCDString(iii,NewBalance,4);
							CurrentConsumMoney=0;
							Disp_Balance(NewBalance);
							memcpy(OldBalance,NewBalance,4);
							memcpy(dispBalance,NewBalance,4);
							PlayNum(Real_Ok);
							BeepOn(1);
					 		ConsumCase=7;
					   	}
						if (status)
						{
							SaveConsumDatasToBak(1);
							bitBeepError=0;
							ConsumCase=0;
					  	}
		   			}
		   		}
		 	}
			else
			{//消费后无卡保存记录退出
				BeepOn(4);
				SaveRecordSub(0);
				ConsumCase=0;
				DelayReadCardTimeCount = 500;
				DispSumConsumMoney(ConsumMode,0xff,0);
			}
			break;
			
		default:
			break;
	}

}

void Disp_Blance(uchar * bolache);

uchar	InputSumConsumMoney(uchar Mode,ulong  InKeyValue)//消费模式
{
	uchar	Buffer[5];
	uchar	st_data;
	uchar	aaa[8];
	if (Mode==CONSUM_SIMPLE)//简易消费模式
	{
		if (!(InKeyValue & (KEY_0_9 | KEY_POINT | KEY_ADD) ))
			return	0x10;//无键
		else
		{
			CurrentConsumMoney=0;
			if (InKeyValue==KEY_POINT)
				InputMenuSort=11;
			else if (InKeyValue==KEY_ADD)
				InputMenuSort=12;
			else 
			{
				InputMenuSort=ChgKeyValueToChar(InKeyValue);
				if (!InputMenuSort)
					InputMenuSort=10;
		   	}
			RdBytesFromAT24C64(SimplePrince_Addr+(InputMenuSort-1)*4,Buffer,4);
			if ( BytesCheckSum(Buffer,4) || BCD_String_Diag(Buffer,3))
				memset(Buffer,0,4);
			CurrentConsumMoney=ChgBCDStringToUlong(Buffer,3);
			if (!CurrentConsumMoney)
				BeepOn(3);
			DispSumConsumMoney(ConsumMode,0xff,CurrentConsumMoney);
			return	0;
		}
	}
	else if (Mode==CONSUM_RATION)//自动消费模式
	{
		InputMenuSort=0;	 
		RdBytesFromAT24C64(MenuPrince_Addr,Buffer,4);
		if (BytesCheckSum(Buffer,4) || BCD_String_Diag(Buffer,3))
			memset(Buffer,0,3);
		CurrentConsumMoney=ChgBCDStringToUlong(Buffer,3);
		if (!CurrentConsumMoney)
			st_data=CurrentConsumMoneyDiag(CardIdentity);//卡种类
		else
		{
			CurrentConsumMoney*=Card_Rebate;
			CurrentConsumMoney=(CurrentConsumMoney+50)/100;//四舍五入处理
			st_data=0;
		}
		return	st_data; 
	}
	else if (Mode==CONSUM_NUM)//菜号方式
	{
		if (InputCase==0x70 || InputCase==0x71)
		{
			if (InKeyValue&KEY_0_9)
			{
				st_data=ChgKeyValueToChar(InKeyValue);
				if (InputCase==0x70)
					InputNum=st_data;
				else
					InputNum=(InputNum%10)*10+st_data;
				if (InputNum>240)
					InputNum=0;
				InputCase=0x71;
				InputMenuSort=InputNum;//菜号
				RdBytesFromAT24C64(MenuPrince_Addr+InputNum*4,Buffer,4);
				if (BytesCheckSum(Buffer,4) || BCD_String_Diag(Buffer,3))
					memset(Buffer,0,4);
				SingleMoney=ChgBCDStringToUlong(Buffer,3);
				DispSumConsumMoney(ConsumMode,InputNum,SingleMoney);
				if (!SingleMoney)
					BeepOn(3);
				else
					bitInPutPoint=1;
			}
			else if (InKeyValue==KEY_ENTER)
			{
				bitAddStatus=0;
				CurrentConsumMoney+=SingleMoney;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x73;
			   	}
				else
				{
					DispSumConsumMoney(ConsumMode,InputMenuSort,CurrentConsumMoney);
					if (!CurrentConsumMoney)
						BeepOn(3);
					InputCase=0x70;
			
					return	0;
				}
			}
			else if (InKeyValue==KEY_ADD &&SingleMoney)
			{
				bitAddStatus=1;
				CurrentConsumMoney+=SingleMoney;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x73;
			   	}
				else
				{
					if (bitInPutPoint)
						InputMaxNum++;
					bitInPutPoint=0;
					InputCase=0x70;
					DispSumConsumMoney(ConsumMode,InputMaxNum,CurrentConsumMoney);
					SingleMoney=0;
					InputNum=0;
				}
			}
			else if (InKeyValue==KEY_MUL)
			{
				InputCount=0;
				InputNum=0;
				InputCase=0x72;
				memset(InputBuffer,0xff,6);
				MulData=1;
			}
		}
		else if (InputCase==0x72)
		{
			if (InKeyValue & KEY_0_9)
			{
				st_data=ChgKeyValueToChar(InKeyValue);
				if (!InputCount && st_data)
				{
					InputBuffer[0]=st_data;
				}
				else
					InputBuffer[InputCount]=st_data;
				if (InputCount==3)
				{
					InputBuffer[0]=InputBuffer[1];
					InputBuffer[1]=InputBuffer[2];
					InputBuffer[2]=st_data;
					InputBuffer[3]=0xff;
					InputCount=2;
				}
				InputCount++;

				ChgInputToLedBuffer(5,InputBuffer,5);
				MulData=ChgInputToUlong(InputBuffer,6)/100;
			}
			else if (InKeyValue==KEY_ADD && SingleMoney)
			{
				bitAddStatus=1;
				if (MulData)
				{
					SingleMoney=SingleMoney*MulData;
					CurrentConsumMoney+=SingleMoney;
				}
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x73;
			   	}
				else
				{	
					if (bitInPutPoint)
						InputMaxNum++;
					bitInPutPoint=0;
					DispSumConsumMoney(ConsumMode,InputMaxNum,CurrentConsumMoney);
					SingleMoney=0;
					InputNum=0;
					InputCase=0x70;
				}
			}
			else if (InKeyValue==KEY_ENTER)
			{
				bitAddStatus=0;
				if (MulData)
				{
					SingleMoney=SingleMoney*MulData;
					CurrentConsumMoney+=SingleMoney;
				}
				InputCase=0x70;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x73;
			   	}
				else
				{		
					DispSumConsumMoney(ConsumMode,InputMenuSort,CurrentConsumMoney);
					return	0;
				}								
			}
		}
		else
		{
			DispOver();
			if (InKeyValue==KEY_ESC)
			{
				bitAddStatus=0;
				InputCase=0;
			}
		}
 	}
	else
	{//金额消费
		if (InputCase==0x70)
		{
			if (!InputCount)
			{
				memset(InputBuffer,0xff,6);
				bitInPutPoint=0;
				SingleMoney=0;
			}
			if ( InKeyValue&KEY_0_9) 
			{
				ShuRuMoney_Err =1;
				st_data=0;
				if (InputCount>=3 &&  bitInPutPoint)
				{
					if (InputBuffer[InputCount-3]&0x80)
						st_data=1;
				}
				if (!st_data)
				{
					st_data=ChgKeyValueToChar(InKeyValue);
					InputBuffer[InputCount++]=st_data;
					if ( InputCount==2 && !InputBuffer[0])
					{
						memset(InputBuffer,0xff,6);
						InputBuffer[0]=st_data;
						InputCount=1;
					}
					if (InputCount>=5 && !bitInPutPoint)
					{
						memset(Buffer,0xff,6);
						memcpy(Buffer,InputBuffer+1,4);
						memcpy(InputBuffer,Buffer,6);
						InputCount=4;
						FormatBuffer(InputBuffer,&InputCount);
					}
			 	}
//			    DispBuffer[0]=0x40;
//			    DispBuffer[1]=0x40;
//			    DispBuffer[2]=0x40;
//			    DispBuffer[3]=0x40;
				Disp_Blance(OldBalance);
				
			    ChgInputToLedBuffer(5,InputBuffer,5);
			    if (bitAddStatus|BatModeFlag) //
					DispBuffer[10]|=0x01;
				else		
					DispBuffer[10]=DispModeChar[ConsumMode];					
				SingleMoney=ChgInputToUlong(InputBuffer,6);
				if (InputCount>=6)
					InputCount=5;
			}
			else if (InKeyValue==KEY_POINT && !bitInPutPoint )
			{
				bitInPutPoint=1;
				ShuRuMoney_Err =1;
				if (InputCount)
					InputBuffer[InputCount-1]|=0x80;
				else
				{
					InputBuffer[0]=0x80;
					InputCount=1;
				}					
				ChgInputToLedBuffer(4,InputBuffer,6);

				if (bitAddStatus|BatModeFlag)//
					DispBuffer[10]|=0x01;
				else		
					DispBuffer[10]=DispModeChar[ConsumMode];	
				SingleMoney=ChgInputToUlong(InputBuffer,6);						
			}
			else if (InKeyValue==KEY_ENTER)
			{
				ShuRuMoney_Err = 0;
				InputCount=0;
				bitAddStatus=0;
				CurrentConsumMoney+=SingleMoney;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x72;
				}
				else
				{
//					DispBuffer[0]=0x40;
//					DispBuffer[1]=0x40;
//					DispBuffer[2]=0x40;
					Disp_Blance(OldBalance);
					ChgUlongToLongDispBuffer(CurrentConsumMoney,aaa);

			 //  		DispBuffer[3]=0x40;
					memcpy(DispBuffer+5,aaa+2,5);					
					return	0;
				}
			}
			else if (InKeyValue==KEY_ADD && SingleMoney)
			{
				bitAddStatus=1;
				CurrentConsumMoney+=SingleMoney;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x72;
				}
				else
				{
//				   DispBuffer[0]=0x40;
//				   DispBuffer[1]=0x40;
//				   DispBuffer[2]=0x40;
					ChgUlongToLongDispBuffer(CurrentConsumMoney,aaa);

			   //		DispBuffer[3]=0x40;
					Disp_Blance(OldBalance);
					memcpy(DispBuffer+5,aaa+2,5);					
	
					InputCount=0;	
					InputCase=0x70;
				}
			}
			else if (InKeyValue==KEY_MUL)
			{
				InputCount=0;
				InputCase=0x71;
				memset(InputBuffer,0xff,6);
				MulData=1;
			}
		}
		else if (InputCase==0x71)
		{//输入乘数
			if (InKeyValue & KEY_0_9)
			{
				st_data=ChgKeyValueToChar(InKeyValue);
				if (!InputCount && st_data)
					InputBuffer[0]=st_data;
				else
					InputBuffer[InputCount]=st_data;
				if (InputCount==3)
				{
					InputBuffer[0]=InputBuffer[1];
					InputBuffer[1]=InputBuffer[2];
					InputBuffer[2]=st_data;
					InputBuffer[3]=0xff;
					FormatBuffer(InputBuffer,&InputCount);
				}
				InputCount++;
				ChgInputToLedBuffer(5,InputBuffer,5);

				if (bitAddStatus|BatModeFlag) //
					DispBuffer[10]|=0x01;
				else		
					DispBuffer[10]=DispModeChar[ConsumMode];	
				MulData=ChgInputToUlong(InputBuffer,6)/100;
			}
			else if (InKeyValue==KEY_ADD && SingleMoney )
			{
				bitAddStatus=1;
				if (MulData)
				{
					SingleMoney=SingleMoney*MulData;
					CurrentConsumMoney+=SingleMoney;
				}
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x72;
			   	}
				else
				{
//				   DispBuffer[0]=0x40;
//				   DispBuffer[1]=0x40;
//				   DispBuffer[2]=0x40;
					Disp_Blance(OldBalance);
					ChgUlongToLongDispBuffer(CurrentConsumMoney,aaa);

			   	//DispBuffer[3]=0x40;
					memcpy(DispBuffer+5,aaa+2,5);					
					if (bitAddStatus|BatModeFlag) //
						DispBuffer[10]|=0x01;
					else		
						DispBuffer[10]=DispModeChar[ConsumMode];
					InputCount=0;
					InputCase=0x70;
				}
			}
			else if (InKeyValue==KEY_ENTER)
			{
				bitAddStatus=0;
				if (MulData)
				{
					SingleMoney=SingleMoney*MulData;
					CurrentConsumMoney+=SingleMoney;
				}
				InputCase=0x70;
				if (CurrentConsumMoney>999999)
				{
					BeepOn(3);
					DispOver();
					InputCase=0x72;
		   		}
				else
				{	
				   DispBuffer[0]=0x40;
				   DispBuffer[1]=0x40;
				   DispBuffer[2]=0x40;
					Disp_Blance(OldBalance);
					ChgUlongToLongDispBuffer(CurrentConsumMoney,aaa);

			  // 		DispBuffer[3]=0x40;
					memcpy(DispBuffer+5,aaa+2,6);					
	
					if (bitAddStatus|BatModeFlag)//
						DispBuffer[10]|=0x01;
					else		
						DispBuffer[10]=DispModeChar[ConsumMode];
					return	0;
				}					
			}
		}
		else
		{
			DispOver();
			if (InKeyValue==KEY_ESC)
			{
				bitAddStatus=0;
				InputCase=0;
			}
		}
	}
	return	0xff;
}
uchar	KeyBoardSetSysParamenter(uchar  bbit, ulong	InKeyValue)
{
	uchar	status;
	if (InKeyValue==KEY_ESC)
	{//取消键，返回主菜单
		InputCase=0;
		bitHaveReadBalance=0;
		CurrentConsumMoney=0;
		bitAddStatus=0;
		DispSumConsumMoney(ConsumMode,0xff,CurrentConsumMoney);		
   	}
	if (!bbit)
	{
		if (InKeyValue==KEY_MENU)
			InputCase=0x30;//设置菜号
		else if (InKeyValue & (KEY_SIMPLE|KEY_NUM|KEY_RATION|KEY_MONEY))
		{//设置消费方式
			if (ConsumMode==CONSUM_RATION && InKeyValue==KEY_NUM && (InputCase&0xf0)!=0x50 && (InputCase&0xf0)!=0x30 )
			{
				bitDispFlash=1;
				InputCase=0x52;
				bitLookSysDatas=1;
				bitLookD=0;	
			}
			else
			{
				SetConsumMode(InKeyValue);
				InputCase=0;	
				if (ConsumMode!=CONSUM_RATION)
					CurrentConsumMoney=0;
			}	
		}		
		else if (ConsumMode==CONSUM_MONEY && InKeyValue == KEY_MUL && (InputCase&0xf0)!=0x30 && ((InputCase&0xf0)!=0x70) )
		{//计算份数查看A值
			if (ConsumModeEnable&0x10)
			{
				InputCase=0x50;
				bitDispFlash=1;
				bitLookSysDatas=0;
				bitLookD=0;	
			}
		}
		else if (InKeyValue==KEY_MUL && (InputCase)==0x30)
		{
			bitDispFlash=1;
			bitLookSysDatas=1;
			bitLookD=1;	
			if (ConsumModeEnable&0x20)
				InputCase=0x51;
			else
				InputCase=0x53;						
		}		
		else if (ConsumMode==CONSUM_RATION && InputCase == 0 &&  InKeyValue == KEY_MUL)
			InputCase=0x20;//时间
		else if (InputCase==0x31 &&  InKeyValue==KEY_MUL )
		{//简易单价
			InputCase=0x51;	
			bitDispFlash=1;
			bitLookD = 1;
			bitLookSysDatas = 1;
		}
		if ((InputCase&0xf0)==0x50)
		{
			if (bitLookSysDatas && InKeyValue==KEY_ENTER )
			{
				bitDispFlash=1;//mw200706
				if (!bitLookD)
				{					
					if (InputCase==0x52)
						LedTcpIndex = 0;
					if (InputCase!=0x5d || LedTcpIndex>4 )
						InputCase++;
					if (InputCase==0x5d)
					{
						if(CommModeChar == 1)
							LedTcpIndex++;	
						else
						InputCase=0x5e;
					}
					if (InputCase==0x5e)
						InputCase=0x52;
				}
				else
				{
					if (InputCase==0x51)
						InputCase=0x53;
					if (InputCase==0x53)
						InputCase=0x54;
					else if (InputCase==0x54)
						InputCase=0x55;
					else if (InputCase==0x55)
						InputCase=0x56;
					else if (InputCase==0x56)
						InputCase=0x5e;
					else if (InputCase==0x5e)
					{
						bitDispFlash=1;
						bitLookSysDatas=1;
						bitLookD=1;	
						if (ConsumModeEnable&0x20)
							InputCase=0x51;
						else
							InputCase=0x53;	
					}					
				}
			}
		}	
		if (ConsumMode==CONSUM_NUM && (InputCase&0xf0)!=0x70 &&  InKeyValue == KEY_MUL)
		{//倒查流水
			InputCase=0x60;	
		}
	}
	else
	{
		if (bitPF8563Error)
		{
			if (InKeyValue )
			{
				if ( InKeyValue==KEY_RATION)
					bitHaveAuto=1;
				else if (bitHaveAuto &&  InKeyValue == KEY_MUL)
				{
					InputCase=0x20;//时间
					bitHaveAuto=0;
				}
				else
					bitHaveAuto=0;
			}
		}
	}
	status=0x20;
	if (!InputCase)
	{
		bitAddStatus=0;
		if (!bitHaveLedError && !bitPF8563Error)
		{
			if (ConsumMode==CONSUM_SIMPLE)
			{
				CurrentConsumMoney=0;
				if (( InKeyValue&(KEY_0_9|KEY_POINT|KEY_ADD)))			
					InputCase=0x70;
			}
			else if(ConsumMode!=CONSUM_RATION) //自动不走此流程
			{
				InputCount=0;
				InputMaxNum=0;
				CurrentConsumMoney=0;
				if (ConsumMode==CONSUM_NUM)	
				{
					if ((InKeyValue & KEY_0_9) && (InKeyValue!=KEY_0))
						InputCase=0x70;
				}
				else
				{
					if (InKeyValue &(KEY_0_9|KEY_POINT) )
						InputCase=0x70;					
				}
			}
		}
		if (!bbit)
			DispSumConsumMoney(ConsumMode,0xff,CurrentConsumMoney);
	}
	if (!bitHaveLedError)
	{
		if ((InputCase&0xf0)==0x10)
	 		DispMainCode();//站点号
		if ((InputCase&0xf0)==0x20)
			SetSysDateTime(InKeyValue);//时间
		if ((InputCase&0xf0)==0x30)
			SetSysPrince(0,InKeyValue);	//菜号	
		if ((InputCase&0xf0)==0x40)
			SetSysPrince(1,InKeyValue);//简易单价
		if ((InputCase&0xf0)==0x50)
			LookSysPatameter();//记录数目查询
		if ((InputCase&0xf0)==0x60) 
			LookRecordDatas(InKeyValue);//消费记录查询
		if ((InputCase&0xf0)==0x70)
			status=InputSumConsumMoney(ConsumMode,InKeyValue);
	}
	return	status;
}

void	SaveConsumDatasToBak(uchar bbit)
{
	uchar	Buffer[RECORD_SIZE];
	uchar	status;
	uchar	st_data;
	ulong	ii,Addr;
	if (!RecordDatasToBuffer(bbit,Buffer)) //备份消费记录
	{
		status=1;
		for (ii=0;ii<RECORDNUM_BLOCK;ii++)
		{
			Addr=RecordDatasToBuffer_Addr+ii*RECORD_SIZE;
			Flash_Rd_Bytes(Addr,&st_data,1);
			if (st_data==0xff)
			{
				Flash_Write_Bytes(Addr,Buffer,RECORD_SIZE);
				status=0;
				break;
			}
			else if(st_data==0xa0) //同一消费多次拔卡
			{
				status=0;
				break;	
			}
		}
		if (status)
		{
			Erase_One_Sector(RecordDatasToBuffer_Addr);//擦除存储记录备份
			Flash_Write_Bytes(RecordDatasToBuffer_Addr,Buffer,RECORD_SIZE);
		}
	}	
}
uchar		CardBalanceDiag(void)//卡余额诊断
{
	ulong	iii;
	ulong	jjj;

	if (CurrentConsumMoney>999999)//单次规划限额9999.99
		return	CARD_LITTLEMONEY;

	iii=ChgBCDStringToUlong(OldBalance,4);
	jjj=ChgBCDStringToUlong(CardMinBalance,3);
	jjj+=CurrentConsumMoney;
	if (jjj>iii)                //余额不足
		return	CARD_LITTLEMONEY;


	iii=Max_ConsumMoney;//单笔限额
	if (CurrentConsumMoney>iii)
		return	CARD_CONSUM_OVER;

	jjj=Limit_DayMoney;//日限额
	if (CurrentConsumMoney>jjj)
		return	CARD_CONSUM_OVER;

	iii=CardDayConsumMoney;//日累计
	if(Limit_MoneySign>1)//日限额标志
	{
		if ( !memcmpself(CardConsumDate,SysTimeDatas.TimeString,3) ) 
		{			
			iii+=CurrentConsumMoney;
			if (iii>jjj)
				return	CARD_CONSUM_OVER;//超出消费限额
		}
	}
	else if(Limit_MoneySign==1)//月限额标志
	{
		if ( !memcmpself(CardConsumDate,SysTimeDatas.TimeString,2) ) 
		{
			iii+=CurrentConsumMoney;
			if (iii>jjj)
				return	CARD_CONSUM_OVER;//超出消费限额
		}
	}
	return	0;
}

uchar	CheckPinSub(ulong ReadKeyValue )//PIN认证
{
	uchar	st_data;
//	uint	sw1sw2=0;
	uchar     bbit=0;

	if(PinCheckSta==0xff)
		bbit=1;
	if (ReadKeyValue & KEY_0_9 && PinCount<8)
	{//输入PIN
		PinCheckSta=0xfe;
		bbit=1;
		st_data=ChgKeyValueToChar(ReadKeyValue);
		if (!PinCount)
		{
			InputBuffer[0]=st_data<<4;
			memset(DispBuffer+2,0,8);
			DispBuffer[0]=0x73;
			DispBuffer[1]=0x37;
		}
		else if (PinCount==1)
			InputBuffer[0]+=st_data;
		else if (PinCount==2)
			InputBuffer[1]=st_data<<4;
		else if (PinCount==3)
			InputBuffer[1]+=st_data;
		else if (PinCount==4)
			InputBuffer[2]=st_data<<4;
		else if (PinCount==5)
			InputBuffer[2]+=st_data;
		PinCount++;
	}
	else if (ReadKeyValue==KEY_ENTER)
	{
		bbit=1;
	    if(PinCount==6)
		{
			/*if(Bak_Sort)//cpu卡
			{
				sw1sw2=CPU_Verify_PIN(0,1,3,InputBuffer);//密码与标识0的PIN校验
				if(sw1sw2==0x9000)	
					PinCheckSta=1;
				else
				{				
					if((sw1sw2&0xfff0)==0x63c0)//警告将死锁
					{
						st_data=(uchar)(sw1sw2&0x000f);
						PinCheckSta=0x20+st_data;
						BeepOn(3);
						PinCount=0;
					}
					else if(sw1sw2==0x6983)//死锁
					{
						PinCheckSta=3;
						BeepOn(3);
						PinCount=0;
					}
				}
			} 
			else*/				
			{
				if (!memcmpself(InputBuffer,PinCode,3))
					PinCheckSta=1;
				else
				{
					PinCheckSta=0x20;
					BeepOn(3);
					PinCount=0;
				}		
			}	
		}
		else
		{
			PinCheckSta=4;
			BeepOn(3);
			PinCount=0;
		}
	}
	//显示
	if(bbit)
	{
		if(PinCheckSta==0xff)//初始状态 
		{	
			memset(DispBuffer+2,0,8);
			DispBuffer[0]=0x73;
			DispBuffer[1]=0x37;//pn
		}
		else if(PinCheckSta==0xfe)//输入状态
		{
		   //PN--
			memset(DispBuffer+2,0x40,PinCount);
		}
		else if(PinCheckSta==3)
		{
			//密码错误次数超限!密码已锁定! 
			//PIN OVER
			memcpy(DispBuffer,"\x73\x06\x37\x00\x00\x3f\x3e\x79\x50\x00",10);
		}
		else if(PinCheckSta==4)
		{
			//PIN SHORT
			memcpy(DispBuffer,"\x73\x06\x37\x00\x00\x6d\x74\x5c\x50\x78",10);
		}
		else if((PinCheckSta&0xf0)==0x20)
		{
			//PIN ERR-X
			memcpy(DispBuffer,"\x73\x06\x37\x00\x00\x79\x50\x50\x00\x00",10);
			if(Bak_Sort)
			{
				DispBuffer[8]=0x40;
				DispBuffer[9]=Disp0_9String[PinCheckSta&0x0f];
			}
			
		}
		LED_DispDatas_all(DispBuffer);
		bbit=0;
	}

	if(PinCheckSta==1)	return	0;
	else	return	0xff;
	
}

void	PrintConsumDatas(uchar bbit)
{
	 uchar	PrintTopDatas[]={
		"\x0c\x00\x00"//名称
	  	"站点号:-------------------""\x01\x00"
	  	"卡编号:------------------""\x02\x00"
		"钱包号:----------------------""\x0d\x00"
	   	"消费额:----------------""\x03\x00"
		"新余额:----------------""\x04\x00"
		"消费日期:20""\x05""年""\x06""月""\x07""日 ""\x08"":""\x09"":""\x0a\x0"
		"\x0"
		"     谢谢惠顾欢迎下次光临""\x0b\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff"};

	uchar	Buffer[16];
	uchar	aa;
	uchar * ptr;
	ulong	iii;
	uchar	i;
	uint	ii;
	Init_Printer();
	ptr=PrintTopDatas;
	while (1)
	{
		UART_ReceiveSub();
		UART_SendSub();	
		aa= * ptr++;
		if (aa==0xff)
		{
			for (i=0;i<4;i++)
			{
				Buffer[0]=0x0a;	
				USART_PUTS_Arry(2,Buffer,1);
				delay_ms(1);
				
				Clr_WatchDog;
			}
			break;
		}
		else if (aa==0x0b)
		{
			if (bbit)
			{
				USART_PUTS_Arry(2,"(复打)",6);
				delay_ms(1);
			}			
		}
		else  if (!aa)
		{
			Buffer[0]=0x0d;
			Buffer[1]=0x0a;
			USART_PUTS_Arry(2,Buffer,2);
			delay_ms(1);
		}
		else if (aa>=0x80 && aa<0xff)
		{
			Buffer[0]=aa;
			Buffer[1]= * ptr++;
			USART_PUTS_Arry(2,Buffer,2);		
		}
		else if (aa<0x80 && aa>=0x20 )			
		USART_PUTS_Arry(2,&aa,1);
		else
		{
			memset(Buffer,0x20,16);
			switch(aa)
			{
				case	1://站点
					Buffer[0]=0x30+MainCode/10000;
					ii=MainCode%10000;
					Buffer[1]=0x30+MainCode/1000;
					ii=ii%1000;
					Buffer[2]=0x30+MainCode/100;
					ii=ii%100;
					Buffer[3]=0x30+ii/10;
					ii=ii%10;
					Buffer[4]=0x30+ii;
					USART_PUTS_Arry(2,Buffer,5);				
					break;
				case	2://卡号
					for (i=0;i<3;i++)
					{
						Buffer[i*2]=0x30+CardPrinterNum[i+1]/0x10;
						Buffer[i*2+1]=0x30+CardPrinterNum[i+1]%0x10;
					}
					USART_PUTS_Arry(2,Buffer,6);							
					break;
				case	3://消费额				 
					ChgMoneyToPrintBuffer('-',SumPrintMoney,Buffer);
					USART_PUTS_Arry(2,Buffer,9);	
					break;
				case	4://新余额
                    iii=ChgBCDStringToUlong(NewBalance,4);
					ChgMoneyToPrintBuffer('-',iii,Buffer);							
					USART_PUTS_Arry(2,Buffer,9);	
					break;
				case	5://年
					aa=BCDToHex(CardConsumTime[0]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					USART_PUTS_Arry(2,Buffer,2);	
					break;
				case	6://月
					aa=BCDToHex(CardConsumTime[1]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					USART_PUTS_Arry(2,Buffer,2);	
					break;
				case	7://日
					aa=BCDToHex(CardConsumTime[2]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					USART_PUTS_Arry(2,Buffer,2);	
					break;
				case	8://时
					aa=BCDToHex(CardConsumTime[3]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					USART_PUTS_Arry(2,Buffer,2);	

					break;
				case	9://分
					aa=BCDToHex(CardConsumTime[4]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					USART_PUTS_Arry(2,Buffer,2);	
					break;
				case	0x0a://秒
					aa=BCDToHex(CardConsumTime[5]);
					Buffer[0]=aa/10+0x30;
					Buffer[1]=aa%10+0x30;
					USART_PUTS_Arry(2,Buffer,2);	
					break;
				case	0x0c:
				    //USART_PUTS_Arry(2,"\x1b\x0e",2);
                    USART_PUTS_Arry(2,"  消 费 清 单   ",16);
            //        USART_PUTS_Arry(2,"\x1b\x14",2);
					break;
				case	0x0d:
					Buffer[0]=0x30+PurseUsingNum/10;
					Buffer[1]=0x30+PurseUsingNum%10;
                    USART_PUTS_Arry(2,Buffer,2);
					break;
				default:
					break;	
			}
		}		
		Clr_WatchDog();
	}  
}  

void	Init_Printer(void)
{
	uchar	Buffer[2];
	Buffer[0]=0x1b;
	Buffer[1]=0x40;
	USART_Send_Char(2,Buffer[0]);
	USART_Send_Char(2,Buffer[1]);
	delay_ms(10);
}

void	ChgMoneyToPrintBuffer(uchar aa,ulong lll,uchar * ptr)
{
	uchar	i;
	ulong	iii=10000000;
	uchar	bbit=0;	
	uchar	bb;
	for (i=0;i<8;i++)
	{
		if (i==6)
			* ptr++=0x2e;
		bb=(uchar) (lll/iii);
		if (!bb)
		{
			if (!bbit && i<5)
				*ptr++=aa;
			else
				* ptr++=0x30+bb;	
		}
		else
		{
			bbit=1;
			* ptr ++=0x30+bb;
		}
		lll=lll%iii;	
		iii=iii/10;
	}
}



void socket2senddata(unsigned char *dat_ptr, unsigned short size);
void udp_server_send_data();
void udp_server_send_data_socket1();

uchar UARTFrame = 0;
extern u8 S1_data ;
typedef unsigned char SOCKET;
extern void Process_Socket2_Data(SOCKET s);
uchar	ReceiveDatasDiag(void);//接收数据诊断
uchar	ReadCardBalanceDatas(uchar PurseNum);
u8 PurseSectorDatas[16];
u8 Flag_Overdata;
uchar	OldCardBufferBak[16];//原卡数据备份
uchar	PurseBTNumNew[2];//新的补贴序列号
extern unsigned char W5500_Interrupt;
void W5500_Interrupt_Process(void);

uchar 	ConsumChar;
uchar btdata = 0;
void saveBTRecord(uchar * data);
extern uchar isclearbt;
extern uchar isHaveBTRecord;


uchar	Card_BT_Sub()//补贴函数
{
	uchar	status;
	uchar	i;
	uint	ii;
	ulong	iii,jjj;
	uchar	Buffer1[32];
	uchar	Buffer2[16];
	uchar bbit=1;
	uchar fail=0;
	uchar	have_err=0;
	uchar	Sector;	
    //等待本次发送接收数据结束
	ii=2000;
	
	if(canbutie != 0) return 0 ;
	if(isHaveBTRecord != 0) return 0;
	
	memcpy(CardSerialNumBak,CardSerialNum,4);//备份序列号
	while ((SerialReceiveLen || bitSerial_ReceiveEnd) && ii)
	{
		UART_ReceiveSub();
		udp_server_send_data();
		ii--;
		Clr_WatchDog();
	}
    //查看是否为同一张卡
	status=RequestCard();
	if (status)
		return	CARD_NOCARD;
  if (memcmp(CardSerialNumBak,CardSerialNum,4))
		return	CARD_CHGERROR;

  //发送补贴命令
    Init_Serial();
    bitSerial_ReceiveEnd=1;
    SerialUnion.S_DatasStruct.UartStatus=0;
    SerialUnion.S_DatasStruct.UartAddrH=0;
    SerialUnion.S_DatasStruct.UartAddrL=0;
    SerialUnion.S_DatasStruct.UartFrameNum=UARTFrame++;
    SerialUnion.S_DatasStruct.UartComd=BTREQUEST_COMD;
    SerialUnion.S_DatasStruct.DatasLen=4;
    memcpy(SerialUnion.S_DatasStruct.Datas,CardSerialNum,4);
    memcpy(Receive_Ip_Port,NetCardDatas+16,6);
    bitSerial_SendRequest=1;
    udp_server_send_data_socket1();//发送

  //接收数据
    for (i=0;i<3;i++)
    {
        iii=800000*3;
        Init_Serial();	
        SerialReceiveTimeCount=200;
        do
        {
					if(W5500_Interrupt)//处理W5500中断		
					{
						W5500_Interrupt_Process();//W5500中断处理程序框架
						if(S1_data != 0)
						{
							S1_data = 0;
							Process_Socket2_Data(1);
							status=ReceiveDatasDiag();
							Receive_Data_Double ();//将接收的数据大小端转换过来
						}
					}
						
				if (bitSerial_ReceiveEnd)
						break;	
				Clr_WatchDog();
        }while (--iii);	
        if (!SerialReceiveLen)//未接收到数据
            break;
        //status=TCPCardReceiveDiag();
        if (!status &&  (SerialUnion.S_DatasStruct.UartComd&0xfff)==BTREQUEST_COMD)
        {
            bbit=0;
            break;
        }
    }

    bitSerial_ReceiveEnd=1;
    if (bbit)//接收状态错误
    {
        //Init_Serial();
        return CARD_BT_ERROR;
    }

    if(SerialUnion.S_DatasStruct.Datas[4])//挂失了
    {
        //Init_Serial();
        return CARD_LOSTCARDNUM;
    }

    if(SerialUnion.S_DatasStruct.DatasLen==5)//没有数据
    {
        //Init_Serial();
        return 0;
    }
    
    status=RequestCard();
    if(status || memcmp(CardSerialNumBak,CardSerialNum,4))//已经无卡
    {
        Init_Serial();
        return  0;
    }
    //bt数据处理
		if(memcmp(CardSerialNum,SerialUnion.S_DatasStruct.Datas+1,4))
		{
			return 0;//
		}
    if (SerialUnion.S_DatasStruct.Datas[6])//有补贴
    {
        PurseUsingNum=SerialUnion.S_DatasStruct.Datas[5];
        Sector=PursesSector[PurseUsingNum];
				btdata = 1;
        status=ReadCardBalanceDatas(Sector);//读出钱包的余额
				btdata = 0;
        if(!status)
        {
            bbit=0;//此时的卡余额信息是可用的
            if (!memcmp(SerialUnion.S_DatasStruct.Datas+7,PurseContrlNum,2))
            {
                Init_Serial();//已经补贴过 
                return 0;
							//memcpy();
//							iii=ChgBCDStringToUlong(OldBalance,4);//原余额
//							jjj = iii;
//							jjj=ChgBCDStringToUlong(SerialUnion.S_DatasStruct.Datas+9,3);//补贴金额
//               if (SerialUnion.S_DatasStruct.Datas[6]==1)//累计补贴
//                   iii = iii - jjj;
//							ChgUlongToBCDString(iii,OldBalance,4);//新余额
//							memcpy(PurseBTNumNew,SerialUnion.S_DatasStruct.Datas+7,2);
							//重新上传记录
            }
            else //需要补贴的部分
            {
                memcpy(Buffer1,PurseSectorDatas,16);//第一块信息
                memcpy(Buffer1+12,SerialUnion.S_DatasStruct.Datas+12,3);//钱包有效期
                Buffer1[15]=CalCheckSum(Buffer1,15);
                
                iii=ChgBCDStringToUlong(OldBalance,4);//原余额
                jjj=ChgBCDStringToUlong(SerialUnion.S_DatasStruct.Datas+9,3);//补贴金额
                if (SerialUnion.S_DatasStruct.Datas[6]==1)//累计补贴
                    jjj+=iii;
                else//清零时
                {
                    if(iii>0 && !Flag_Overdata)//有效期内有可消费额
                    {
                        Init_Serial();
                        return 1;
                    }     
                }
                 memcpy(Buffer2,OldCardBufferBak,16);//余额信息
                ChgUlongToBCDString(jjj,Buffer2,4);//新余额
								if(SerialUnion.S_DatasStruct.Datas[6]==1)
									ii=GetU16_HiLo(PurseConsumCount)+1;
								else 
									ii=GetU16_HiLo(PurseConsumCount)+2;
								ii = DoubleBigToSmall(ii);
								memcpy(Buffer2+11,(uchar *)&ii,2);//钱包的消费次数
                memcpy(Buffer2+13,SerialUnion.S_DatasStruct.Datas+7,2);//新的补贴序列号
                memcpy(PurseBTNumNew,SerialUnion.S_DatasStruct.Datas+7,2);
                Buffer2[15]=CalCheckSum(Buffer2,15);//准备新的写卡数据
                do{
									Clr_WatchDog();
                    fail=0;
                    if (PurseUsingNum)//主钱包不写
                        status=MFRC522_Write(Sector*4,Buffer1);                              
                    if (!status)
                    {
                        status=MFRC522_Write(Sector*4+1,Buffer2);
                        if (!status)
                            status=MFRC522_Write(Sector*4+2,Buffer2);   
                    }
										Clr_WatchDog();
                    if(status)
                        fail=1;//写过程失败	
                    if(fail)//补贴过程中发生错误
                    {
                        while(1)
                        {
													Clr_WatchDog();
                            status=RequestCard();
                            if(!status && !memcmp(CardSerialNumBak,CardSerialNum,4))//放回原来的卡
                            {
                                if(have_err)
                                {
                                    DispSumConsumMoney(ConsumMode,0xff,0);
                                    have_err=0;
																		status=CardAuthKeySub(1,Sector*4);
																		if (!status)
																			break;	
                                }
																else
                                	break;	
                            }
                            else
                            {
                                if(!have_err)
                                {
                                    DISP_ErrorSub(CARD_BT_ERROR);
                                    BeepOn(4);
                                    have_err=1;
                                }
                                if (ScanKeySub(KEY_ANY)==KEY_ESC)
                                {
                                    status=0xff;
                                    fail=0;
                                    break;
                                }	
                            }
                            
                        }
                    }
                    Clr_WatchDog();
                }while(fail);
            }
        }
        else//读补贴钱包失败
        {
            Init_Serial();
            return 1;
        }   
    }
    else//无数据
    {
        Init_Serial();
        return 0;
    }
    //写记录
	if (SerialUnion.S_DatasStruct.Datas[6]==1)
	{//有效期内累加
		ConsumChar=2;
		SumConsumMoney=ChgBCDStringToUlong(SerialUnion.S_DatasStruct.Datas+9,3);//补贴额
		ChgUlongToBCDString(jjj,NewBalance,4);//新余额
		if(!status)
		{
			BeepOn(1);	
			if(!SaveRecordIndex)
				ii=MAXRECORD_NUM;
			else
				ii=SaveRecordIndex;
			if(!(RecordDatasToBuffer(0,Buffer1)))
			{
				Init_Serial();
				bitSerial_ReceiveEnd=1;
				SerialUnion.S_DatasStruct.UartStatus=0;
				SerialUnion.S_DatasStruct.UartAddrH=ii/256;
				SerialUnion.S_DatasStruct.UartAddrL=ii%256;
				SerialUnion.S_DatasStruct.UartFrameNum=UARTFrame++;
				SerialUnion.S_DatasStruct.UartComd=BTRECORD_COMD;
				SerialUnion.S_DatasStruct.DatasLen=31;
				memcpy(SerialUnion.S_DatasStruct.Datas,Buffer1+1,31);
				//memcpy(Receive_Ip_Port,NetCardDatas+16,6);
				bitSerial_SendRequest=1;
	      udp_server_send_data_socket1();
				
				bbit = 1;
				//接收数据
				for (i=0;i<3;i++)
				{
						iii=800000*3;
						Init_Serial();	
						SerialReceiveTimeCount=200;
						do
						{
							if(W5500_Interrupt)//处理W5500中断		
							{
								W5500_Interrupt_Process();//W5500中断处理程序框架
								if(S1_data != 0)
								{
									S1_data = 0;
									Process_Socket2_Data(1);
									status=ReceiveDatasDiag();
									Receive_Data_Double ();//将接收的数据大小端转换过来
								}
							}
														
							if (bitSerial_ReceiveEnd)
									break;	
							Clr_WatchDog();
						}while (--iii);	
						if (!SerialReceiveLen)//未接收到数据
								break;
						//status=TCPCardReceiveDiag();
						if (!status &&  (SerialUnion.S_DatasStruct.UartComd&0xfff)==BTRECORD_COMD)
						{
								bbit=0;
								break;
						}
				}
				
				if(bbit)
				{
					saveBTRecord(Buffer1);
				}
				
				bitSerial_ReceiveEnd = 0;
			}
		}
		Clr_WatchDog();
	}
	else
	{//清补贴或过期累加
		ConsumChar=4;
		iii=ChgBCDStringToUlong(SerialUnion.S_DatasStruct.Datas+9,3);//备份补贴额
		SumConsumMoney=ChgBCDStringToUlong(OldBalance,4);//补贴额
		memset(NewBalance,0,4);
		if(!status)
		{
			BeepOn(1);
			if(!SaveRecordIndex)
				ii=MAXRECORD_NUM;
			else
				ii=SaveRecordIndex;
			if(!(RecordDatasToBuffer(0,Buffer1)))
			{
				Init_Serial();
				bitSerial_ReceiveEnd=1;
				SerialUnion.S_DatasStruct.UartStatus=0;
				SerialUnion.S_DatasStruct.UartAddrH=ii/256;
				SerialUnion.S_DatasStruct.UartAddrL=ii%256;
				SerialUnion.S_DatasStruct.UartFrameNum=UARTFrame++;
				SerialUnion.S_DatasStruct.UartComd=BTRECORD_COMD;
				SerialUnion.S_DatasStruct.DatasLen=31;
				memcpy(SerialUnion.S_DatasStruct.Datas,Buffer1+1,31);
				memcpy(Receive_Ip_Port,NetCardDatas+16,6);
				bitSerial_SendRequest=1;
	      udp_server_send_data_socket1();
				
				bbit = 1;
				//接收数据
				for (i=0;i<3;i++)
				{
						iii=800000*3;
						Init_Serial();	
						SerialReceiveTimeCount=200;
						do
						{
							if(W5500_Interrupt)//处理W5500中断		
							{
								W5500_Interrupt_Process();//W5500中断处理程序框架
								if(S1_data != 0)
								{
									S1_data = 0;
									Process_Socket2_Data(1);
									status=ReceiveDatasDiag();
									Receive_Data_Double ();//将接收的数据大小端转换过来
								}
							}
	
							if (bitSerial_ReceiveEnd)
									break;	
							Clr_WatchDog();
						}while (--iii);	
						if (!SerialReceiveLen)//未接收到数据
								break;
						//status=TCPCardReceiveDiag();
						if (!status &&  (SerialUnion.S_DatasStruct.UartComd&0xfff)==BTRECORD_COMD)
						{
								bbit=0;
								break;
						}
				}
				
				if(bbit)
				{
					saveBTRecord(Buffer1);
				}
				
				bitSerial_ReceiveEnd = 0;
				
				SumConsumMoney=0;
			}	
		}

		isclearbt = 1;
		ConsumChar=2;
		SumConsumMoney=jjj;//补贴额;//补贴额
		memset(OldBalance,0,4);
		ChgUlongToBCDString(SumConsumMoney,NewBalance,4);
		if(!status)
		{
			if(!SaveRecordIndex)
				ii=MAXRECORD_NUM;
			else
				ii=SaveRecordIndex;
			if(!(RecordDatasToBuffer(0,Buffer1)))
			{
				Init_Serial();
				bitSerial_ReceiveEnd=1;
				SerialUnion.S_DatasStruct.UartStatus=0;
				SerialUnion.S_DatasStruct.UartAddrH=ii/256;
				SerialUnion.S_DatasStruct.UartAddrL=ii%256;
				SerialUnion.S_DatasStruct.UartFrameNum=UARTFrame++;
				SerialUnion.S_DatasStruct.UartComd=BTRECORD_COMD;
				SerialUnion.S_DatasStruct.DatasLen=31;
				memcpy(SerialUnion.S_DatasStruct.Datas,Buffer1+1,31);
				memcpy(Receive_Ip_Port,NetCardDatas+16,6);
				bitSerial_SendRequest=1;
	      udp_server_send_data_socket1();
				
				
				bbit = 1;
				//接收数据
				for (i=0;i<3;i++)
				{
						iii=800000*3;
						Init_Serial();	
						SerialReceiveTimeCount=200;
						do
						{
							if(W5500_Interrupt)//处理W5500中断		
							{
								W5500_Interrupt_Process();//W5500中断处理程序框架
								if(S1_data != 0)
								{
									S1_data = 0;
									Process_Socket2_Data(1);
									status=ReceiveDatasDiag();
									Receive_Data_Double ();//将接收的数据大小端转换过来
								}
							}
							
								
							
								if (bitSerial_ReceiveEnd)
										break;	
								Clr_WatchDog();
						}while (--iii);	
						if (!SerialReceiveLen)//未接收到数据
								break;
						//status=TCPCardReceiveDiag();
						if (!status &&  (SerialUnion.S_DatasStruct.UartComd&0xfff)==BTRECORD_COMD)
						{
								bbit=0;
								break;
						}
				}
				
				if(bbit)
				{
					saveBTRecord(Buffer1);
				}
				
				bitSerial_ReceiveEnd = 0;
				
			}
		}
		
		isclearbt = 0;
		Clr_WatchDog();
	}
	SumConsumMoney=0;
	ConsumChar=0;
	Init_Serial();	
	return  status;
}





