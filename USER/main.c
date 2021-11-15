	
#include "includes.h"
#include "ExternVoidDef.h"
#include "beep.h"
#include "mfrc522.h"
#include "ExternVariableDef.h"
#include "VariableDef.h"
#include "W5500.h"	

extern void Jmp_Loader(void);
extern u32 consume_num;
extern u8 macaddress[6];


extern void W5500_Initialization(void);
extern void Load_Net_Parameters(void);
extern void W5500_Socket_Set(void);
extern void Process_Socket_Data(SOCKET );
extern void udp_server_send_data();
void Process_Socket2_Data(SOCKET s);
extern u8 S1_data;
void chekc_w5500();
uchar isHaveBTRecord = 0;
uchar getRecord(uchar * data);
extern uchar UARTFrame;
void udp_server_send_data_socket1();
extern uchar getbtrec;

int main(void)
{	

	uchar BatModeFlagBag;
	u8 CardKey[6];
	uchar BTdata[32];
	
	memcpy(CardSerialNum,"\x11\x22\x33\x44",4);
	memset(CalCardKey,2,8);
	
	CalPurseKeyCode(CardSerialNum,MatchCode,CalCardKey,CardKey);			
	Clr_WatchDog();
	BSP_Init(); //硬件设备、系统时钟初始化
		
	while(1)
	{	
		//MainCode = 1;
	//**************************//检测电池模式还是电源模式**********************************************//
		BatModeFlag =PCin(2);
		
		if(BatModeFlag !=BatModeFlagBag)	 //每次模式转换响两声
		{
			if(BatModeFlag)
			{
				ChangeLedBright(0x8b); //修改数码管亮度低
				DispBuffer[10]|=0x01;
			}
			else
			{
				ChangeLedBright(0x8a); //修改数码管亮度高
				 DispBuffer[10]&=0xfe;
			}
			BeepOn(2);
		}
		BatModeFlagBag =  BatModeFlag;
	//******************************************************************************************************//			
	 
	if (!LoopCount)
			bitUpdateParameter=1;
		ReadSysParameter(1);
		LoopCount++;
		if (bitRecordFull)
		{//记录已满，如果有记录采集，重新诊断
			if (bitHaveCollectRecord)
			{
				bitHaveCollectRecord=0;
				bitRecordFull=RecordFullDiag();//擦除
				if (!bitRecordFull)
					DispSumConsumMoney(ConsumMode,0xff,0);
			}
		}
		else
		{//如果有消费
			if (bitNeedDiagRecordFull)
			{
				bitNeedDiagRecordFull=0;
				bitRecordFull=RecordFullDiag();//擦除
		 	}
		}
		if (bitRecordFull)
			DISP_ErrorSub(SYS_SAVERECORDFULL);
		else
		{
			ConsumSub();//消费			
		}			
		if(CommModeChar == 1)
		{		
				if(getbtrec)
				{
					isHaveBTRecord = getRecord(BTdata);
					if(isHaveBTRecord)
					{
						bitSerial_ReceiveEnd=1;
						SerialUnion.S_DatasStruct.UartStatus=0;
						SerialUnion.S_DatasStruct.UartAddrH=0;
						SerialUnion.S_DatasStruct.UartAddrL=0;
						SerialUnion.S_DatasStruct.UartFrameNum=UARTFrame++;
						SerialUnion.S_DatasStruct.UartComd=BTRECORD_COMD;
						SerialUnion.S_DatasStruct.DatasLen=31;
						memcpy(SerialUnion.S_DatasStruct.Datas,BTdata+1,31);
						memcpy(Receive_Ip_Port,NetCardDatas+16,6);
						bitSerial_SendRequest=1;
						udp_server_send_data_socket1();
					}
					getbtrec = 0;
				}
				
				chekc_w5500();
				W5500_Socket_Set();//W5500端口初始化配置

				if(W5500_Interrupt)//处理W5500中断		
				{
					W5500_Interrupt_Process();//W5500中断处理程序框架
				}
				if((S0_Data & S_RECEIVE) == S_RECEIVE)//如果Socket0接收到数据
				{
					S0_Data&=~S_RECEIVE;
					Process_Socket_Data(0);//W5500接收数据
				}		

				UART_ReceiveSub();	  //UDP接收数据指令处理
				udp_server_send_data();
				
				if(S1_data != 0)
				{
					S1_data = 0;
					Process_Socket2_Data(1);
				}
				
				UART_ReceiveSub();	  //UDP接收数据指令处理
				udp_server_send_data();
				//	   //发送完成后对存储变量初始化
			if(bitNeedDownLoad) //在线下载
			{
				LoadModeFlag = 0;
				Jmp_Loader();  //下载程序的钩子函数
				LoadModeFlag = 0;
			}
			if(STX_FLAG ==1)
			{	
				STX_FLAG =0;
				while(1);
			}			
			
		}
		else
		{
			if(bitNeedDownLoad) //在线下载
			{
				LoadModeFlag = 0;
				Jmp_Loader();  //下载程序的钩子函数
				LoadModeFlag = 0;
			}
			
		 	UART_ReceiveSub();	//485模式
			UART_SendSub();	
			if(STX_FLAG ==1)
			{	
				STX_FLAG =0;
				while(1);
			}
		}
		Clr_WatchDog();	
	}  
} 

