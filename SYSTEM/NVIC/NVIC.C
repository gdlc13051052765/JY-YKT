#include "misc.h"
#include "NVIC.H"
/**************************************************************************************

**************************************************************************************/
void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}
void DK_NVIC_Init(unsigned char Group)
{
	switch(Group)
	{
		case 0:
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		break;
		case 1:
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
		break;
		case 2:
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		break;
		case 3:
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
		break;
		case 4:
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
		break;
	}
}

void DK_NVIC_IRQ_Set(unsigned char NVIC_IRQChannel, unsigned char pre_pri, unsigned char sub_pri, FunctionalState cmd)
{
  NVIC_InitTypeDef NVIC_InitStructure;		  
  NVIC_InitStructure.NVIC_IRQChannel = NVIC_IRQChannel;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pre_pri;  //抢占优先级		
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub_pri;				 //子优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = cmd;
  NVIC_Init(&NVIC_InitStructure);
}

