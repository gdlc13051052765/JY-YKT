#include "MyDefine.h"
#include "ExternVoidDef.h"
#include "ExternVariableDef.h"
#include "char.h"
#include "gpio.h"

#include "stm32f10x.h"

void NOP_nus(unsigned short nus)
{
	for(;nus>0;nus--)
	{__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();}
}
//模拟1629管脚配置
#define 	CS_1629_GPIO     		PD
#define 	CS_1629_BIT      		GPIO_Pin_1
#define 	CS_1629_Type     		Out_PP


#define 	CLK162X_GPIO     		PD
#define 	CLK162X_BIT      		GPIO_Pin_5
#define 	CLK162X_Type	     	Out_PP

#define 	SI_1629_GPIO     		PD
#define 	SI_1629_BIT      		GPIO_Pin_0
#define 	SI_1629_Type_Out    Out_PP
#define 	SI_1629_Type_In     IPU 

#define 	CS_1620_GPIO     		PD
#define 	CS_1620_BIT      		GPIO_Pin_4
#define 	CS_1620_Type     		Out_PP


#define 	SI_1620_GPIO     		PD
#define 	SI_1620_BIT      		GPIO_Pin_0
#define 	SI_1620_Type_Out    Out_PP
#define 	SI_1620_Type_In     IPU
//IO方向设置
#define CLK_OUT() 	 DK_GPIO_Init(CLK162X_GPIO,CLK162X_BIT,CLK162X_Type);

#define CS1629_OUT() DK_GPIO_Init(CS_1629_GPIO,CS_1629_BIT,CS_1629_Type);
#define CS1620_OUT() DK_GPIO_Init(CS_1620_GPIO,CS_1620_BIT,CS_1620_Type);

#define SI1629_IN()  DK_GPIO_Init(SI_1629_GPIO,SI_1629_BIT,SI_1629_Type_In);
#define SI1629_OUT() DK_GPIO_Init(SI_1629_GPIO,SI_1629_BIT,SI_1629_Type_Out);

#define SI1620_IN()  DK_GPIO_Init(SI_1620_GPIO,SI_1620_BIT,SI_1620_Type_In);
#define SI1620_OUT() DK_GPIO_Init(SI_1620_GPIO,SI_1620_BIT,SI_1620_Type_Out);
//IO操作函数	 
#define CLR_CS_TM1620    PDout(4) = 0 
#define SET_CS_TM1620    PDout(4) = 1 

#define CLR_CS_TM1629    PDout(1) = 0 
#define SET_CS_TM1629    PDout(1) = 1 

#define CLR_SCK_TM162x    PDout(5) = 0 
#define SET_SCK_TM162x    PDout(5) = 1 
	 
#define READ_SO1629    PDin(0)  
#define WRITE_SO1629   PDout(0) 

#define READ_SO1620    PDin(0)  
#define WRITE_SO1620   PDout(0)   

void 	Disp_Gpio_Init(void);
void	Disp_AllOn(void);
void	Disp_AllOff(void);
void	DISP_ErrorSub(uchar);
void	DispMainCode(void);
void	SetNetDatas(void);
void	DispSumConsumMoney(uchar,uchar,ulong);
void	Disp_Balance(uchar * );//显示余额
void	Disp_Hello(void);
void	DispConsumModeEnable(uchar ,uchar );//显示消费允许方式
void	DispDate(uchar * );//显示日期
void	DispTime(uchar *);//显示时间
void	Disp_Clr_Ram(void);//显示清空Pos;
void	Disp_Ver(uchar);//显示版本号
void	ChgIntToDispBuffer(uint,uchar * );
void	DISP_WriteError(uchar * );
void	DispMinBalance(void);//显示底金
void	DispOver(void);
uchar	FindMonth(uchar * );//查找日期
void	LED_Indata(uchar bbit,uchar aa);  //写入数据
uchar 	ReadByteFrom1629(void);//倒序接收
void 	LED_DispDatas_1629(uchar ledx,uchar * ptr,uchar num);
void 	LED_DispDatas(uchar bbit,uchar * ptr);
void 	LED_DispDatas_all(uchar * ptr);
void	ReakKeySub(void);
void 	LED_OneByte_Disp(uchar ledx,uchar prt);
void 	LED_NumFalsh_Disp(uchar ledx,uchar num,unsigned long delays);
void 	Led_Wait_Disp(void);
//显示控制管脚初始化
void Disp_Gpio_Init(void)
{
	  CLK_OUT();
	  SI1629_OUT();
	 SI1620_OUT();
	  CS1629_OUT();
	  CS1620_OUT();
	  DK_GPIO_Init(PC,GPIO_Pin_2,Out_PP);
	  WRITE_SO1620 = 0  ;
	  WRITE_SO1620 = 1 ;
	  WRITE_SO1629 = 0  ;
	  WRITE_SO1629 = 1 ;
	  CLR_CS_TM1629;
	  SET_CS_TM1629; 
	   CLR_CS_TM1620;
	  SET_CS_TM1620; 
	  CLR_SCK_TM162x ;
	  SET_SCK_TM162x ;
	  CLR_SCK_TM162x ; 
}

void	Disp_Hello(void) 
{
	memset(DispBuffer,0,10);
	memcpy(DispBuffer,"\x76\x79\x38\x38\x3f",5);
	LED_DispDatas_all(DispBuffer);
}

void	DispOver(void)
{
	memset(DispBuffer+4,0,6);
	DispBuffer[0]=0x3f;
	DispBuffer[1]=0x3e;
	DispBuffer[2]=0x79;
	DispBuffer[3]=0x50;	
	LED_DispDatas_all(DispBuffer);
}

void	Disp_AllOn(void)
{
	memset(DispBuffer,0xff,11);
	LED_DispDatas_all(DispBuffer);
}
uchar	FindMonth(uchar * ptr)
{
	 uchar	MString[37]={'J','a','n','F','e','b','M','a','r',
							 'A','p','r','M','a','y','J','u','n',
							 'J','u','l','A','u','g','S','e','p',
							 'O','c','t','N','o','v','D','e','c'};
	uchar	i=0;
	uchar	j;
	do
	{
		j=BytesComp(ptr,MString+3*i,3);
		if (!j) 
			return (i+1);
 		i++;
	}while (i<12);
    return (0);
}
void	Disp_AllOff(void)
{
	memset(DispBuffer,0,11);
	LED_DispDatas_all(DispBuffer);
}

int dispErrorcnt = 0;
extern u8 isOnJdq;
void	DISP_ErrorSub(uchar Num)
{
	memcpy(DispBuffer,DispErrorString+Num*16,10);
	LED_DispDatas_all(DispBuffer);
	
	dispErrorcnt = 0;
	
	if((CARD_LITTLEMONEY == Num || CARD_LOSTCARDNUM == Num) && isOnJdq == 0)
	{
		while(1)
		{
			ulong	ReadKeyValue,iii;	
			ReadKeyValue=ScanKeySub(KEY_ANY);//读按键
			if(ReadKeyValue == KEY_ESC)
			{
				
				
				break;
			}
			
			Clr_WatchDog();	
			
			dispErrorcnt++;
			if(dispErrorcnt >= 30)
			{
				dispErrorcnt = 0;
				BeepOn(1);
			}
			
		}
	}
	
}
//显示写卡错误
void	DISP_WriteError(uchar * ptr)
{
	uchar	i;
	uchar	st_data;
	uchar	LedX;
	//zjx_mark
	DispBuffer[0]=Disp0_9String[WriteCardSta%10];
	DispBuffer[1]=0x79;//E
	DispBuffer[2]=0x50;//r
	DispBuffer[3]=0x50;//r
//	DispBuffer[3]=0x40;
	LedX=4;	
	for (i=0;i<3;i++)
	{
		st_data=ptr[i+1];
		st_data>>=4;
		DispBuffer[LedX+2*i]=Disp0_9String[st_data];
		st_data=ptr[i+1];
		st_data&=0x0f;
		DispBuffer[LedX+2*i+1]=Disp0_9String[st_data];
	}
	LED_DispDatas_all(DispBuffer);
}
void	Disp_Ver(uchar Num)//显示版本号
{
	uchar	i;
	uchar	VerString[11];

	DispBuffer[10]=DispModeChar[ConsumMode];
	DispBuffer[9]=0;
	if (!Num)
	{
		memcpy(DispBuffer,PosVerString,5);
		memcpy(DispBuffer+5,PosVerString+5,5);
	}
	else if (Num==1)
	{
		memcpy(VerString,__DATE__,sizeof (__DATE__));//编译日期
		DispBuffer[0]=0x3e;//U
		DispBuffer[1]=0x40;
		i=VerString[9]-0x30;
	 	DispBuffer[2]=Disp0_9String[i];
		i=VerString[10]-0x30;	
		DispBuffer[3]=Disp0_9String[i];
		DispBuffer[4]=0x40;	
		i=FindMonth(VerString);	
		DispBuffer[5]=Disp0_9String[i/10];	
		DispBuffer[6]=Disp0_9String[i%10];	
		DispBuffer[7]=0x40;	
		i=VerString[4]-0x30;
	 	DispBuffer[8]=Disp0_9String[i];
		i=VerString[5]-0x30;	
		DispBuffer[9]=Disp0_9String[i];
	}
	else
	{
		DispBuffer[0]=0x6d;
		DispBuffer[1]=0x40;
		DispBuffer[2]=0x50;
		DispBuffer[3]=0x79;
		DispBuffer[4]=0;
		DispBuffer[5]=Disp0_9String[8];
		DispBuffer[6]=Disp0_9String[1];
		DispBuffer[7]=Disp0_9String[9];
		DispBuffer[8]=Disp0_9String[2];
		DispBuffer[9]=0;
	}
	LED_DispDatas_all(DispBuffer);
}
void	DispMainCode(void)//显示站点号
{
	uchar	LedX;
	DispBuffer[4]=0;
	LedX=5;	
	DispBuffer[0]=0x77;//A
	DispBuffer[1]=0x5e;//d
	DispBuffer[2]=0x5e;//d
	DispBuffer[3]=0x50;//r
	ChgIntToDispBuffer(MainCode,DispBuffer+LedX);
	LED_DispDatas_all(DispBuffer);
}

void	DispConsumModeEnable(uchar aa,uchar CharEnable)
{
	uchar	i,st_data;
	uchar	aaa[5];
	memset(DispBuffer,0,10);
	if (!aa)
	{
		DispBuffer[0]=0x77;//A
		if (CharEnable&0x10)
			memcpy(aaa,"\x3f\x73\x79\x37\x00",5);//OPEN
		else
			memcpy(aaa,"\x39\x38\x3f\x6d\x79",5);//CLOSE
	}
	else
	{
		DispBuffer[0]=0x38;//L
		if (CharEnable&0x20)
			memcpy(aaa,"\x3f\x73\x79\x37\x00",5);//OPEN
		else
			memcpy(aaa,"\x39\x38\x3f\x6d\x79",5);//CLOSE
	}
 	memcpy(DispBuffer+5,aaa,5);
	st_data=0;
	for (i=0;i<4;i++)
	{
		if (CharEnable & (1<<i))
			st_data|=DispModeChar[i];
	}
	DispBuffer[10]=st_data;
	LED_DispDatas_all(DispBuffer);
}

void	DispDate(uchar * ptr )//显示日期
{
	uchar	i,st_data;
	uchar	Position[6]={2,3,5,6,8,9};
	DispBuffer[0]=0x5e;//d
	DispBuffer[1]=0x40;//-
	DispBuffer[4]=0x40;//-
	DispBuffer[7]=0x40;//-
	for (i=0;i<3;i++)
	{
		st_data=ptr[i]>>4;
		DispBuffer[Position[2*i]]=Disp0_9String[st_data];
		st_data=ptr[i]&0x0f;
		DispBuffer[Position[2*i+1]]=Disp0_9String[st_data];
	}
	LED_DispDatas_all(DispBuffer);
}

void	DispTime(uchar * ptr )//显示时间
{
	uchar	i,st_data;
	uchar	Position[6]={2,3,5,6,8,9};
	DispBuffer[0]=0x78;//d
	DispBuffer[1]=0x40;//-
	DispBuffer[4]=0x40;//-
	DispBuffer[7]=0x40;//-
	for (i=0;i<3;i++)
	{
		st_data=ptr[i]>>4;
		DispBuffer[Position[2*i]]=Disp0_9String[st_data];
		st_data=ptr[i]&0x0f;
		DispBuffer[Position[2*i+1]]=Disp0_9String[st_data];
	}
	LED_DispDatas_all(DispBuffer);
}

void Disp_Aation_consume(uchar * ptr)
{
	 	LED_DispDatas_1629(2,(ptr+2),0);
}
void	DispSumConsumMoney(uchar Mode,uchar	Num,ulong  MoneyValue)
{
	uchar	aaa[10];
	uchar	bbb[8];
	uchar   Buffer[4];
	ulong   iii;
	memcpy(aaa,"\x40\x40\x40\x40\x00\x00\x00\x00\x00\x00",10);

	if (Mode==CONSUM_RATION)
	{
		RdBytesFromAT24C64(MenuPrince_Addr,Buffer,4);
		if (BytesCheckSum(Buffer,4) || BCD_String_Diag(Buffer,3))
			memset(Buffer,0,4);	
		iii=ChgBCDStringToUlong(Buffer,3);
	}
	else
	{
		if (Mode==CONSUM_NUM)
		{
			if (Num>50)
				Num=0;
			aaa[0]=Disp0_9String[Num/10];
			aaa[1]=Disp0_9String[Num%10];
		}
		iii=MoneyValue;
	}
	aaa[9]=Disp0_9String[0];
	ChgUlongToLongDispBuffer(iii,bbb);
	if (iii)
	{
		if (!bbb[2])
			memcpy(aaa+5,bbb+3,5);
		else if (!bbb[1])
			memcpy(aaa+5,bbb+2,5);
		else
			memcpy(aaa+5,bbb+1,5);	
	}
	memcpy(DispBuffer,aaa,10);
	//Mode = 4;
	DispBuffer[10]=DispModeChar[Mode];
	if (bitAddStatus|BatModeFlag)
		DispBuffer[10]|=0x01;
	else
	   DispBuffer[10]&=0xfe;
	
	if(canbutie != 0) 
	{
		DispBuffer[10]&=~(0x02);
	}else
	{
		DispBuffer[10]|=0x02;
	}
	
	if (Mode==CONSUM_RATION)
	{	 //Led_Wait_Disp();
		TIM3_Close_INT();
		LED_DispDatas_1629(5,(DispBuffer+5),5);
		Led_Wait_Disp();
		TIM3_OPen_INT();					
	}
	else  
		LED_DispDatas_all(DispBuffer);
}

void Disp_Blance(uchar * bolached)
{
		ulong	iii;
	uchar	aaa[8];
	uchar * bolache = dispBalance;
	
	if(dispBalance[0] == 0 && dispBalance[1] == 0 && dispBalance[2] == 0 && dispBalance[3] == 0)
	{
		DispBuffer[0] = DispBuffer[1] = DispBuffer[2] = DispBuffer[3] = 0x40;
		DispBuffer[4] = 0;
		
	}else
	{
		iii=ChgBCDStringToUlong(bolache,4);
		ChgUlongToLongDispBuffer(iii,aaa);

		if (!aaa[2])
			memcpy(DispBuffer,aaa+3,5);
		else if (!aaa[1])	
			memcpy(DispBuffer,aaa+2,5);
		else
			memcpy(DispBuffer,aaa+1,5);
		
	}
}

void	Disp_Balance(uchar	* Balance)//显示余额
{
	ulong	iii;
	uchar	aaa[8];
	iii=ChgBCDStringToUlong(Balance,4);
	ChgUlongToLongDispBuffer(iii,aaa);

	if (!aaa[2])
		memcpy(DispBuffer,aaa+3,5);
	else if (!aaa[1])	
		memcpy(DispBuffer,aaa+2,5);
	else
		memcpy(DispBuffer,aaa+1,5);
	ChgUlongToLongDispBuffer(CurrentConsumMoney,aaa);
	if (CurrentConsumMoney)
	{
		if (!aaa[2])
			memcpy(DispBuffer+5,aaa+3,5);
		else
			memcpy(DispBuffer+5,aaa+2,5);	
	}
	else
	{
		memcpy(DispBuffer+5,"\x00\x39\x00\x00\x0f",5);
		DispBuffer[7]=Disp0_9String[PurseUsingNum/10];
		DispBuffer[8]=Disp0_9String[PurseUsingNum%10];
	}
	DispBuffer[10]=DispModeChar[ConsumMode];
	if (bitAddStatus|BatModeFlag) //
		DispBuffer[10]|=0x01;
	else
	   DispBuffer[10]&=0xfe;
	LED_DispDatas_all(DispBuffer);
}

void	Disp_Clr_Ram(void)//显示清空Pos;
{

	DispBuffer[0]=0x00;	
	DispBuffer[1]=0x39;//C
	DispBuffer[2]=0x38;//L
	DispBuffer[3]=0x50;//r
	DispBuffer[4]=0x00;	
	DispBuffer[5]=0x00;	
	DispBuffer[6]=0x73;//P
	DispBuffer[7]=0x3f;//O	
	DispBuffer[8]=0x6D;//S
	DispBuffer[9]=0x00;//.
	LED_DispDatas_all(DispBuffer);
}
void	DispMinBalance(void)//显示底金
{
	ulong	lll;
	uchar	aaa[8];
	lll=ChgBCDStringToUlong(CardMinBalance,3);
	DispBuffer[0]=0x5e;//d
	DispBuffer[1]=0x0e;//J
	DispBuffer[2]=0x40;
	ChgUlongToLongDispBuffer(lll,aaa);	
	DispBuffer[3]=0x40;
	DispBuffer[4]=0x40;
	memcpy(DispBuffer+5,aaa+3,5);	
	DispBuffer[10]=DispModeChar[ConsumMode];
	LED_DispDatas_all(DispBuffer);
}
void	ChgIntToDispBuffer(uint	int_data,uchar * ptr)
{
	uchar	st_data;
	uint	jj=10000;
	uchar	i;
	for (i=0;i<5;i++)
	{
		st_data=int_data/jj;
		int_data=int_data%jj;
		jj=jj/10;
		ptr[i]=	Disp0_9String[st_data];
	}
}
//显示网卡参数
void	DispNetDatas(uchar aa)
{
	uchar	Buffer[24];
	uchar	aaa[5];
	uchar	i,st_data;
	uint	ii;

	//mw	调整设置网络参数时的显示
	uchar	TopicString[5*19]={0x06,0x73,0x06,0x40,0x06,//	IP1-1
								  0x06,0x73,0x06,0x40,0x5b,//   IP1-2
								  0x06,0x73,0x06,0x40,0x4f,//   IP1-3
								  0x06,0x73,0x06,0x40,0x66,//   IP1-4//本机地址
								  0x73,0x78,0x06,0x00,0x00,//   pt1//本地端口
								  0x6f,0x78,0x00,0x40,0x06,//	gt -1
								  0x6f,0x78,0x00,0x40,0x5b,//   gt -2
								  0x6f,0x78,0x00,0x40,0x4f,//   gt -3
								  0x6f,0x78,0x00,0x40,0x66,//   gt -4//网关
								  0x6d,0x3e,0x7c,0x40,0x06,//	sub-1
								  0x6d,0x3e,0x7c,0x40,0x5b,//   sub-2
								  0x6d,0x3e,0x7c,0x40,0x4f,//   sub-3
								  0x6d,0x3e,0x7c,0x40,0x66,//   sub-4//子网掩码
								  0x00,0x00,0x00,0x00,0x00,//        //物理地址
								  0x06,0x73,0x5b,0x40,0x06,//	IP2-1
								  0x06,0x73,0x5b,0x40,0x5b,//   IP2-2
								  0x06,0x73,0x5b,0x40,0x4f,//   IP2-3
								  0x06,0x73,0x5b,0x40,0x66,//   IP2-4//远端IP
								  0x73,0x78,0x5b,0x00,0x00};//  pt2//远端端口
	//

	uchar	BufferX[19]={0,1,2,3,4,6,7,8,9,10,11,12,13,14,16,17,18,19,20};
	RdBytesFromAT24C64(TcpCardDatas_Addr,Buffer,24);
	if (Buffer[0]!=0xa0 || BytesCheckSum(Buffer,24))
		memcpy(Buffer+1,DefaultNetDatas,22);//默认的网络参数
	memcpy(DispBuffer,TopicString+5*aa,5);
	i=BufferX[aa];
	if (aa!=4 && aa!=13 && aa!=18)
	{
			st_data=Buffer[i+1];//mw
			DispBuffer[7]=0;
			DispBuffer[8]=0;
			DispBuffer[9]=0;

			DispBuffer[5]=Disp0_9String[st_data/100];
			st_data=st_data%100;
			DispBuffer[6]=Disp0_9String[st_data/10];
			DispBuffer[7]=Disp0_9String[st_data%10];			
	}
	else
	{
		memcpy((uchar *)&ii,Buffer+1+i,2);
		ChgIntToStrings(ii,aaa);
		for (i=0;i<5;i++)
			DispBuffer[5+i]=Disp0_9String[aaa[i]];
	}
	if(aa==4)
	{
		DispBuffer[6]=0x06;
		DispBuffer[7]=0x7d;
		DispBuffer[8]=0x3f;
		DispBuffer[9]=0x3f;
	}
	LED_DispDatas_all(DispBuffer);
}
static uchar	Bufferb[24];;
void	SetNetDatas(void)
{
	volatile uchar	InputX=0;
	volatile	uchar	InputY=0;
	uchar	st_data;
	uchar	i;
	int sdf = 0;
	uint	ii;
	ulong	ReadKeyValue,iii;
	uchar	bbit=1;
	uchar	bitHaveInputDatas=0;
	uchar	Buffera[24];
	//uchar	Bufferb[24];
	uint16_t tempbuf[24];
	//mw	调整设置网络参数时的显示
	const uchar	TopicString[5*19]={0x06,0x73,0x06,0x40,0x06,//	IP1-1
								  0x06,0x73,0x06,0x40,0x5b,//   IP1-2
								  0x06,0x73,0x06,0x40,0x4f,//   IP1-3
								  0x06,0x73,0x06,0x40,0x66,//   IP1-4//本机地址
								  0x73,0x78,0x06,0x00,0x00,//   pt1//本地端口
								  0x6f,0x78,0x00,0x40,0x06,//	gt -1
								  0x6f,0x78,0x00,0x40,0x5b,//   gt -2
								  0x6f,0x78,0x00,0x40,0x4f,//   gt -3
								  0x6f,0x78,0x00,0x40,0x66,//   gt -4//网关
								  0x6d,0x3e,0x7c,0x40,0x06,//	sub-1
								  0x6d,0x3e,0x7c,0x40,0x5b,//   sub-2
								  0x6d,0x3e,0x7c,0x40,0x4f,//   sub-3
								  0x6d,0x3e,0x7c,0x40,0x66,//   sub-4//子网掩码
								  0x73,0x76,0x77,0x00,0x00,//   PHA//物理地址
								  0x06,0x73,0x5b,0x40,0x06,//	IP2-1
								  0x06,0x73,0x5b,0x40,0x5b,//   IP2-2
								  0x06,0x73,0x5b,0x40,0x4f,//   IP2-3
								  0x06,0x73,0x5b,0x40,0x66,//   IP2-4//远端IP
								  0x73,0x78,0x5b,0x00,0x00};//  pt2//远端端口
	//
	const uchar	BufferX[19]={0,1,2,3,4,6,7,8,9,10,11,12,13,14,16,17,18,19,20};

	RdBytesFromAT24C64(TcpCardDatas_Addr,Buffera,24);
	if (Buffera[0]!=0xa0 || BytesCheckSum(Buffera,24))
	{
		//memcpy(Buffera+1,DefaultNetDatas,22);//默认的网络参数
		for(i=0;i<11;i++)
		{
			tempbuf[i] = FLASH_ReadHalfWord(TcpCardDatasFlashAdd +i*2);
			Bufferb[i*2+1] = tempbuf[i]>>8 ;
			Bufferb[i*2+2] = tempbuf[i] ;
			
			//memset(Bufferb,0,1);
		}
	}
	
	Buffera[0]=0xff;
	Buffera[23]=0x08;
	InputCount=0;
	InputBuffer[0]=Buffera[1]/100;
	i=Buffera[1]%100;							
	InputBuffer[1]=i/10;
	InputBuffer[2]=i%10;
	while (1)
	{
InStatr:
		LED_NumFalsh_Disp(5+InputCount,1,0x10000);
		ReadKeyValue=ScanKeySub(KEY_0_9 |KEY_ESC | KEY_ENTER);
		if (ReadKeyValue==KEY_ESC)
			return;
		else if (ReadKeyValue==KEY_ENTER)
		{
			if (bitHaveInputDatas)
			{//上次输入的数据转换为数据存储在Buffer内
				bitHaveInputDatas=0;
				if (InputX!=4 && InputX!=13 && InputX!=18)
				{//字节输入
					ii=InputBuffer[0]*100+InputBuffer[1]*10+InputBuffer[2];
					i=BufferX[InputX];
					if (ii<256)
						Buffera[i+1]=(uchar)ii;	
					else
					{
						BeepOn(3);
						bbit=1;
						InputCount=0;
						InputBuffer[0]=Buffera[1+i]/100;
						st_data=Buffera[1+i]%100;
						InputBuffer[1]=st_data/10;
						InputBuffer[2]=st_data%10;							
						goto  InStatr;	
					}
				}
				else
				{
					iii=(ulong)InputBuffer[0]*10000+InputBuffer[1]*1000+InputBuffer[2]*100+InputBuffer[3]*10+InputBuffer[4];
					i=BufferX[InputX];
					if (iii<65536)
					{
						ii=(uint)iii;
						memcpy(Buffera+1+i,(uchar *)&ii,2);
					}
					else
					{
						BeepOn(3);
						bbit=1;
						InputCount=0;
						memcpy((uchar *)&ii,Buffera+1+i,2);
						ChgIntToStrings(ii,InputBuffer);
						goto	InStatr;
					}
				}
			}
			if (ReadKeyValue==KEY_ENTER)
			{
				bbit=1;
				if (InputX==18)
					break;
				else
					InputX++;
			}
			i=BufferX[InputX];//Buffer下标
			if (InputX!=4 && InputX!=13 && InputX!=18)
			{
				memset(InputBuffer,0,5);
				InputBuffer[0]=Buffera[1+i]/100;
				st_data=Buffera[1+i]%100;
				InputBuffer[1]=st_data/10;
				InputBuffer[2]=st_data%10;					
			}
			else
			{
				memcpy((uchar *)&ii,Buffera+1+i,2);
				ChgIntToStrings(ii,InputBuffer);
			}
		}
		else if (ReadKeyValue & KEY_0_9)
		{
			bitHaveInputDatas=1;
			InputBuffer[InputCount]=ChgKeyValueToChar(ReadKeyValue);
			InputCount++;
			if (InputX!=4 && InputX!=13 && InputX!=18)
				InputCount=InputCount%3;
			else
				InputCount=InputCount%5;
			bbit=1;
		}
		if (bbit)
		{//显示
			bbit=0;
			memset(DispBuffer,0,10);
			memcpy(DispBuffer,TopicString+5*InputX,5);
			if (InputX!=4 && InputX!=13 && InputX!=18)
			{
				DispBuffer[5]=Disp0_9String[InputBuffer[0]];
				DispBuffer[6]=Disp0_9String[InputBuffer[1]];
				DispBuffer[7]=Disp0_9String[InputBuffer[2]];
			}
			else
			{
				for (i=0;i<5;i++)
					DispBuffer[5+i]=Disp0_9String[InputBuffer[i]];
			}
			LED_DispDatas_all(DispBuffer);
//			sdf++;
//			if(sdf >= 10) 
//			{
//			
//			
//				sdf = 0;
//			}
		}
		Clr_WatchDog();	
	}
	Buffera[0]=0xa0;
	Buffera[23]=CalCheckSum(Buffera,23);
	WrBytesToAT24C64(TcpCardDatas_Addr,Buffera,24);
	
	for(i=0;i<22;)
	{
		tempbuf[i/2] = (Buffera[i+1]) *256 + (uint16_t)Buffera[i+2];
		i+=2;
	}
	writeSysParameterToFlash(TcpCardDatasFlashAdd,tempbuf,22);
}

//========================================================================
void	LED_Indata(uchar bbit,uchar aa)  //写入数据
{
	uchar	i;			
	if(!bbit)
	{//此处的括号不能省略
		CLR_CS_TM1620;
	}
	else
	{//此处的括号不能省略
		CLR_CS_TM1629;
	}
		
	for (i=0;i<8;i++)
	{
		//SCK_TM1620=0;
		CLR_SCK_TM162x;
		if ((aa&0x01)!=0)
		{
		  //SI_TM1620=1;
		 SI1620_OUT();
		  WRITE_SO1620 = 1;
		}	
			
		else
		{
			//SI_TM1620=0;
			SI1620_OUT();
		  	WRITE_SO1620 = 0;
		}
			
		//SCK_TM1620=1;
		SET_SCK_TM162x;
		aa>>=1;			
	}
}

uchar ReadByteFrom1629(void)//倒序接收
{    
	uchar  i=8,aa=0;
	
    //SI_TM1629=1;
	WRITE_SO1629 = 1;
	SI1629_IN();
    CLR_CS_TM1629;
    do
    {
		SET_SCK_TM162x;//拉高数据.
		 NOP_nus(5);
		 CLR_SCK_TM162x;//在SCK为低时读入SI数据线上的数据,作为输出一个位..
		NOP_nus	(5);
        aa<<=1;
		//SI1629_IN();
		 NOP_nus(5);
		if(READ_SO1629) 
			aa|=0x01;
    }while(--i);
	SI1629_OUT();
    return(aa);
}

void LED_DispDatas_1629(uchar ledx,uchar * ptr,uchar num)
{
	uchar	i;
	
	SI1629_OUT();
	//SI_TM1629=1;
	WRITE_SO1629 = 1;

	SET_SCK_TM162x;
	SET_CS_TM1629;
	LED_Indata(1,0x40);
	SET_CS_TM1629;
	LED_Indata(1,(0xc0+(ledx-2)*2));
				
	for (i=0;i<num;i++)
	{		
		LED_Indata(1,ptr[i]);
		LED_Indata(1,0);
	}
	SET_CS_TM1629;
//	LED_Indata(1,0x8b);
	SET_CS_TM1629;				
}

void LED_DispDatas(uchar bbit,uchar * ptr)
{
	uchar	i;

	SI1620_OUT();
	//SI_TM1620=1;
	WRITE_SO1620 = 1;

	SET_SCK_TM162x;
	SET_CS_TM1620;
	LED_Indata(0,0x40);
	SET_CS_TM1620;
	if(!bbit)//全显示
	{
		LED_Indata(0,0xc0);					
		for (i=0;i<3;i++)
		{	
			if(i<2)	
				LED_Indata(0,ptr[i]);
			else
				LED_Indata(0,ptr[10]);				
			LED_Indata(0,0);	
		}
	}
	else
	{
		LED_Indata(0,0xc4);					
		LED_Indata(0,ptr[10]);				
		LED_Indata(0,0);
	}
	SET_CS_TM1620;
//	LED_Indata(0,0x8b);
	SET_CS_TM1620;			
}

void LED_DispDatas_all(uchar * ptr)
{   
	Forbidden=1;
//	ET0=0;	
	TIM3_Close_INT();
	LED_DispDatas(0,ptr);
	LED_DispDatas_1629(2,(ptr+2),8);
	//ET0=1;
	TIM3_OPen_INT();
	Forbidden=0;
}

void	ReakKeySub(void)
{
	uchar	i,aa;

	SET_CS_TM1629;
	aa=0x42;
	CLR_CS_TM1629;		
	for (i=0;i<8;i++)
	{
		CLR_SCK_TM162x;
		if ((aa&0x01)!=0)
		{	
			//SI_TM1629=1;
			SI1629_OUT();
			WRITE_SO1629 = 1;
		}
		else
		{
			//SI_TM1629=0;
			SI1629_OUT();
			WRITE_SO1629 = 0;
		}
		SET_SCK_TM162x;
		aa>>=1;			
	}
	for(i=0;i<4;i++)
		keybuff[i]=ReadByteFrom1629();

	SET_CS_TM1629;
}
void LED_OneByte_Disp(uchar ledx,uchar prt)//固定地址
{
	SI1629_OUT();
	//SI_TM1629=1;
	WRITE_SO1629 = 1;

	SET_SCK_TM162x;
	if(ledx<2)
	{
		SET_CS_TM1620;
		LED_Indata(0,0x44);
		SET_CS_TM1620;
		LED_Indata(0,(0xc0+ledx*2));//跳过没有用到的字节 
		LED_Indata(0,prt);//只写一个字节
		SET_CS_TM1620;
	}
	else
	{
		SET_CS_TM1629;
		LED_Indata(1,0x44);
		SET_CS_TM1629;
		LED_Indata(1,(0xc0+(ledx-2)*2));
		LED_Indata(1,prt);
		SET_CS_TM1629;
	}
}
//连续LED闪动效果
//ledx:起始位置; num:个数; delays:延时圈数
void LED_NumFalsh_Disp(uchar ledx,uchar num,unsigned long delays)
{
    uchar i;
	Forbidden=1;
	//ET0=0;
//	TIM3_Close_INT();

	Falsh_TimeCount++;
	if(Falsh_TimeCount==delays)
	{
		for(i=0;i<num;i++)
		{
			LED_OneByte_Disp(ledx+i,DispBuffer[ledx+i]);
		}
	}
	else if(Falsh_TimeCount==(delays*2))
	{
		Falsh_TimeCount=0;
		for(i=0;i<num;i++)
		{
			LED_OneByte_Disp(ledx+i,0);
		}
	}
	else if(Falsh_TimeCount>(delays*2))	
	{
		Falsh_TimeCount=0;	
	}
	//ET0=1;
//	TIM3_OPen_INT();

	Forbidden=0;
}

//放卡效果显示
void Led_Wait_Disp(void)
{
	Falsh_TimeCount++;
	if(Falsh_TimeCount==8)
	{
		memset(DispBuffer,0,5);
		DispBuffer[LedWattingChar]=0x40;
		LedWattingChar++;
		LedWattingChar%=5;
		Forbidden=1;
		//ET0=0;
		//TIM3_Close_INT();

		LED_DispDatas(0,DispBuffer);
		LED_DispDatas_1629(2,(DispBuffer+2),3);

		//ET0=1;
	//	TIM3_OPen_INT();

		Forbidden=0;
	}
	else if(Falsh_TimeCount>8)	
	{
		Falsh_TimeCount=0;	
	}
	
}

//修改数码管的亮度
void ChangeLedBright(uchar ptr)
{
  	SET_CS_TM1620;
	LED_Indata(0,ptr);
	SET_CS_TM1620;
	SET_CS_TM1629;
	LED_Indata(1,ptr);
	SET_CS_TM1629;
}





