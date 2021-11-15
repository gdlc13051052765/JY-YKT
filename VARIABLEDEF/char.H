#ifndef __CHAR_H
#define __CHAR_H

#define		ulong	unsigned 	long
#define		uint	unsigned  	short
#define		uchar	unsigned  	char

struct	sDatasStruct//ͨѶ���ݸ�ʽ
{
	uint	UartSeAddrCode;
	uint	UartReAddrCode;
	uint	UartComd;
	uchar	UartStatus;
	unsigned char	UartAddrH;//��ַ��
	unsigned char	UartAddrL;//��ַ��
	unsigned char	UartFrameNum;//֡��
	unsigned char	DatasLen;//���ݳ���
	unsigned char	Datas[139];//���ݰ�
};
union	sDatasUnion
{
	unsigned char	S_DatasBuffer[150];
	struct	sDatasStruct	S_DatasStruct;
};

//ʱ��ṹ
struct	sTimeStruct
{
	unsigned char	YearChar;
	unsigned char	MonthChar;
	unsigned char	DayChar;
	unsigned char	HourChar;
	unsigned char	MinuteChar;
	unsigned char	SecondChar;
};
union	uTimeUnion
{
	unsigned char	TimeString[6];
	struct	sTimeStruct	S_Time;
}; 
#endif
