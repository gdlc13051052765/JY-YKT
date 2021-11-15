#ifndef __VS1003_H_
#define __VS1003_H_	
 


/*****************************************************************/

/*******************************
MP3家庭娱乐模块：VS1003
VS1003与单片机接口
VS_SCK:        串行总线的时钟
VS_MOSI:      串行输入
VS_MISO:      串行输出
XCS:          片选输入，低有效
XRESET:         异步复位，低有效
DREQ:       (中断)    数据请求输入总线
XDCS:           数据片选端
*******************************/
//VS1003接口初始化,XCS，XRESET,XDCS,SCK,MOSI输出高，DREQ上拉输入，MISO浮空输入
#define     VS1003_XCS_GPIO     PC
#define     VS1003_XCS_BIT     GPIO_Pin_13
#define     VS1003_XCS_Type    Out_PP
#define     VS1003_XCS         PCout(13)

#define 	VS1003_SCK_GPIO     PE
#define 	VS1003_SCK_BIT     GPIO_Pin_5
#define 	VS1003_SCK_Type    Out_PP
#define 	VS1003_SCK         PEout(5)

#define 	VS1003_MOSI_GPIO     PE
#define 	VS1003_MOSI_BIT     GPIO_Pin_4
#define 	VS1003_MOSI_Type    Out_PP
#define 	VS1003_MOSI  	    PEout(4)

#define 	VS1003_XRESET_GPIO     PC
#define 	VS1003_XRESET_BIT     GPIO_Pin_14
#define 	VS1003_XRESET_Type    Out_PP
#define 	VS1003_XRESET		  PCout(14)

#define 	VS1003_MISO_GPIO  	 PE
#define 	VS1003_MISO_BIT  	 GPIO_Pin_3
#define 	VS1003_MISO_Type     IN_FLOATING    //浮空输入
#define 	VS1003_MISO_PIN 	 PEin(3)

#define 	VS1003_DREQ_GPIO  	 PD
#define 	VS1003_DREQ_BIT  	 GPIO_Pin_14
#define 	VS1003_DREQ_Type     IPD  //上拉 输入
#define 	VS1003_DREQ_PIN  	 PDin(14)

#define 	VS1003_XDCS_GPIO  	 PD
#define 	VS1003_XDCS_BIT  	 GPIO_Pin_13
#define 	VS1003_XDCS_Type     Out_PP
#define 	VS1003_XDCS     	 PDout(13)

#define 	RC522_XCS_H()       VS1003_XCS=1
#define 	RC522_XCS_L()    	 VS1003_XCS=0
#define 	RC522_XRESET_H()    VS1003_XRESET=1
#define 	RC522_XRESET_L()    VS1003_XRESET=0
#define 	VS1003_XDCS_H()      VS1003_XDCS=1
#define 	VS1003_XDCS_L()      VS1003_XDCS=0

#define     CLOCK_REG       0x9800        //0x9800 is for VS1011 VS1002



void RC522_GPIO_INIT(void);
void RC522_WriteByte(unsigned char CH);
void RC522_WriteDATA(unsigned char dat);
unsigned char RC522_ReadByte(void);


#endif



