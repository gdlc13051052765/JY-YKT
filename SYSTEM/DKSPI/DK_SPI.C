#include "stm32f10x_spi.h"	
#include "GPIO.H"
#include "DK_SPI.H"  

/**************************************************************************************
* ��    ��:    DK_GPIO_Init(u8 GPIOx,u16 GPIOx_x,u8 Type)  
* ��    ��:    
* ��    ��:    
* �� �� ֵ:    ��
* ��    �ߣ�   D&K
**************************************************************************************/ 
void DK_SPI_Init(u8 SPIx)
{				
	SPI_InitTypeDef   SPI_InitStructure;
	if(SPIx==1)
	{														
 		SPI_Cmd(SPI1, DISABLE);            		//�����Ƚ���,���ܸı�MODE
 		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);	  
		DK_GPIO_Init(PA,GPIO_Pin_4,Out_PP);    // SS
		DK_GPIO_Init(PA,GPIO_Pin_5,AF_PP);	    // SCK	 �������
		DK_GPIO_Init(PA,GPIO_Pin_6,AF_PP);		// MISO		����
		DK_GPIO_Init(PA,GPIO_Pin_7,AF_PP);  	// MOSI   �������		   

	}
	if(SPIx==2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
		DK_GPIO_Init(PB,GPIO_Pin_12,Out_PP);    // SS
		DK_GPIO_Init(PB,GPIO_Pin_13,AF_PP);	    // SCK	 �������
		DK_GPIO_Init(PB,GPIO_Pin_14,AF_PP);		// MISO		����
		DK_GPIO_Init(PB,GPIO_Pin_15,AF_PP);  	// MOSI   �������	
	}
	if(SPIx==3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);

	}
  	SPI_InitStructure.SPI_Direction =SPI_Direction_2Lines_FullDuplex;  //����ȫ˫��
   	SPI_InitStructure.SPI_Mode =SPI_Mode_Master;          	//��
   	SPI_InitStructure.SPI_DataSize =SPI_DataSize_8b;      	//8λ
   	SPI_InitStructure.SPI_CPOL =SPI_CPOL_High;        		//CPOL=1ʱ�����ո�
   	SPI_InitStructure.SPI_CPHA =SPI_CPHA_2Edge;       		//CPHA=1 ���ݲ����2��
   	SPI_InitStructure.SPI_NSS =SPI_NSS_Soft;        		// NSS
    SPI_InitStructure.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_8;  //256��Ƶ ���ǵ���
    SPI_InitStructure.SPI_FirstBit =SPI_FirstBit_MSB;       //��λ��ǰ
    SPI_InitStructure.SPI_CRCPolynomial =7;        			//CRC7
   
	if(SPIx==1)
	{
  		SPI_Init(SPI1,&SPI_InitStructure);	 
		SPI_Cmd(SPI1, ENABLE); 
	}
	if(SPIx==2)	
	{
  		SPI_Init(SPI2,&SPI_InitStructure);	
		SPI_Cmd(SPI2, ENABLE); 
	}
	if(SPIx==3)	
	{
  		SPI_Init(SPI3,&SPI_InitStructure); 
		SPI_Cmd(SPI3, ENABLE); 
	}

//spi�����ý����˿���ʹ���ˡ�
}
/*******************************************************************************
*�������ܣ�	SPI1����һ������
*����������
***********************************************************************************/
void DK_SPI1_Send_Byte(u8 SPIx,u8 Data)
{	 
	if(1==SPIx)	
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	  //ȷ������ǰBufferΪ�գ�Ҳ����˵��һ���Ѿ��������   
	    SPI_I2S_SendData(SPI1, Data);    
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	    SPI_I2S_ReceiveData(SPI1);   //���Ĵ�����Ӳ�������־λ��
	}
	if(2==SPIx)	
	{
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);	  //ȷ������ǰBufferΪ�գ�Ҳ����˵��һ���Ѿ��������   
	    SPI_I2S_SendData(SPI2, Data);    
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	    SPI_I2S_ReceiveData(SPI2);   //���Ĵ�����Ӳ�������־λ��
	}
	if(3==SPIx)	
	{
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);	  //ȷ������ǰBufferΪ�գ�Ҳ����˵��һ���Ѿ��������   
	    SPI_I2S_SendData(SPI3, Data);    
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
	    SPI_I2S_ReceiveData(SPI3);   //���Ĵ�����Ӳ�������־λ��
	}
}
/*******************************************************************************
*�������ܣ�	SPI1����һ������
*����������
*******************************************************************************/
u8 DK_SPI1_Recieve_Byte(u8 SPIx)
{     
	u8 temp,Data=0xff;

	if(1==SPIx)	
	{
	    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	    SPI_I2S_SendData(SPI1, Data); 
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	 	temp = SPI_I2S_ReceiveData(SPI1);  
	}
	if(2==SPIx)	
	{
	    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	    SPI_I2S_SendData(SPI2, Data); 
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	 	temp = SPI_I2S_ReceiveData(SPI2);  
	}
	if(3==SPIx)	
	{
	    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
	    SPI_I2S_SendData(SPI3, Data); 
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
	 	temp = SPI_I2S_ReceiveData(SPI3);  
	}
	return temp;
}
/*******************************************************************************
*�������ܣ�	SPI1����һ�����ݲ�����һ������
*����������
*******************************************************************************/
u8 DK_SPI1_S_R_Byte(u8 SPIx,u8 byte)
{     

	if(1==SPIx)	
	{
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); //SPI1 �ȴ���������
  SPI_I2S_SendData(SPI1, byte); //SPI1 ��������
	
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//SPI1 �ȴ���������
  return SPI_I2S_ReceiveData(SPI1); //��SPI1 �������� ���ز���
	}
	if(2==SPIx)	
	{
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); //SPI1 �ȴ���������
  SPI_I2S_SendData(SPI2, byte); //SPI1 ��������
	
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);//SPI1 �ȴ���������
  return SPI_I2S_ReceiveData(SPI2); //��SPI1 �������� ���ز���
	}

	if(3==SPIx)	
	{
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET); //SPI1 �ȴ���������
  SPI_I2S_SendData(SPI3, byte); //SPI1 ��������
	
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);//SPI1 �ȴ���������
  return SPI_I2S_ReceiveData(SPI3); //��SPI1 �������� ���ز���
	}
	 return 0;
	
}

/*******************************************************************************
*�������ܣ�	����SPI�ٶȴ�С��SpeedSet����Ϊ0��ʾ����ģʽ������Ϊ1��ʾ����ģʽ
*����������
*******************************************************************************/
void DK_SPI_SetSpeed(u8 SPIx,u8 SpeedSet)
{
    SPI_InitTypeDef SPI_InitStructure;

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    //����ٶ���������0�������ģʽ����0�����ģʽ
    if(SpeedSet==0)
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    }
    else
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    }
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	if(1==SPIx)	SPI_Init(SPI1, &SPI_InitStructure);
	else if(2==SPIx)	SPI_Init(SPI2, &SPI_InitStructure);
	else if(3==SPIx)	SPI_Init(SPI3, &SPI_InitStructure);
}
