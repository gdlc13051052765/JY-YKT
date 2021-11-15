/**************************************************************************************
£
**************************************************************************************/ 
#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "MoNiSPI.H"	

//VS1003«˝∂Ø÷–µƒ—” ±∫Ø ˝£¨»Îø⁄≤Œ ˝Œ™0µΩ65535
static void VS1003_Delay_nus(unsigned short nus)
{
	for(;nus>0;nus--)
	{__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();}
}
static void VS1003_Delay_nms(unsigned short nms)
{
	for(;nms>0;nms--)
	{VS1003_Delay_nus(1000);}
}



//VS1003”Îµ•∆¨ª˙Ω”ø⁄≥ı ºªØ,XCS£¨XRESET,XDCS,SCK,MOSI ‰≥ˆ∏ﬂ£¨DREQ…œ¿≠ ‰»Î£® ‰≥ˆµÁ¡˜£©£¨MISO∏°ø’ ‰»Î
void RC522_GPIO_INIT(void)
{
  DK_GPIO_Init(VS1003_XCS_GPIO,VS1003_XCS_BIT,VS1003_XCS_Type); 
	DK_GPIO_Init(VS1003_XRESET_GPIO,VS1003_XRESET_BIT,VS1003_XRESET_Type);
	DK_GPIO_Init(VS1003_XDCS_GPIO,VS1003_XDCS_BIT,VS1003_XDCS_Type);
	DK_GPIO_Init(VS1003_SCK_GPIO,VS1003_SCK_BIT,VS1003_SCK_Type);
	DK_GPIO_Init(VS1003_MOSI_GPIO,VS1003_MOSI_BIT,VS1003_MOSI_Type);
	DK_GPIO_Init(VS1003_MISO_GPIO,VS1003_MISO_BIT,VS1003_MISO_Type);
	DK_GPIO_Init(VS1003_DREQ_GPIO,VS1003_DREQ_BIT,VS1003_DREQ_Type);

	VS1003_XCS=1;
	VS1003_XRESET=1;
	VS1003_XDCS=1;
	VS1003_SCK=1;
	VS1003_MOSI=1;
}  
//œÚVS1003–¥»Î“ª∏ˆ◊÷Ω⁄
void RC522_WriteByte(u8 CH)
{   
	u8 i,data_bf;
	data_bf=CH;
	
	for(i=0;i<8;i++)
	{
	    if(data_bf&0x80)
		    VS1003_MOSI=1;    //MOSI=1;
		else
		    VS1003_MOSI=0;   //MOSI=0;
		VS1003_SCK=1;  //¿≠∏ﬂVS1003_SCK
		data_bf=data_bf<<1;
		VS1003_SCK=0;	//¿≠µÕVS1003_SCK
	}
}

//¥”VS1003∂¡»°“ª∏ˆ◊÷Ω⁄
u8 RC522_ReadByte(void)
{
    u8 i,data_bf=0;
	for(i=0;i<8;i++)
	{
	    data_bf=data_bf<<1;
		VS1003_SCK=1;   //¿≠∏ﬂVS1003_SCK
		if(VS1003_MISO_PIN)
		    data_bf|=0x01;
	    else
		    data_bf&=0xfe;
		VS1003_SCK=0;		//¿≠µÕVS1003_SCK
	}
	return data_bf;
}






