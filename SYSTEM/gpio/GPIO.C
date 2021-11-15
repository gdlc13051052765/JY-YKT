#include "stm32f10x_gpio.h"
#include "GPIO.H"
/**************************************************************************************
**************************************************************************************/
 #define RCC_APB2Periph_ALLGPIO             (RCC_APB2Periph_GPIOA \
                                              | RCC_APB2Periph_GPIOB \
                                              | RCC_APB2Periph_GPIOC \
                                              | RCC_APB2Periph_GPIOD \
                                              | RCC_APB2Periph_GPIOE )

/**************************************************************************************
* ��    ��:    DK_GPIO_Init(u8 GPIOx,u16 GPIOx_x,u8 Type)  
* ��    ��:    ��ʼ��GPIO�˿�
* ��    ��:    GPIOxΪPA,PB,PC,PD,PE,PF
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
void DK_GPIO_Init(unsigned char GPIOx,unsigned short GPIOx_x,unsigned char Type)  //�������Ͳ��� "GPIO.h"
{			  
	GPIO_InitTypeDef   GPIO_InitStructure;	  //���ݽṹ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//���ùܽ����ʣ����50M
  GPIO_InitStructure.GPIO_Pin = GPIOx_x;			//ѡ��ܽ�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //���ùܽ�	
  switch(Type)
	{
	  case 1:
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//�ܽ�״̬Ϊģ������
		break;
		case 2:
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//�ܽ�״̬Ϊ��������
		break;
		case 3:
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//�ܽ�״̬Ϊ��������
		break;
		case 4:
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//�ܽ�״̬Ϊ��������
		break;
		case 5:
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		//�ܽ�״̬Ϊ��©���
		break;
		case 6:
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//�ܽ�״̬Ϊ�������
		break;
		case 7:
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;		//�ܽ�״̬Ϊ���ÿ�©���
		break;
		case 8:
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�ܽ�״̬Ϊ�����������
		break;
	}
	switch(GPIOx)
	{
		case 1:	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��GPIOAʱ��
		GPIO_Init(GPIOA, &GPIO_InitStructure);				//���ú�����ʼ���ܽ�
		break;
		case 2:	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��GPIOBʱ��
		GPIO_Init(GPIOB, &GPIO_InitStructure);				//���ú�����ʼ���ܽ�
		break;
		case 3:	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��GPIOCʱ��
		GPIO_Init(GPIOC, &GPIO_InitStructure);				//���ú�����ʼ���ܽ�
		break;
		case 4:	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��GPIODʱ��
		GPIO_Init(GPIOD, &GPIO_InitStructure);				//���ú�����ʼ���ܽ�
		break;
		case 5:	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��GPIOEʱ��
		GPIO_Init(GPIOE, &GPIO_InitStructure);				//���ú�����ʼ���ܽ�
		break;
		case 6:	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //ʹ��GPIOFʱ��
		GPIO_Init(GPIOF, &GPIO_InitStructure);				//���ú�����ʼ���ܽ�
		break;
		case 7:	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //ʹ��GPIOGʱ��
		GPIO_Init(GPIOG, &GPIO_InitStructure);				//���ú�����ʼ���ܽ�
		break;
	}
}
 /**************************************************************************************
* ��    ��:    DK_GPIO_Write_H()
* ��    ��:    ��ָ��GPIO�ڵĸ߰�λд������
* ��    ��:    GPIOxָ���˿ڣ�	PortValҪд�������
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
void DK_GPIO_Write_H(GPIO_TypeDef* GPIOx, unsigned char PortVal)
{
  unsigned short ReadValue;
	unsigned short WriteValue; 
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	ReadValue = GPIO_ReadOutputData(GPIOx);
	ReadValue = ReadValue&0x00ff;
	WriteValue=PortVal;
	WriteValue=WriteValue<<0x08;
	WriteValue=WriteValue|ReadValue;
	GPIOx->ODR = WriteValue;
}
/**************************************************************************************
* ��    ��:    DK_GPIO_Write_L()
* ��    ��:    ��ָ��GPIO�ڵĵͰ�λд������
* ��    ��:    GPIOxָ���˿ڣ�	PortValҪд�������
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
void DK_GPIO_Write_L(GPIO_TypeDef* GPIOx, unsigned char PortVal)
{
  unsigned short ReadValue;
	unsigned short WriteValue; 
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	ReadValue = GPIO_ReadOutputData(GPIOx);
	ReadValue = ReadValue&0xFF00;
	WriteValue=PortVal;
	WriteValue=WriteValue|ReadValue;
	GPIOx->ODR = WriteValue;
}
/*******************************************************************************
* Function Name  : DK_GPIO_AINConfig
* Description    : Configures all IOs as AIN to reduce the power consumption.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void DK_GPIO_AINConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable all GPIOs Clock*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ALLGPIO, ENABLE);

  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);


#if defined (USE_STM3210E_EVAL) || defined (USE_STM3210C_EVAL)
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_Init(GPIOG, &GPIO_InitStructure);
#endif /* USE_STM3210E_EVAL */

  /* Disable all GPIOs Clock*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ALLGPIO, DISABLE);
}