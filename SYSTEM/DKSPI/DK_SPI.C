#include "stm32f10x_spi.h"	
#include "GPIO.H"
#include "DK_SPI.H"  

/**************************************************************************************
* 名    称:    DK_GPIO_Init(u8 GPIOx,u16 GPIOx_x,u8 Type)  
* 功    能:    
* 参    数:    
* 返 回 值:    无
* 作    者：   D&K
**************************************************************************************/ 
void DK_SPI_Init(u8 SPIx)
{				
	SPI_InitTypeDef   SPI_InitStructure;
	if(SPIx==1)
	{														
 		SPI_Cmd(SPI1, DISABLE);            		//必须先禁能,才能改变MODE
 		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);	  
		DK_GPIO_Init(PA,GPIO_Pin_4,Out_PP);    // SS
		DK_GPIO_Init(PA,GPIO_Pin_5,AF_PP);	    // SCK	 复用输出
		DK_GPIO_Init(PA,GPIO_Pin_6,AF_PP);		// MISO		输入
		DK_GPIO_Init(PA,GPIO_Pin_7,AF_PP);  	// MOSI   复用输出		   

	}
	if(SPIx==2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
		DK_GPIO_Init(PB,GPIO_Pin_12,Out_PP);    // SS
		DK_GPIO_Init(PB,GPIO_Pin_13,AF_PP);	    // SCK	 复用输出
		DK_GPIO_Init(PB,GPIO_Pin_14,AF_PP);		// MISO		输入
		DK_GPIO_Init(PB,GPIO_Pin_15,AF_PP);  	// MOSI   复用输出	
	}
	if(SPIx==3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);

	}
  	SPI_InitStructure.SPI_Direction =SPI_Direction_2Lines_FullDuplex;  //两线全双工
   	SPI_InitStructure.SPI_Mode =SPI_Mode_Master;          	//主
   	SPI_InitStructure.SPI_DataSize =SPI_DataSize_8b;      	//8位
   	SPI_InitStructure.SPI_CPOL =SPI_CPOL_High;        		//CPOL=1时钟悬空高
   	SPI_InitStructure.SPI_CPHA =SPI_CPHA_2Edge;       		//CPHA=1 数据捕获第2个
   	SPI_InitStructure.SPI_NSS =SPI_NSS_Soft;        		// NSS
    SPI_InitStructure.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_8;  //256分频 先是低速
    SPI_InitStructure.SPI_FirstBit =SPI_FirstBit_MSB;       //高位在前
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

//spi的配置结束了可以使用了。
}
/*******************************************************************************
*函数功能：	SPI1发送一个数据
*函数描述：
***********************************************************************************/
void DK_SPI1_Send_Byte(u8 SPIx,u8 Data)
{	 
	if(1==SPIx)	
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	  //确保发生前Buffer为空，也就是说上一次已经发生完成   
	    SPI_I2S_SendData(SPI1, Data);    
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	    SPI_I2S_ReceiveData(SPI1);   //读寄存器用硬件清除标志位。
	}
	if(2==SPIx)	
	{
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);	  //确保发生前Buffer为空，也就是说上一次已经发生完成   
	    SPI_I2S_SendData(SPI2, Data);    
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	    SPI_I2S_ReceiveData(SPI2);   //读寄存器用硬件清除标志位。
	}
	if(3==SPIx)	
	{
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);	  //确保发生前Buffer为空，也就是说上一次已经发生完成   
	    SPI_I2S_SendData(SPI3, Data);    
		while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
	    SPI_I2S_ReceiveData(SPI3);   //读寄存器用硬件清除标志位。
	}
}
/*******************************************************************************
*函数功能：	SPI1接收一个数据
*函数描述：
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
*函数功能：	SPI1发送一个数据并接收一个数据
*函数描述：
*******************************************************************************/
u8 DK_SPI1_S_R_Byte(u8 SPIx,u8 byte)
{     

	if(1==SPIx)	
	{
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); //SPI1 等待发送数据
  SPI_I2S_SendData(SPI1, byte); //SPI1 发送数据
	
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//SPI1 等待接收数据
  return SPI_I2S_ReceiveData(SPI1); //从SPI1 接收数据 返回参数
	}
	if(2==SPIx)	
	{
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); //SPI1 等待发送数据
  SPI_I2S_SendData(SPI2, byte); //SPI1 发送数据
	
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);//SPI1 等待接收数据
  return SPI_I2S_ReceiveData(SPI2); //从SPI1 接收数据 返回参数
	}

	if(3==SPIx)	
	{
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET); //SPI1 等待发送数据
  SPI_I2S_SendData(SPI3, byte); //SPI1 发送数据
	
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);//SPI1 等待接收数据
  return SPI_I2S_ReceiveData(SPI3); //从SPI1 接收数据 返回参数
	}
	 return 0;
	
}

/*******************************************************************************
*函数功能：	设置SPI速度大小，SpeedSet参数为0表示低速模式，参数为1表示高速模式
*函数描述：
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
    //如果速度设置输入0，则低速模式，非0则高速模式
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

