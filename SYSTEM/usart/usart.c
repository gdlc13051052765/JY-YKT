#include "sys.h"
#include "usart.h"	
#include "MyDefine.h"
#include "ExternVariableDef.h"
#include "ExternVoidDef.h"
#include "serialfiles.h"
#include "char.h"
#include "485.h"  
#include "gpio.h"
#include "includes.h"	
void	Init_Serial(void);
void	SerialSendChar (uchar );

#define wantTempCnt 40

u8 wantRecData[wantTempCnt];
u8 wantRecDatatemp[wantTempCnt];
u8 wantRecDataLen = 0;
u8 isHaveRec = 0;

u8 IsInCmdAT = 0;
//********************************************************************************

//1	  
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.  

void DK_USART_Init(unsigned long Baud,unsigned char USARTx)
{								 
	USART_InitTypeDef    USART_InitStructure; 
	USART_InitStructure.USART_BaudRate =Baud;  //bps,如9600  19200
	#ifndef _USART_Use
	#define _USART_Use
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8位数据
		USART_InitStructure.USART_StopBits = USART_StopBits_1;	  //1位停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;	//	无奇偶校验
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	   //无硬件控制流
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	 //发送接收使能
	#endif
	if(USARTx==1)
	{											  
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
		DK_GPIO_Init(PA,GPIO_Pin_9,AF_PP);	//复用推挽输出  TX
		DK_GPIO_Init(PA,GPIO_Pin_10,IN_FLOATING);	//  RX
	  USART_Init(USART1, &USART_InitStructure);
		USART_Cmd(USART1, ENABLE);	  
	}
	if(USARTx==2)
	{
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
		DK_GPIO_Init(PA,GPIO_Pin_2,AF_PP);	//复用推挽输出  TX
		DK_GPIO_Init(PA,GPIO_Pin_3,IN_FLOATING);	//  RX
		USART_Init(USART2, &USART_InitStructure);
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);//清楚中断位
		USART_Cmd(USART2, ENABLE);	  
	}
	if(USARTx==3)
	{
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
		//GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);  //端口管脚重映射
		DK_GPIO_Init(PB,GPIO_Pin_10,AF_PP);	//复用推挽输出  TX
		DK_GPIO_Init(PB,GPIO_Pin_11,IPU);	//  RX
		USART_Init(USART3, &USART_InitStructure);
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);//清楚中断位
		USART_Cmd(USART3, ENABLE);					  
	}
 	
}
/*******************************************************************************
*函数功能：打开中断
*函数描述：
***********************************************************************************/
void DK_USART_OPen_INT(unsigned char USARTx)
{
	switch(USARTx)
	{
	 	case 1:	
			USART_ClearITPendingBit(USART1,USART_IT_RXNE);
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		break;
		
		case 2:	
			USART_ClearITPendingBit(USART2,USART_IT_RXNE);
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		break;

		case 3:	
			USART_ClearITPendingBit(USART3,USART_IT_RXNE);
			USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
		break;
	} 
}
/*******************************************************************************
*函数功能：关闭中断
*函数描述：
***********************************************************************************/
void DK_USART_Close_INT(unsigned char USARTx)
{
	switch(USARTx)
	{
	 	case 1:	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
		break;
		
		case 2:	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
		break;

		case 3:	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
		break;
	} 
}
/*******************************************************************************
*函数功能：查询法发送一个字节
*函数描述：
***********************************************************************************/
void USART_Send_Char(unsigned char USARTx,unsigned char data)
{
	switch(USARTx)
	{
	 	case 1:	USART_SendData(USART1,data); 
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);     //等待发送完毕
		break;
		
		case 2:	USART_SendData(USART2,data);  
				while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		break;

		case 3:	USART_SendData(USART3,data);  
				while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		break;
	} 
}
/*******************************************************************************
*函数功能：USART输出一个字符串数据
*函数描述：如果buffer指向一个数组，则数组的最后一位必须为0
*******************************************************************************/
void USART_PUTS_Arry(unsigned char USARTx,unsigned char *buffer,unsigned int len)
{			   
    while(len--)
	{
	    USART_Send_Char(USARTx,*buffer);
		buffer++;
	}	   
} 
//串口1中断服务程序
void USART1_IRQHandler(void)                	
{
	volatile unsigned char  st_data;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		st_data = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		  		 
     } 
}

//通讯数据解析
//st_data--需要解析的数据
uchar	SerAPDU_Analyse(uchar st_data)
{
	uchar	j;
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
								
							//UART_ReceiveSub();

						//	UART_SendSub();	
            }				 
        }
    }
    return  0;
}

//串口3中断服务程序
void USART3_IRQHandler(void)                	
{
	u8 st_data;
	u8 i;
//IsInCmdAT=1;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		st_data = USART_ReceiveData(USART3);//(USART1->DR);	//读取接收到的数据
			if (bitSerial_ReceiveEnd)
			return;	
			if(IsInCmdAT == 1)
		{
			for(i=0;i<wantTempCnt-1;i++)
			{
				wantRecData[i] = wantRecData[i+1];
			}
			
			wantRecData[wantTempCnt-1] = st_data;
			
			for(i = wantTempCnt - wantRecDataLen;i<wantTempCnt;i++)
			{
				if(wantRecData[i] !=wantRecDatatemp[i] )
				{
					break;
				}
			}
			
			if(i == wantTempCnt)
			{
				isHaveRec = 1;
			}
			
		}
		else
			{
				SerAPDU_Analyse(st_data); //串口数据分析		
			}				 
	 }
}
u8 sendATToWifi(u8 * cmd,u8 * whantRec,u8 wantlen)
{
	int i;
	int j;
	IsInCmdAT = 1;
	while(*cmd != 0)
	{
		USART_SendData(USART3,*cmd);  
		cmd++;
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	}
	//USART_SendData(USART3,'\n');  
	
	j = wantTempCnt - wantlen;
	
	for(i=0;i<wantlen;i++)
	{
		wantRecDatatemp[j+i] = whantRec[i];
		
	}
	
	wantRecDataLen = wantlen;
	isHaveRec = 0;
	
	i=0;
	
	while(isHaveRec == 0)
	{
		Clr_WatchDog();
		delay_ms(10);
		i++;
		if(i >= 1000)
		{
			IsInCmdAT = 0;
			return 0xFF;
		}
	}
	IsInCmdAT = 0;
	return 0x00;;
	
}


void	SerialSendChar (uchar aa)
{
	u485_TE;
//	delay_ms(20);
	USART_Send_Char(3,aa);
//	delay_ms(20);	
}
void	Init_Serial(void)
{
	SerialReceiveLen=0;
	bitSerial_ReceiveEnd=0;
	//NetFlag = 1;
	SerialReCase=0;
	SerialReceiveTimeCount=0;
	memset(SerialUnion.S_DatasBuffer,0,100);
	u485_RE;//485通讯接口转换为接收方式
} 

#endif 
