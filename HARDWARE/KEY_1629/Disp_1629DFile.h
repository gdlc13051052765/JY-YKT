
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
void	Disp_Balance(unsigned char * );//��ʾ���
void	Disp_Hello(void);
void	DispConsumModeEnable(unsigned char ,unsigned char );//��ʾ��������ʽ
void	DispDate(unsigned char * );//��ʾ����
void	DispTime(unsigned char *);//��ʾʱ��
void	Disp_Clr_Ram(void);//��ʾ���Pos;
void	Disp_Ver(unsigned char);//��ʾ�汾��
void	ChgIntToDispBuffer(unsigned int,unsigned char * );
;
void	DispMinBalance(void);//��ʾ�׽�
void	DispOver(void);
unsigned char	FindMonth(unsigned char * );//��������
void	LED_Indata(unsigned char bbit,unsigned char aa);  //д������
unsigned char 	ReadByteFrom1629(void);//�������
void 	LED_DispDatas_1629(unsigned char ledx,unsigned char * ptr,unsigned char num);
void 	LED_DispDatas(unsigned char bbit,unsigned char * ptr);
void 	LED_DispDatas_all(unsigned char * ptr);
void	ReakKeySub(void);
void 	LED_OneByte_Disp(unsigned char ledx,unsigned char prt);
void 	LED_NumFalsh_Disp(unsigned char ledx,unsigned char num,unsigned int delays);
void 	Led_Wait_Disp(void);
#endif
