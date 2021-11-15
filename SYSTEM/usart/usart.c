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
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.  

void DK_USART_Init(unsigned long Baud,unsigned char USARTx)
{								 
	USART_InitTypeDef    USART_InitStructure; 
	USART_InitStructure.USART_BaudRate =Baud;  //bps,��9600  19200
	#ifndef _USART_Use
	#define _USART_Use
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8λ����
		USART_InitStructure.USART_StopBits = USART_StopBits_1;	  //1λֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;	//	����żУ��
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	   //��Ӳ��������
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	 //���ͽ���ʹ��
	#endif
	if(USARTx==1)
	{											  
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
		DK_GPIO_Init(PA,GPIO_Pin_9,AF_PP);	//�����������  TX
		DK_GPIO_Init(PA,GPIO_Pin_10,IN_FLOATING);	//  RX
	  USART_Init(USART1, &USART_InitStructure);
		USART_Cmd(USART1, ENABLE);	  
	}
	if(USARTx==2)
	{
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
		DK_GPIO_Init(PA,GPIO_Pin_2,AF_PP);	//�����������  TX
		DK_GPIO_Init(PA,GPIO_Pin_3,IN_FLOATING);	//  RX
		USART_Init(USART2, &USART_InitStructure);
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);//����ж�λ
		USART_Cmd(USART2, ENABLE);	  
	}
	if(USARTx==3)
	{
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
		//GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);  //�˿ڹܽ���ӳ��
		DK_GPIO_Init(PB,GPIO_Pin_10,AF_PP);	//�����������  TX
		DK_GPIO_Init(PB,GPIO_Pin_11,IPU);	//  RX
		USART_Init(USART3, &USART_InitStructure);
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);//����ж�λ
		USART_Cmd(USART3, ENABLE);					  
	}
 	
}
/*******************************************************************************
*�������ܣ����ж�
*����������
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
*�������ܣ��ر��ж�
*����������
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
*�������ܣ���ѯ������һ���ֽ�
*����������
***********************************************************************************/
void USART_Send_Char(unsigned char USARTx,unsigned char data)
{
	switch(USARTx)
	{
	 	case 1:	USART_SendData(USART1,data); 
				while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);     //�ȴ��������
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
*�������ܣ�USART���һ���ַ�������
*�������������bufferָ��һ�����飬����������һλ����Ϊ0
*******************************************************************************/
void USART_PUTS_Arry(unsigned char USARTx,unsigned char *buffer,unsigned int len)
{			   
    while(len--)
	{
	    USART_Send_Char(USARTx,*buffer);
		buffer++;
	}	   
} 
//����1�жϷ������
void USART1_IRQHandler(void)                	
{
	volatile unsigned char  st_data;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		st_data = USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		  		 
     } 
}

//ͨѶ���ݽ���
//st_data--��Ҫ����������
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
            {//CRCУ��ͱȽ�
								SerialReCheckSum = DoubleBigToSmall(SerialReCheckSum);
                bitSerial_ReceiveEnd=1;
								
							//UART_ReceiveSub();

						//	UART_SendSub();	
            }				 
        }
    }
    return  0;
}

//����3�жϷ������
void USART3_IRQHandler(void)                	
{
	u8 st_data;
	u8 i;
//IsInCmdAT=1;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		st_data = USART_ReceiveData(USART3);//(USART1->DR);	//��ȡ���յ�������
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
				SerAPDU_Analyse(st_data); //�������ݷ���		
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
	u485_RE;//485ͨѶ�ӿ�ת��Ϊ���շ�ʽ
} 

#endif 
