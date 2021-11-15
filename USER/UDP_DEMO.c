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

unsigned int udp_len;			//udp发送数据长度
void RCC_Configuration(void);		//设置系统时钟为72MHZ(这个可以根据需要改)
void NVIC_Configuration(void);		//STM32中断向量表配配置
void Timer2_Init_Config(void);		//Timer2初始化配置
void System_Initialization(void);	//STM32系统初始化函数(初始化STM32时钟及外设)
void Delay(unsigned int d);			//延时函数(ms)

unsigned int Timer2_Counter=0; //Timer2定时器计数变量(ms)
unsigned int W5500_Send_Delay_Counter=0; //W5500发送延时计数变量(ms)

/*******************************************************************************
* 函数名  : W5500_Initialization
* 描述    : W5500初始货配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		//初始化W5500寄存器函数
	Detect_Gateway();	//检查网关服务器 
	Socket_Init(0);		//指定Socket(0~7)初始化,初始化端口0
	Socket_Init(1);		//指定Socket(0~7)初始化,初始化端口0
	
}

/*******************************************************************************
* 函数名  : Load_Net_Parameters
* 描述    : 装载网络参数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 网关、掩码、物理地址、本机IP地址、端口号、目的IP地址、目的端口号、端口工作模式
*******************************************************************************/

unsigned char S1_DIPR[4];
unsigned char S1_PORT[2];

void Load_Net_Parameters(void)
{
	uchar	Buffer[24],i;
	uchar	ParameterDatas[27];
	uint16_t tempbuf[24];
	FLASH_Unlock();//解锁
	MainCode=(*(vu16*) 0x8061000);
	FLASH_Lock();//锁定	
	  

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
	
	Gateway_IP[0] = ParameterDatas[6];//加载网关参数
	Gateway_IP[1] = ParameterDatas[7];
	Gateway_IP[2] = ParameterDatas[8];
	Gateway_IP[3] = ParameterDatas[9];

	Sub_Mask[0]=ParameterDatas[10];//加载子网掩码
	Sub_Mask[1]=ParameterDatas[11];
	Sub_Mask[2]=ParameterDatas[12];
	Sub_Mask[3]=ParameterDatas[13];

	Phy_Addr[0]=0x0c;//加载物理地址
	Phy_Addr[1]=0x29;
	Phy_Addr[2]=0xab;
	Phy_Addr[3]=0x7c;
	Phy_Addr[4]=MainCode>>8;;
	Phy_Addr[5]=MainCode;

	IP_Addr[0]=ParameterDatas[0];//加载本机IP地址
	IP_Addr[1]=ParameterDatas[1];
	IP_Addr[2]=ParameterDatas[2];
	IP_Addr[3]=ParameterDatas[3];

	S0_Port[0] = ParameterDatas[5];//加载端口0的端口号5000 
	S0_Port[1] = ParameterDatas[4];

//	S0_DIP[0]=ParameterDatas[20];//加载端口0的目的IP地址
//	S0_DIP[1]=ParameterDatas[21];
//	S0_DIP[2]=ParameterDatas[22];
//	S0_DIP[3]=ParameterDatas[23];
//	
//	S0_DPort[0] = 0x17;//加载端口0的目的端口号6000
//	S0_DPort[1] = 0x70;

	S1_DIPR[0] = ParameterDatas[20];	//UDP(广播)模式,目的主机IP地址
	S1_DIPR[1] = ParameterDatas[21];
	S1_DIPR[2] = ParameterDatas[22];
	S1_DIPR[3] = ParameterDatas[23];
//
	S1_PORT[0] = ParameterDatas[25];	//UDP(广播)模式,目的主机端口号
	S1_PORT[1] = ParameterDatas[24];

	S0_Mode=UDP_MODE;//加载端口0的工作模式,UDP模式
}

/*******************************************************************************
* 函数名  : W5500_Socket_Set
* 描述    : W5500端口初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 分别设置4个端口,根据端口工作模式,将端口置于TCP服务器、TCP客户端或UDP模式.
*			从端口状态字节Socket_State可以判断端口的工作情况
*******************************************************************************/
void W5500_Socket_Set(void)
{
	if(S0_State==0)//端口0初始化配置
	{
		if(S0_Mode==TCP_SERVER)//TCP服务器模式 
		{
			if(Socket_Listen(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else if(S0_Mode==TCP_CLIENT)//TCP客户端模式 
		{
			if(Socket_Connect(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else//UDP模式 
		{
			if(Socket_UDP(0)==TRUE)
				S0_State=S_INIT|S_CONN;
			else
				S0_State=0;
		}
	}
}

/*******************************************************************************
* 函数名  : Process_Socket_Data
* 描述    : W5500接收并发送接收到的数据
* 输入    : s:端口号
* 输出    : 无
* 返回值  : 无
* 说明    : 本过程先调用S_rx_process()从W5500的端口接收数据缓冲区读取数据,
*			然后将读取的数据从Rx_Buffer拷贝到Temp_Buffer缓冲区进行处理。
*			处理完毕，将数据从Temp_Buffer拷贝到Tx_Buffer缓冲区。调用S_tx_process()
*			发送数据。
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

uchar	TCPCardReceiveDiag(void)//接收到网卡数据的合法性及校验和诊断
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
//发送数据
void udp_server_send_data()
{ 
	uchar	i,len;
	uint16_t	Sum;
	volatile uchar	CheckSum;
	uchar	SerialSendCheckSum=0;//发送校验和

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
		Write_SOCK_Data_Buffer(0, Tx_Buffer, udp_len);//指定Socke
		Init_Serial();
}

void socket2senddata(unsigned char *dat_ptr, unsigned short size);
//发送数据
void udp_server_send_data_socket1()
{ 
	uchar	i,len;
	uint16_t	Sum;
	volatile uchar	CheckSum;
	uchar	SerialSendCheckSum=0;//发送校验和

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
		socket2senddata( Tx_Buffer, udp_len);//指定Socke
		Init_Serial();
}

void Receive_Data_Double (void)
{
	SerialUnion.S_DatasStruct.UartReAddrCode = DoubleBigToSmall(SerialUnion.S_DatasStruct.UartReAddrCode);
	SerialUnion.S_DatasStruct.UartComd = DoubleBigToSmall(SerialUnion.S_DatasStruct.UartComd);
	SerialUnion.S_DatasStruct.UartSeAddrCode = DoubleBigToSmall(SerialUnion.S_DatasStruct.UartSeAddrCode);
}
//通讯数据解析
//st_data--需要解析的数据
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
            {//CRC校验和比较
								SerialReCheckSum = DoubleBigToSmall(SerialReCheckSum);
                bitSerial_ReceiveEnd=1;
            }				 
        }
    }
    return  0;
}