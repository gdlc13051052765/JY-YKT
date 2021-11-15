#include "stm32f10x.h"
#include "includes.h"

#define ValueVoice	1  //������С  ���30

void InitBY8001(void)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART1��GPIOAʱ���Լ����ù���ʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	USART_InitStructure.USART_BaudRate = 9600;						//����9600bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
	USART_InitStructure.USART_Parity =USART_Parity_No;//USART_Parity_Odd;				//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART2, ENABLE);        
}


//����һ���ַ�
void SendUsart2(u8 data)
{
	USART2->DR = data;      
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
}

extern u8 VoiceVal;

//����������С
void SetValue(void)
{
	
	u8 checksum = 0;
	VoiceVal = 30;
	SendUsart2(0x7E);
	SendUsart2(0x04);
	checksum = 0x04;
	SendUsart2(0x31);
	checksum ^= 0x31;
	SendUsart2(VoiceVal);
	checksum ^= VoiceVal;
	SendUsart2(checksum);
	SendUsart2(0xEF);
}

//���õ���ѭ��
void SetPlayOne(void)
{
	u8 checksum = 0;
	SendUsart2(0x7E);
	SendUsart2(0x04);
	checksum = 0x04;
	SendUsart2(0x33);
	checksum ^= 0x33;
	SendUsart2(0x02);
	checksum ^= 0x02;
	SendUsart2(checksum);
	SendUsart2(0xEF);
}

//������Ŀ
void SetPlay(u8 num)
{
	u8 checksum = 0;
	SendUsart2(0x7E);
	SendUsart2(0x05);
	checksum = 0x05;
	SendUsart2(0x41);
	checksum ^= 0x41;
	SendUsart2(0x00);
	checksum ^= 0x00;
	SendUsart2(num);
	checksum ^= num;
	SendUsart2(checksum);
	SendUsart2(0xEF);
}

//������ͣ
void SetPlayStop()
{
	SendUsart2(0x7E);
	SendUsart2(0x03);
	SendUsart2(0x0E);
	SendUsart2(0x0D);
	SendUsart2(0xEF);
}

//void delay_ms(uint32_t i);
void PlayNum(u8 num)
{
	//SetPlayStop();
	//SetPlay(0);
	//delay_ms(50);
//	SetValue();
//	delay_ms(50);
	SetPlayStop();
	delay_ms(50);
	//if(!PDin(9))
		SetPlay(num);
	//delay_ms(50);
	//SetValue();
}
