#include "485.H"
#include "GPIO.H"
#include "USART.H"

//串口的4052选择管脚配置
#define 	COM_SEL0_GPIO     		PE
#define 	COM_SEL0_BIT      		GPIO_Pin_15
#define 	COM_SEL0_Type_Out     	Out_PP

#define 	COM_SEL1_GPIO     		PE
#define 	COM_SEL1_BIT      		GPIO_Pin_14
#define 	COM_SEL1_Type_Out     	Out_PP

#define 	COM1_SEL0_GPIO     		PE
#define 	COM1_SEL0_BIT      		GPIO_Pin_13
#define 	COM1_SEL0_Type_Out     	Out_PP

#define 	COM1_SEL1_GPIO     		PE
#define 	COM1_SEL1_BIT      		GPIO_Pin_12
#define 	COM1_SEL1_Type_Out     	Out_PP
 
//IO方向设置
#define COM_SEL0_OUT()  DK_GPIO_Init(COM_SEL0_GPIO,COM_SEL0_BIT,COM_SEL0_Type_Out);
#define COM_SEL1_OUT()  DK_GPIO_Init(COM_SEL1_GPIO,COM_SEL1_BIT,COM_SEL1_Type_Out);
#define COM1_SEL0_OUT() DK_GPIO_Init(COM1_SEL0_GPIO,COM1_SEL0_BIT,COM1_SEL0_Type_Out);
#define COM1_SEL1_OUT() DK_GPIO_Init(COM1_SEL1_GPIO,COM1_SEL1_BIT,COM1_SEL1_Type_Out);
//IO操作函数	 
#define COM_SEL0    PEout(15) //
#define COM_SEL1    PEout(14) //
#define COM1_SEL0    PEout(13) //
#define COM1_SEL1    PEout(12) //

/***********************************************************************
*函数描述：
*函数功能：
*************************************************************************/
void u485_GPIO_INT(void)
{
	u485_OUT  ;
	u485_RE;
}
//串口的4052选择管脚初始化
void USART_COMSEL_GPIO_INIT(void)
{
	COM_SEL0_OUT();
	COM_SEL1_OUT();
	COM1_SEL0_OUT();
	COM1_SEL1_OUT();
}
//485模式4052选择
void COMSEL_485Mode(void)
{
	COM_SEL0 = 0;
	COM_SEL1 = 0;	
}
//232模式4052选择
void COMSEL_232Mode(void)
{
	COM_SEL0 = 1;
	COM_SEL1 = 0;	
}
//串口3空闲
void COMSEL_NoMode(void)
{
	COM_SEL0 = 1;
	COM_SEL1 = 1;	
}
//232模式4052选择
void SelWifiMode(void)
{
	COM_SEL0 = 0;
	COM_SEL1 = 1;	
}
 //串口2工作在打印机模式
 void Usart2ToPrint(void)
 {
 	COM1_SEL0 = 0;
	COM1_SEL1 = 0;
 }
  //串口2工作在MP3模式
 void Usart2ToMP3(void)
 {
 	COM1_SEL0 = 1;
	COM1_SEL1 = 0;
 }