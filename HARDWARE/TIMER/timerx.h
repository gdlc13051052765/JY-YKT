#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//��ʱ��2��ʼ������ 
void TIM2_Int_Init(u16 arr,u16 psc);
//��ʱ��3��ʼ������
void TIM3_Int_Init(u16 arr,u16 psc);
//��ʱ��3�Ĺرպ���
void TIM3_Close_INT(void);
//��ʱ��3�򿪵ĺ���
void TIM3_OPen_INT(void);
#endif
