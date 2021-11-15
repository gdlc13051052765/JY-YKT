	
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
	BSP_Init(); //Ӳ���豸��ϵͳʱ�ӳ�ʼ��
		
	while(1)
	{	
		//MainCode = 1;
	//**************************//�����ģʽ���ǵ�Դģʽ**********************************************//
		BatModeFlag =PCin(2);
		
		if(BatModeFlag !=BatModeFlagBag)	 //ÿ��ģʽת��������
		{
			if(BatModeFlag)
			{
				ChangeLedBright(0x8b); //�޸���������ȵ�
				DispBuffer[10]|=0x01;
			}
			else
			{
				ChangeLedBright(0x8a); //�޸���������ȸ�
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
		{//��¼����������м�¼�ɼ����������
			if (bitHaveCollectRecord)
			{
				bitHaveCollectRecord=0;
				bitRecordFull=RecordFullDiag();//����
				if (!bitRecordFull)
					DispSumConsumMoney(ConsumMode,0xff,0);
			}
		}
		else
		{//���������
			if (bitNeedDiagRecordFull)
			{
				bitNeedDiagRecordFull=0;
				bitRecordFull=RecordFullDiag();//����
		 	}
		}
		if (bitRecordFull)
			DISP_ErrorSub(SYS_SAVERECORDFULL);
		else
		{
			ConsumSub();//����			
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
				W5500_Socket_Set();//W5500�˿ڳ�ʼ������

				if(W5500_Interrupt)//����W5500�ж�		
				{
					W5500_Interrupt_Process();//W5500�жϴ��������
				}
				if((S0_Data & S_RECEIVE) == S_RECEIVE)//���Socket0���յ�����
				{
					S0_Data&=~S_RECEIVE;
					Process_Socket_Data(0);//W5500��������
				}		

				UART_ReceiveSub();	  //UDP��������ָ���
				udp_server_send_data();
				
				if(S1_data != 0)
				{
					S1_data = 0;
					Process_Socket2_Data(1);
				}
				
				UART_ReceiveSub();	  //UDP��������ָ���
				udp_server_send_data();
				//	   //������ɺ�Դ洢������ʼ��
			if(bitNeedDownLoad) //��������
			{
				LoadModeFlag = 0;
				Jmp_Loader();  //���س���Ĺ��Ӻ���
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
			if(bitNeedDownLoad) //��������
			{
				LoadModeFlag = 0;
				Jmp_Loader();  //���س���Ĺ��Ӻ���
				LoadModeFlag = 0;
			}
			
		 	UART_ReceiveSub();	//485ģʽ
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

