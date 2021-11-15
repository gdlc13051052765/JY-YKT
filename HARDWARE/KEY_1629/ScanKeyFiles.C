#include "MyDefine.h"
#include "ExternVariableDef.h"
#include "ExternVoidDef.h"
#include "char.h"

unsigned long	ScanKeySub(ulong);//读出键值
unsigned long	ScanKeySub3(ulong);//读出键值
unsigned char	ChgKeyValueToChar(ulong);//将键值转换为0-9

unsigned long	ScanKeySub(ulong	ll)//尽量只用在扫描一个键时
{
	unsigned long	lll_Data=0;
	if (bitHaveKey)
	{
		if (KeyValue & ll)
		{
			switch(KeyValue)
			{
///////////////////////////键盘按键定义//////////////////
				case		KEY_SIMPLE		:   
				case		KEY_NUM			:  //0x00020000
				case		KEY_RATION		:  //0x00010000
				case		KEY_MONEY		: //0x00001000
				case		KEY_MENU		:		//0x00002000		
				case		KEY_ESC			:				//0x00004000
				case		KEY_MUL			: //0x00040000
				case		KEY_ENTER		: //0x00008000
				case		KEY_ADD			: //0x00800000
				case		KEY_POINT		: //0x08000000
				case		KEY_0			: //0x80000000
				case		KEY_1			:  //0x10000000
				case		KEY_2			:  //0x01000000
				case		KEY_3			:		//0x00100000
				case		KEY_4			:  //0x20000000
				case		KEY_5			://0x02000000
				case		KEY_6			: //0x00200000
				case		KEY_7			: //0x40000000
				case		KEY_8			: //0x04000000
				case		KEY_9			: //0x00400000
				case		KEY_0_9			:
				case		KEY_ANY			:
				case		KEY_NO			:

					BeepOn(1);
					lll_Data=KeyValue;
					break;
				default:
					break;
			}		
	 	}
		bitHaveKey=0;
		KeyValue=0;
	}
	return	lll_Data;
}
unsigned long	ScanKeySub3(unsigned long	ll)
{
	unsigned long		lll_Data=0;
	if (bitHaveKey)
	{
		if (KeyValue & ll)
		{
			BeepOn(1);
			lll_Data=KeyValue;
	 	}
		bitHaveKey=0;
		KeyValue=0;
	}
	return	lll_Data;
}
//zjx_change8_080602
unsigned char	ChgKeyValueToChar(unsigned long ll)//将键值转换为0-9
{
	if (ll==KEY_0)
		return	0;
	else if (ll==KEY_1)
		return	1;
	else if (ll==KEY_2)
		return	2;
	else if (ll==KEY_3)
		return	3;
	else if (ll==KEY_4)
		return	4;
	else if (ll==KEY_5)
		return	5;
	else if (ll&KEY_6)
		return	6;
	else if (ll==KEY_7)
		return	7;
	else if (ll==KEY_8)
		return	8;
	else if (ll==KEY_9)
		return	9;
}

unsigned long	ChgCharValueToKey(unsigned long ll)//将键值转换为0-9
{
	if (ll==0)
		return	KEY_0;
	else if (ll==1)
		return	KEY_1;
	else if (ll==2)
		return KEY_2;
	else if (ll==3)
		return	KEY_3;
	else if (ll==4)
		return	KEY_4;
	else if (ll==5)
		return	KEY_5;
	else if (ll==6)
		return	KEY_6;
	else if (ll==7)
		return	KEY_7;
	else if (ll==8)
		return	KEY_8;
	else if (ll==9)
		return	KEY_9;
}
