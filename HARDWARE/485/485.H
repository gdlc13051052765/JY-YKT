#ifndef __485_H__
#define __485_H__


#define u485_OUT  DK_GPIO_Init(PE,GPIO_Pin_11,Out_PP)
#define u485_RE   PEout(11)=0;
#define u485_TE   PEout(11)=1;
void u485_GPIO_INT(void);
void Usart2ToMP3(void);

#endif

