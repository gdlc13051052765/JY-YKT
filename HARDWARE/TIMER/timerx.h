#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//定时器2初始化函数 
void TIM2_Int_Init(u16 arr,u16 psc);
//定时器3初始化函数
void TIM3_Int_Init(u16 arr,u16 psc);
//定时器3的关闭函数
void TIM3_Close_INT(void);
//定时器3打开的函数
void TIM3_OPen_INT(void);
#endif
