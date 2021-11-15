#ifndef __CHAR_H
#define __CHAR_H

#define		ulong	unsigned 	long
#define		uint	unsigned  	short
#define		uchar	unsigned  	char

struct	sDatasStruct//通讯数据格式
{
	uint	UartSeAddrCode;
	uint	UartReAddrCode;
	uint	UartComd;
	uchar	UartStatus;
	unsigned char	UartAddrH;//地址高
	unsigned char	UartAddrL;//地址低
	unsigned char	UartFrameNum;//帧号
	unsigned char	DatasLen;//数据长度
	unsigned char	Datas[139];//数据包
};
union	sDatasUnion
{
	unsigned char	S_DatasBuffer[150];
	struct	sDatasStruct	S_DatasStruct;
};

//时间结构
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

