
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "MyDefine.h"
#include "char.h"


extern	uint		MainCode;//վ���
extern  uint		UdpCode;//UDP�˿ں�
extern	unsigned char		bitPinEnable;
extern	unsigned char		PinCheckSta;
extern	unsigned char		PinCount;
extern	unsigned char		bitNeedRST;
extern	unsigned char		bitNeedDownLoad;
extern	unsigned char		bitPurseEnd;

extern	uint						SerialReceiveTimeCount;
extern	unsigned char		TCPDatasComd;
extern	unsigned char		OutChar16C550;
extern	unsigned char		CommModeChar;
extern unsigned char canbutie;

extern	unsigned char		Receive_Ip_Port[6];

extern	unsigned char		CtrlChar;//�����ֽ��������Ҫͨ��573���Ʒ�������485ͨѶ���շ�ת��
extern	unsigned char		LoopCount;
extern	unsigned char		bitUpdateParameter;
extern	unsigned char		bitHaveReadBalance; 
extern	unsigned char		bitSetDateEnable; 
extern	unsigned char		bitWrittingFlash;
extern	unsigned char		bitDispFlash;
extern	unsigned char		bitBeepError;
extern	unsigned char		bitHaveCollectRecord;
extern	unsigned char		bitHaveReCollectRecord;
extern	unsigned char		bitStatus8563;
extern	unsigned char		bitSysTimeDisable;
extern	unsigned char		bitRecordFull;
extern	unsigned char		bitConsumZeroEnable;
extern	unsigned char		bitHaveSeclectPurse;
extern	unsigned char		bitCommStatus;


extern	unsigned char		StatusBak;
extern	unsigned char		DelayReadCardTimeCount;

extern	unsigned char		DownLoadPragamSymbl[6];
extern	unsigned char		LoadProgamReDatas[6];
extern	unsigned char		T3_TimeCount;
extern	unsigned char		bitHaveLedError;
extern	unsigned char		bitNeedDiagRecordFull;

extern	unsigned char		ConsumCase;
extern	unsigned char		ConsumMode;//���ѷ�ʽ
extern	const uchar			DefaultNetDatas[22];

extern	unsigned long		Sys_SumConsumMoney;	
extern	unsigned char		CardDayConsumCount;
extern	unsigned char		PurseContrlNum[2];
extern  unsigned char   PurseConsumCount[2];
extern	unsigned char		PurseUsingNum;
extern	unsigned char		SelectPurseNum;
extern	unsigned char		CommEncryptKey[8];
extern	unsigned char		PurseEnable[10];
extern	unsigned char		CardBatchEnable[32];//�����Ƿ���Ч
extern	unsigned char		CardMinBalance[3];
extern	unsigned long		Limit_DayMoney;//���޶�
extern	unsigned long		Max_ConsumMoney;
extern	unsigned char		PursesSector[10];
extern	unsigned char		ConsumModeEnable;//���ѷ�ʽ����
extern	unsigned long		SumConsumMoney;//�����Ѷ�
extern	unsigned long		SumPrintMoney;//��ӡ�������Ѷ�
extern	unsigned char		bitNeedPrinter;//1-��Ҫ��ӡ

extern	unsigned long		CurrentConsumMoney;//�������Ѷ�
extern	unsigned char		MenuSort;//�˺�
extern	unsigned char		InputMenuSort;
extern	unsigned char		Mac2Bak[4];
extern	unsigned char		ConsumSndInfo[60];
extern	unsigned char		ConsumCountDateTime[4];//�ƴ���������ʱ��
extern	unsigned char		CardEnableMonths;//��������Ч����
extern	unsigned char		DispCount;
extern	uint						BeepDelayTimeCount;//������
extern  uint        		BeepTimes;

extern	unsigned char		bitHaveKey;
extern	unsigned char		bitHaveReleaseKey;
extern	unsigned long		KeyValue;
extern	unsigned char		InputCase;
extern	unsigned char		InputBuffer[8];
extern	unsigned char		InputCount;
extern	unsigned char		bitHaveInputDatas;
extern	unsigned char		InputNum;
extern	unsigned char		bitInPutPoint;
extern	unsigned char		InputMaxNum;
extern	unsigned long		SingleMoney;
extern	uint						MulData;
extern	unsigned char		CardConsumTime[6];

extern	union	sDatasUnion   SerialUnion;

extern		unsigned char		SerialReceiveLen;
extern		unsigned char		SerialReCase;
extern		unsigned char		rChar;
extern		uint						SerialReCheckSum;
extern		unsigned char		SerialReCheckSumCount;

extern	unsigned char		  bitEven;
extern	unsigned char			bitSerial_ReceiveEnd;
extern	unsigned char			bitSerial_SendRequest;
extern	unsigned char     bitUARTSendEnd;

extern	unsigned char		  bitPF8563Error;
extern	union	uTimeUnion	SysTimeDatas;
extern	union	uTimeUnion	SysTimeDatasBak;

extern	uint		RecordSum;//��¼����
extern	uint		NoCollectRecordSum;//û�вɼ��ļ�¼����
extern	uint		NoCollectRecordIndex;//û�вɼ��ļ�¼ָ��
extern	uint		ReCollectRecordIndex;//�Ѿ��ɼ��ļ�¼ָ��
extern	uint		SaveRecordIndex;//�洢��¼ָ��
extern	unsigned char		SerialSendNoCollectNum;//�ϴδ��͵�δ�ɼ�¼����
extern	unsigned char		SerialSendReCollectNum;//�ϴδ��͵ĸ��ɼ�¼����


extern	unsigned char		CardType[2];
extern	unsigned char		CardSerialNum[4];
extern	unsigned char		CardSerialNumBak[4];
extern	unsigned char		CardBatch;//������
extern	unsigned char		CardIdentity;

extern	unsigned char		CardPrinterNum[4];//ӡˢ���
extern	unsigned char		OldBalance[4];
extern	unsigned char		NewBalance[4];
extern unsigned char		dispBalance[4];

extern	unsigned long		CardDayConsumMoney;//���ϵ������Ѷ�
extern	unsigned char		CardConsumDate[3];//���ϵ�����������

extern	 	unsigned char	DispModeChar[6];
extern	 	unsigned char	Disp0_9String[10];
extern	 	unsigned char	PaultRate9600Symbl[4];
extern	 	unsigned char	PaultRate4800Symbl[4];

extern	 	unsigned char	DownLoadDatas[6];
extern	 	unsigned char	AllFF[32];
extern	 	const unsigned char	DispErrorString[34*16];

extern  unsigned char 	CPU_CommBuf[50];
extern  unsigned char 	*CPU_RcLen;
extern	unsigned char   PsamRcvBufCount;
extern	unsigned char   PsamRcvBuf[50];//psam


extern	unsigned char  	Card_Rebate;
extern	unsigned char  	Sys_PinCode[3];
extern	uint   	   		  PosConsumCount;
extern	unsigned char  	WriteCardSta;//д��״̬
extern	unsigned char		MatchCode[4];//ƥ����
extern	unsigned char   ComChallenge[5];
extern  unsigned char   BeepCotrl;

extern	unsigned char		PosVerString[10];
extern	unsigned char		LedWattingChar;
extern 	unsigned char		BatModeFlag;

//zjx_change_20110905
extern	unsigned char 	keybuff[4];
extern	unsigned long  	Falsh_TimeCount;
extern	unsigned char		DispBuffer[11];//��ʾ����

extern	unsigned char		bitAddStatus;
extern	unsigned char	  Forbidden;
extern	unsigned char   ReadKey_Pause; 
extern	unsigned char		bitHaveAuto;
extern	unsigned char		bitLookSysDatas;
extern	unsigned char		bitLookD;

extern	unsigned char 	NetCardDatas[22];
extern	unsigned char 	TypeA_Sort;//M1=0/CPU=1
extern	unsigned char 	Bak_Sort;

extern	unsigned char		CardKeyCode[6];//��������
extern	unsigned char		CalCardKey[8];//����Կ
extern	unsigned char		CardSector;//��������������

extern	unsigned char	 	PinCode[3];
extern	unsigned char		Limit_MoneySign;
extern	unsigned char		DaySumMoneyDatasBak[28];

extern	unsigned char		PurseWrBufferBak[25];
extern	unsigned char		MainWrBufferBak[29];

extern	unsigned char		PurseConsumDateTime[4];
extern	unsigned long		PurseSumConsumMoney;
extern	unsigned char		PurseBT_Num[2];//����������
extern	unsigned char   DefaultKind[4];

extern	unsigned char		Flag_NotDefault;
extern	unsigned char		bitUseMoneyBlock;
extern	unsigned char   Flag_BakRecode;

extern	unsigned char		PurseDatas_Info[24];//Ǯ�����ݱ���

extern	unsigned char  	rebufff[70];
extern	unsigned char  	ser0_BytesReceived;
extern	unsigned char  	ser0_ReceiveLen;

//add by lc
extern uint  	RecDelay;  
extern uchar 	Udp_Send_Flag; 
extern u32 		Led_Open_time ;
extern uchar 	STX_FLAG;
extern uchar 	wifi_mode_flag ;
extern uchar	LedTcpIndex;//��ʾTCPIP�������������
extern uchar 	LoadModeResponse[15];
extern uchar  LoadModeFlag ;//ad by lc
extern uchar  CpuID[3];
extern uint 	ClientPort;//�ͻ��˶˿ں�
extern uint 	POSServerPort;
extern uchar 	RemoteIP[4];
extern uchar	ReWriteCardDatasBak[16];//д��ǰ��Ҫ���ǵ�����

extern uint 	ClientPort;//�ͻ��˶˿ں�
extern uchar 	RemoteIP[4];
extern uchar 	RemoteIPFlag;
extern uchar 	EscComsumeSaveRecordFlag ;
extern uchar 	BeepFlag ;
extern uchar	NeedResartFlag;
extern ulong  LocalTime;
extern u8     StxRemoteIp[4];
extern u8     FirstSendStx;
extern u8  		Rebate_YesOrNo_Flag ;
extern uchar DayLimetMoney[3];//���ص����޶�
extern uchar DayLimetFlag;
extern ulong ResetEnc28j60Num ;
extern uchar FirstUdpConnect;//��һ�ν�������
extern uchar Reset_Wifi_Flag ;
extern uchar ShuRuMoney_Err ;//������벻��ȫ

