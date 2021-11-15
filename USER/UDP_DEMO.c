#include "stm32f10x.h"
#include "stm32f10x_spi.h"	
#include "stm32f10x_exti.h"	
#include "W5500.h"
#include "string.h" 
#include "spi.h" 
#include "char.h" 
#include "MyDefine.h"
#include "ExternVoidDef.h"
#include "ExternVariableDef.h"
#include "stm32f10x_flash.h"

unsigned int udp_len;			//udp�������ݳ���
void RCC_Configuration(void);		//����ϵͳʱ��Ϊ72MHZ(������Ը�����Ҫ��)
void NVIC_Configuration(void);		//STM32�ж�������������
void Timer2_Init_Config(void);		//Timer2��ʼ������
void System_Initialization(void);	//STM32ϵͳ��ʼ������(��ʼ��STM32ʱ�Ӽ�����)
void Delay(unsigned int d);			//��ʱ����(ms)

unsigned int Timer2_Counter=0; //Timer2��ʱ����������(ms)
unsigned int W5500_Send_Delay_Counter=0; //W5500������ʱ��������(ms)

/*******************************************************************************
* ������  : W5500_Initialization
* ����    : W5500��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		//��ʼ��W5500�Ĵ�������
	Detect_Gateway();	//������ط����� 
	Socket_Init(0);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
	Socket_Init(1);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
	
}

/*******************************************************************************
* ������  : Load_Net_Parameters
* ����    : װ���������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ء����롢�����ַ������IP��ַ���˿ںš�Ŀ��IP��ַ��Ŀ�Ķ˿ںš��˿ڹ���ģʽ
*******************************************************************************/

unsigned char S1_DIPR[4];
unsigned char S1_PORT[2];

void Load_Net_Parameters(void)
{
	uchar	Buffer[24],i;
	uchar	ParameterDatas[27];
	uint16_t tempbuf[24];
	FLASH_Unlock();//����
	MainCode=(*(vu16*) 0x8061000);
	FLASH_Lock();//����	
	  

	RdBytesFromAT24C64(TcpCardDatas_Addr,Buffer,24);
	if (Buffer[0]!=0xa0 || BytesCheckSum(Buffer,24))
	{
//		memcpy(ParameterDatas,DefaultNetDatas,14);
//		memcpy(ParameterDatas+14,DefaultNetDatas,4);
//		memcpy(ParameterDatas+18,DefaultNetDatas+14,8);
		
		for(i=0;i<11;i++)
		{
			tempbuf[i] = FLASH_ReadHalfWord(TcpCardDatasFlashAdd +i*2);
			Buffer[i*2+1] = tempbuf[i]>>8 ;
			Buffer[i*2+2] = tempbuf[i] ;
			memcpy(ParameterDatas,Buffer+1,14);
			memcpy(ParameterDatas+14,Buffer+1,4);
			memcpy(ParameterDatas+18,Buffer+15,8);
			
			//memset(Bufferb,0,1);
		}
	}
	else
	{
		memcpy(ParameterDatas,Buffer+1,14);
		memcpy(ParameterDatas+14,Buffer+1,4);
		memcpy(ParameterDatas+18,Buffer+15,8);
	}
	
	Gateway_IP[0] = ParameterDatas[6];//�������ز���
	Gateway_IP[1] = ParameterDatas[7];
	Gateway_IP[2] = ParameterDatas[8];
	Gateway_IP[3] = ParameterDatas[9];

	Sub_Mask[0]=ParameterDatas[10];//������������
	Sub_Mask[1]=ParameterDatas[11];
	Sub_Mask[2]=ParameterDatas[12];
	Sub_Mask[3]=ParameterDatas[13];

	Phy_Addr[0]=0x0c;//���������ַ
	Phy_Addr[1]=0x29;
	Phy_Addr[2]=0xab;
	Phy_Addr[3]=0x7c;
	Phy_Addr[4]=MainCode>>8;;
	Phy_Addr[5]=MainCode;

	IP_Addr[0]=ParameterDatas[0];//���ر���IP��ַ
	IP_Addr[1]=ParameterDatas[1];
	IP_Addr[2]=ParameterDatas[2];
	IP_Addr[3]=ParameterDatas[3];

	S0_Port[0] = ParameterDatas[5];//���ض˿�0�Ķ˿ں�5000 
	S0_Port[1] = ParameterDatas[4];

//	S0_DIP[0]=ParameterDatas[20];//���ض˿�0��Ŀ��IP��ַ
//	S0_DIP[1]=ParameterDatas[21];
//	S0_DIP[2]=ParameterDatas[22];
//	S0_DIP[3]=ParameterDatas[23];
//	
//	S0_DPort[0] = 0x17;//���ض˿�0��Ŀ�Ķ˿ں�6000
//	S0_DPort[1] = 0x70;

	S1_DIPR[0] = ParameterDatas[20];	//UDP(�㲥)ģʽ,Ŀ������IP��ַ
	S1_DIPR[1] = ParameterDatas[21];
	S1_DIPR[2] = ParameterDatas[22];
	S1_DIPR[3] = ParameterDatas[23];
//
	S1_PORT[0] = ParameterDatas[25];	//UDP(�㲥)ģʽ,Ŀ�������˿ں�
	S1_PORT[1] = ParameterDatas[24];

	S0_Mode=UDP_MODE;//���ض˿�0�Ĺ���ģʽ,UDPģʽ
}

/*******************************************************************************
* ������  : W5500_Socket_Set
* ����    : W5500�˿ڳ�ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : �ֱ�����4���˿�,���ݶ˿ڹ���ģʽ,���˿�����TCP��������TCP�ͻ��˻�UDPģʽ.
*			�Ӷ˿�״̬�ֽ�Socket_State�����ж϶˿ڵĹ������
*******************************************************************************/
void W5500_Socket_Set(void)
{
	if(S0_State==0)//�˿�0��ʼ������
	{
		if(S0_Mode==TCP_SERVER)//TCP������ģʽ 
		{
			if(Socket_Listen(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else if(S0_Mode==TCP_CLIENT)//TCP�ͻ���ģʽ 
		{
			if(Socket_Connect(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else//UDPģʽ 
		{
			if(Socket_UDP(0)==TRUE)
				S0_State=S_INIT|S_CONN;
			else
				S0_State=0;
		}
	}
}

/*******************************************************************************
* ������  : Process_Socket_Data
* ����    : W5500���ղ����ͽ��յ�������
* ����    : s:�˿ں�
* ���    : ��
* ����ֵ  : ��
* ˵��    : �������ȵ���S_rx_process()��W5500�Ķ˿ڽ������ݻ�������ȡ����,
*			Ȼ�󽫶�ȡ�����ݴ�Rx_Buffer������Temp_Buffer���������д���
*			������ϣ������ݴ�Temp_Buffer������Tx_Buffer������������S_tx_process()
*			�������ݡ�
*******************************************************************************/
void Process_Socket_Data(SOCKET s)
{
	unsigned short size,i;
	size=Read_SOCK_Data_Buffer(s, Rx_Buffer);
	UDP_DIPR[0] = Rx_Buffer[0];
	UDP_DIPR[1] = Rx_Buffer[1];
	UDP_DIPR[2] = Rx_Buffer[2];
	UDP_DIPR[3] = Rx_Buffer[3];

	UDP_DPORT[0] = Rx_Buffer[4];
	UDP_DPORT[1] = Rx_Buffer[5];
	//memcpy(SerialUnion.S_DatasBuffer, Rx_Buffer+8, size-8);			
	bitSerial_ReceiveEnd=1;
	SerialReceiveLen = size -8;
	for(i=8;i<(size);i++)
	{
			Udp_Analyse(Rx_Buffer[i]);
	}
}

void Process_Socket2_Data(SOCKET s)
{
	unsigned short size,i;
	size=Read_SOCK_Data_Buffer(s, Rx_Buffer);
	
	//memcpy(SerialUnion.S_DatasBuffer, Rx_Buffer+8, size-8);			
	bitSerial_ReceiveEnd=1;
	SerialReceiveLen = size -8;
	for(i=8;i<(size);i++)
	{
			Udp_Analyse(Rx_Buffer[i]);
	}
}

uchar	TCPCardReceiveDiag(void)//���յ��������ݵĺϷ��Լ�У������
{
	
}
void	UdpAscToHex(uchar * Sum,uchar aa)
{
	uchar	bb;
	bb=aa>>4;
	if (bb<10)
		bb+=0x30;
	else
		bb+=0x37;
	Sum[0]+=bb;
	Tx_Buffer[udp_len++] = bb;

	bb=aa&15;
	if (bb<10)
		bb+=0x30;
	else
		bb+=0x37;
	Sum[0]+=bb;
	
	Tx_Buffer[udp_len++] = bb;
		
}
//��������
void udp_server_send_data()
{ 
	uchar	i,len;
	uint16_t	Sum;
	volatile uchar	CheckSum;
	uchar	SerialSendCheckSum=0;//����У���

	if (!bitSerial_SendRequest)	
		return	;

	bitSerial_SendRequest=0;

	udp_len = 0;

	SerialUnion.S_DatasStruct.UartSeAddrCode=MainCode;//MainCode	
	SerialUnion.S_DatasStruct.UartReAddrCode=0;
	if (SerialUnion.S_DatasStruct.UartStatus)
		SerialUnion.S_DatasStruct.DatasLen=0;  

	Receive_Data_Double ();
	Sum=Cal_CRC_Sub(SerialUnion.S_DatasBuffer,SerialUnion.S_DatasStruct.DatasLen+11);		
	
		Tx_Buffer[udp_len++] = STX;
		for(i=0;i<SerialUnion.S_DatasStruct.DatasLen+11;i++)
		{
			UdpAscToHex(&CheckSum,SerialUnion.S_DatasBuffer[i]);	
		}
		Tx_Buffer[udp_len++] = ETX;
		UdpAscToHex(&CheckSum,Sum/256);
		UdpAscToHex(&CheckSum,Sum%256);
		Write_SOCK_Data_Buffer(0, Tx_Buffer, udp_len);//ָ��Socke
		Init_Serial();
}

void socket2senddata(unsigned char *dat_ptr, unsigned short size);
//��������
void udp_server_send_data_socket1()
{ 
	uchar	i,len;
	uint16_t	Sum;
	volatile uchar	CheckSum;
	uchar	SerialSendCheckSum=0;//����У���

	if (!bitSerial_SendRequest)	
		return	;

	bitSerial_SendRequest=0;

	udp_len = 0;

	SerialUnion.S_DatasStruct.UartSeAddrCode=MainCode;//MainCode	
	SerialUnion.S_DatasStruct.UartReAddrCode=0;
	if (SerialUnion.S_DatasStruct.UartStatus)
		SerialUnion.S_DatasStruct.DatasLen=0;  

	Receive_Data_Double ();
	Sum=Cal_CRC_Sub(SerialUnion.S_DatasBuffer,SerialUnion.S_DatasStruct.DatasLen+11);		
	
		Tx_Buffer[udp_len++] = STX;
		for(i=0;i<SerialUnion.S_DatasStruct.DatasLen+11;i++)
		{
			UdpAscToHex(&CheckSum,SerialUnion.S_DatasBuffer[i]);	
		}
		Tx_Buffer[udp_len++] = ETX;
		UdpAscToHex(&CheckSum,Sum/256);
		UdpAscToHex(&CheckSum,Sum%256);
		socket2senddata( Tx_Buffer, udp_len);//ָ��Socke
		Init_Serial();
}

void Receive_Data_Double (void)
{
	SerialUnion.S_DatasStruct.UartReAddrCode = DoubleBigToSmall(SerialUnion.S_DatasStruct.UartReAddrCode);
	SerialUnion.S_DatasStruct.UartComd = DoubleBigToSmall(SerialUnion.S_DatasStruct.UartComd);
	SerialUnion.S_DatasStruct.UartSeAddrCode = DoubleBigToSmall(SerialUnion.S_DatasStruct.UartSeAddrCode);
}
//ͨѶ���ݽ���
//st_data--��Ҫ����������
u8	Udp_Analyse(u8 st_data)
{
	u8	j;
    if (!SerialReCase)
    {
        if (st_data==STX)
        {
            SerialReCase=1;
            bitEven=0;
            SerialReceiveLen=0;			
        }
    }
    else if (SerialReCase==1)
    {
        if (st_data<0x3a && st_data>0x2f)
            j=st_data-0x30;
        else if (st_data>0x40 && st_data<0x47)
            j=st_data-0x37;
        else if (st_data==ETX)
        {
            SerialReCase=2;
            bitEven=0;	
            SerialReCheckSumCount=0;				
        }
        else
        {
            SerialReCase=0;
            return	0xff;
        }
        
        if (SerialReCase==1)
        {
            if (!bitEven)
            {
                rChar=j<<4;
                bitEven=1;
            }
            else
            {
                bitEven=0;
                rChar+=j;	
                SerialUnion.S_DatasBuffer[SerialReceiveLen++]=rChar;
                if (SerialReceiveLen==2)
                {
                    if (DoubleBigToSmall(SerialUnion.S_DatasStruct.UartSeAddrCode))
                    {
                        SerialReCase=0;
                        return	0xff;
                    } 
                } 
                else if (SerialReceiveLen==6)
                {
					
                   if (DoubleBigToSmall(SerialUnion.S_DatasStruct.UartReAddrCode)!=MainCode &&	DoubleBigToSmall(SerialUnion.S_DatasStruct.UartComd)!=RD_ADDR_COMD)
                    {
                        SerialReCase=0;
                        return	0xff;
                    }  
                } 
            }
        }		

    } //
    else if (SerialReCase==2)
    {
        if (st_data<0x3a && st_data>0x2f)
            j=st_data-0x30;
        else if (st_data>0x40 && st_data<0x47)
            j=st_data-0x37;
        else
        {
            SerialReCase=0;
            return	0xff;
        }
        if (!bitEven)
        {
            rChar=j<<4;
            bitEven=1;
        }
        else
        {
            bitEven=0;
            rChar+=j;
            * (SerialReCheckSumCount+(uchar *)&SerialReCheckSum)=rChar;	
			
            SerialReCheckSumCount++;
            if (SerialReCheckSumCount==2)
            {//CRCУ��ͱȽ�
								SerialReCheckSum = DoubleBigToSmall(SerialReCheckSum);
                bitSerial_ReceiveEnd=1;
            }				 
        }
    }
    return  0;
}