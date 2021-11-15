
#ifndef __DISP_1629FILES_H
#define __DISP_1629FILES_H
#include "includes.h"	
void 	Disp_Gpio(void);
void	Disp_AllOn(void);
void	Disp_AllOff(void);
void	DISP_ErrorSub(unsigned char);
void	DispMainCode(void);
void	SetNetDatas(void);
void	DispSumConsumMoney(unsigned char,unsigned char,unsigned long);
void	Disp_Balance(unsigned char * );//显示余额
void	Disp_Hello(void);
void	DispConsumModeEnable(unsigned char ,unsigned char );//显示消费允许方式
void	DispDate(unsigned char * );//显示日期
void	DispTime(unsigned char *);//显示时间
void	Disp_Clr_Ram(void);//显示清空Pos;
void	Disp_Ver(unsigned char);//显示版本号
void	ChgIntToDispBuffer(unsigned int,unsigned char * );
;
void	DispMinBalance(void);//显示底金
void	DispOver(void);
unsigned char	FindMonth(unsigned char * );//查找日期
void	LED_Indata(unsigned char bbit,unsigned char aa);  //写入数据
unsigned char 	ReadByteFrom1629(void);//倒序接收
void 	LED_DispDatas_1629(unsigned char ledx,unsigned char * ptr,unsigned char num);
void 	LED_DispDatas(unsigned char bbit,unsigned char * ptr);
void 	LED_DispDatas_all(unsigned char * ptr);
void	ReakKeySub(void);
void 	LED_OneByte_Disp(unsigned char ledx,unsigned char prt);
void 	LED_NumFalsh_Disp(unsigned char ledx,unsigned char num,unsigned int delays);
void 	Led_Wait_Disp(void);
#endif
