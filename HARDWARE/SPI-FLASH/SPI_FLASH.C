#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "DK_SPI.H"
#include "SPI_FLASH.H"
#include "stm32f10x_spi.h"	
#include "USART.H"
#include "char.H"
#include  <string.h>
#include "stm32f10x.h"
#include "MyDefine.h"
#include "delay.h"
#include "485.h"
#include 	"MyDefine.h"
#include	"ExternVariableDef.h"


unsigned char SST25_buffer[40];
void Erase_One_Sector(unsigned long a1);
extern void USART_PUTS_Arry(unsigned char USARTx,unsigned char *buffer,unsigned int len);
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))
uint8_t TxBuffer1[] = "SPI SST25VF016B Example: This is SPI DEMO, 终端上出现这一行字，说明SST25VF016B的读写正常";
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
/**************************************************************************************

**************************************************************************************/


void Busy_Test(void);   //忙检测
void Write_EN(void);  //写使能
void Write_DIS(void);  //写禁止
void Write_CMD(void); //写状态
unsigned char Read_CMD(void); //读状态
void Page_Clr(unsigned long a1);  //页擦除

/* Select SPI FLASH: ChipSelect pin low  */
#define Select_Flash()      PBout(12)=0                //GPIO_ResetBits(GPIOA, GPIO_Pin_4)
/* Deselect SPI FLASH: ChipSelect pin high */
#define NotSelect_Flash()    PBout(12)=1               //GPIO_SetBits(GPIOA, GPIO_Pin_4)




 void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
/**************************************************************************************
* 名    称:    void Write_EN(void)
* 功    能:    写使能
* 参    数:    
* 返 回 值:    无
* 作    者：   D&K
**************************************************************************************/ 
void Write_EN(void){
	Select_Flash();        //SPI CS 使能
	SPI_Flash_SendByte(0x06); //写使能指令
	NotSelect_Flash();  //SPI CS 禁用
}


/**************************************************************************************
* 名    称:    Write_DIS(void)
* 功    能:    写禁止
* 参    数:    
* 返 回 值:    无
* 作    者：   D&K
**************************************************************************************/ 
void Write_DIS(void){

	Select_Flash();  //SPI CS 使能
	SPI_Flash_SendByte(0x04); //写禁止指令
	NotSelect_Flash();  //SPI CS 禁用
	Busy_Test();   //忙检测
	
}	


/**************************************************************************************
* 名    称:    Busy_Test(void)
* 功    能:    忙检测
* 参    数:    
* 返 回 值:    无
* 作    者：   D&K
**************************************************************************************/ 
void Busy_Test(void){
	unsigned char a=1;
	int i = 0;
	while((a&0x01)==1) 
	{
		a=Read_CMD();
		i++;
		if(i>=100)
		{
			i = 0;
			Clr_WatchDog();	
		}
		
	}	

}

/**************************************************************************************
* 名    称:    unsigned char Read_CMD(void)
* 功    能:    读状态寄存器
* 参    数:    
* 返 回 值:    无
* 作    者：   D&K
**************************************************************************************/ 
unsigned char Read_CMD(void){
	unsigned char busy;
	Select_Flash();
	SPI_Flash_SendByte(0x05);
	busy = SPI_Flash_ReadByte();
	NotSelect_Flash();
	return(busy);
	
}

/**************************************************************************************
* 名    称:    SST25_R_BLOCK(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize)
* 功    能:    页读
* 参    数:    unsigned long addr--页   unsigned char *readbuff--数组   unsigned int BlockSize--长度
* 返 回 值:    无
* 作    者：   D&K
**************************************************************************************/ 
void Flash_Rd_Bytes(unsigned long addr, unsigned char *readbuff, unsigned short BlockSize){
	
	unsigned int i=0; 	
	Select_Flash();
	SPI_Flash_SendByte(0x03);
	SPI_Flash_SendByte((addr&0xffffff)>>16);
	SPI_Flash_SendByte((addr&0xffff)>>8);
	SPI_Flash_SendByte(addr&0xff);
	//SPI_Flash_SendByte(0);
	while(i<BlockSize){	
		readbuff[i]=SPI_Flash_ReadByte();		
		i++;
	}
	NotSelect_Flash();	

	Busy_Test();
}


uchar tmpdataread[1024] = {0};
void Force_Flash_Rd_Bytes(unsigned long addr, unsigned char *readbuff, unsigned short BlockSize)
{
	memset(tmpdataread,0,BlockSize);
	while(1)
	{
		Flash_Rd_Bytes(addr,readbuff,BlockSize);
		if(memcmp(tmpdataread,readbuff,BlockSize) == 0)
		{
			break;
		}else
		{
			memcpy(tmpdataread,readbuff,BlockSize);
		}
	}
}

/**************************************************************************************
* 名    称:    SST25_W_BLOCK(uint32_t addr, u8 *readbuff, uint16_t BlockSize)
* 功    能:    页写
* 参    数:    uint32_t addr--页   u8 *readbuff--数组   uint16_t BlockSize--长度    
* 返 回 值:    无
* 作    者：   D&K
**************************************************************************************/ 
void Flash_Write_Bytes_onepage(uint32_t addr, u8 *readbuff, unsigned short BlockSize){
	unsigned int i=0,a2;
//	Erase_One_Sector(addr);   								  //删除页		  
//	Write_CMD();
  	Write_EN();	
	Select_Flash();    
	SPI_Flash_SendByte(0x02);
	SPI_Flash_SendByte((addr&0xffffff)>>16);
	SPI_Flash_SendByte((addr&0xffff)>>8);
	SPI_Flash_SendByte(addr&0xff);
 // 	SPI_Flash_SendByte(readbuff[0]);
	//SPI_Flash_SendByte(readbuff[1]);
//	NotSelect_Flash();
//	i=2;
	i = 0;
	while(i<BlockSize){
		//a2=120;
		//while(a2>0) a2--;
		//Select_Flash();
		//SPI_Flash_SendByte(0xad);
		SPI_Flash_SendByte(readbuff[i++]);
		//SPI_Flash_SendByte(readbuff[i++]);
		//NotSelect_Flash();
	}
	
	a2=100;
	while(a2>0) a2--;
	NotSelect_Flash();
	Write_DIS();	
	//Select_Flash();	
	Busy_Test();
}


void Flash_Write_Bytes(u32 WriteAddr,u8* pBuffer,u16 NumByteToWrite)   
{  
	u16 pageremain;   
	pageremain=256-WriteAddr%256; //单页剩余的字节数    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//如果要写入的字节数小或等于单页剩余的字节数。不大于256个字节
	while(1)
	{   
		Flash_Write_Bytes_onepage(WriteAddr,pBuffer,pageremain);//要写入的字节数小或等于单页剩余的字节数直接写
		if(NumByteToWrite==pageremain)break;//写入结束了

		else //NumByteToWrite>pageremain，如果要写入的数据大于单页剩余的字节数。
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;

			NumByteToWrite-=pageremain;  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite;  //不够256个字节了
		}
	}   
} 

uchar tmpdatawrite[1024];
void Force_Flash_Write_Bytes(u32 WriteAddr,u8* pBuffer,u16 NumByteToWrite)   
{
	memset(tmpdatawrite,0,NumByteToWrite);
	while(1)
	{
		Flash_Write_Bytes(WriteAddr,pBuffer,NumByteToWrite);
		Flash_Rd_Bytes(WriteAddr,tmpdatawrite,NumByteToWrite);
		if(memcmp(tmpdatawrite,pBuffer,NumByteToWrite) == 0)
		{
			break;
		}else
		{
			//memcpy(tmpdataread,readbuff,BlockSize);
		}
	}
}

/**************************************************************************************
* 名    称:    Page_Clr(unsigned long a1)
* 功    能:    页擦除
* 参    数:    unsigned long a1--页   
* 返 回 值:    无
* 作    者：   D&K
**************************************************************************************/ 
void Erase_One_Sector(unsigned long a1){
	//Write_CMD();
	Write_EN();     
	Select_Flash();	  
	SPI_Flash_SendByte(0x20);
	SPI_Flash_SendByte((a1&0xffffff)>>16);          //addh
	SPI_Flash_SendByte((a1&0xffff)>>8);          //addl 
	SPI_Flash_SendByte(a1&0xff);                 //wtt
	NotSelect_Flash();
	Busy_Test();
	
}

/**************************************************************************************
* 名    称:    Page_Clr(unsigned long a1)
* 功    能:    擦除整个芯片
* 参    数:    unsigned long a1--页   
* 返 回 值:    无
* 作    者：   D&K
**************************************************************************************/ 
void	Erase_Entire_Chip(void)  //擦除整个芯片
{
	//Write_CMD();
	Write_EN();     
	Select_Flash();	  
	SPI_Flash_SendByte(0xc7);   
	delay_ms(200);              //wtt
	NotSelect_Flash();
	Busy_Test();
	delay_ms(200);
	
}

/**************************************************************************************
* 名    称:    FlashReadID(void)
* 功    能:    读工厂码及型号的函数
* 参    数:    
* 返 回 值:    
* 作    者：   D&K
**************************************************************************************/ 
void FlashReadID(void)
{
//	char fac_id,dev_id;
	Select_Flash();	
  	SPI_Flash_SendByte(0x90);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	//fac_id = 0;
  //	fac_id= SPI_Flash_ReadByte();		          //BFH: 工程码SST
//	dev_id = 0;
//	dev_id= SPI_Flash_ReadByte();	              //41H: 器件型号SST25VF016B    
  	NotSelect_Flash();	
}

/**************************************************************************************
* 名    称:    SPI_Flash_Init(void)
* 功    能:    初始化SPI FLASH 驱动
* 参    数:    
* 返 回 值:    
* 作    者：   D&K
**************************************************************************************/ 
void SPI_Flash_Init(void)
{
  DK_SPI_Init(2);
}

/**************************************************************************************
* 名    称:    SPI_Flash_SendByte(u8 byte)
* 功    能:    从SPI FLASH 读取一个字节  必须使用在Start_Read_Sequence前
* 参    数:    
* 返 回 值:    
* 作    者：   D&K
**************************************************************************************/ 
u8 SPI_Flash_ReadByte(void)
{
  return (SPI_Flash_SendByte(Dummy_Byte));
}

/**************************************************************************************
* 名    称:    SPI_Flash_SendByte(u8 byte)
* 功    能:    通过内部SPI发送一个字节并从SPI总线返回字节
* 参    数:    字节
* 返 回 值:    无
* 作    者：   D&K
**************************************************************************************/ 
u8 SPI_Flash_SendByte(u8 byte)
{
//   while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); //SPI1 等待发送数据
//   SPI_I2S_SendData(SPI1, byte); //SPI1 发送数据
// 	
//   while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//SPI1 等待接收数据
//   return SPI_I2S_ReceiveData(SPI1); //从SPI1 接收数据 返回参数 	
	
	   return DK_SPI1_S_R_Byte(2,byte);
}

void UP_Data(void)
{	u32 add;
	u16 i;
	 u485_TE;
	for(i=0;i<SaveRecordIndex;i++)
	{
		add = i*RECORD_SIZE;
		Flash_Rd_Bytes(add, SST25_buffer,RECORD_SIZE);
		USART_Send_Char(1,(i>>8));
		USART_Send_Char(1,i);
		USART_PUTS_Arry(1,SST25_buffer,RECORD_SIZE);//adlc测试时候使用
		delay_ms(3000);	
	}  
/*	for(i=0;i<128;i++)
	{
		add = i*RECORD_SIZE;
		SST25_buffer[0]=0;
		Flash_Write_Bytes(add, SST25_buffer,1);
		Flash_Rd_Bytes(add, SST25_buffer,RECORD_SIZE);
		USART_Send_Char(1,(i>>8));
		USART_Send_Char(1,i);
		USART_PUTS_Arry(1,SST25_buffer,RECORD_SIZE);//adlc测试时候使用
		delay_ms(5000);	  
	}  */
}

u8 dataf[1024];
u8 datatmp[1024];

void testflash()
{
	ulong i=0;
	int j = 0;
	ulong errcnt = 0;
	
	Erase_Entire_Chip();
	
	for(i=0;i<512;i++)
	{
		dataf[i] = 0xAA;
	}
	
	for(i=0;i<1024;i++)
	{
		while(1)
		{
			Flash_Write_Bytes(i*512,dataf,512);
			Flash_Rd_Bytes(i*512,datatmp,512);
			if(memcmp(dataf,datatmp,512) == 0)
			{
				break;
			}
		}
		
		
	}
	
	while(1)
	{
		errcnt = 0;
		
		for(i=0;i<512*1024;i++)
		{
			Flash_Rd_Bytes(i,datatmp,1);
			
			if(datatmp[0] != 0xAA)
			{
				errcnt++;
			}
			
		}
		errcnt = errcnt;
		
	}
	
}


//FLASH硬件自检
u8	Check_SPI_FLASH(void)
{	
	char Fac_id,Dev_id ;
	Select_Flash();	
  	SPI_Flash_SendByte(0x90);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	delay_ms(100);
  	Fac_id= SPI_Flash_ReadByte();		          //BFH: 工程码SST
	Dev_id= SPI_Flash_ReadByte();	              //8dH: 器件型号SST25VF04B    
  	NotSelect_Flash();	
	if(Fac_id == 0xef)
	{
		return 0;
	}
	else
	return 1;	
}