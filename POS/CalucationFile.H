#include "char.h"
		 
extern	uchar   	BytesCheckSum(uchar *,uchar);//校验和比较
extern	uchar     BytesComp( uchar * ,uchar *,uchar);//字符串比较，正确返回0
extern	uchar	    BCD_String_Diag(uchar * ,uchar);//BCD码字符串诊断
extern	uchar   BCDToHex(uchar  );
extern	uchar   HexToBCD(uchar  );
extern	ulong	ChgBCDStringToUlong(uchar * ,uchar);//BCD码字符串转换为整型数
extern	uchar	CalCheckSum(uchar * ,uchar);//计算校验和�
extern	void	ChgUlongToBCDString( ulong ,uchar * ,uchar );
extern	void	ChgIntToStrings(uint	ii,uchar * ptr);
extern	ulong	ChgInputToUlong(uchar * ,uchar);//输入的数字转换为长整形
extern	ulong 	GetU32_HiLo(uchar *);
extern	uint 	GetU16_HiLo(uchar *);
extern	void 	PutU32_HiLo(uchar *,ulong);
extern	void 	PutU16_HiLo(uchar *,uint);
extern	void	FormatBuffer(uchar * ,uchar *);
extern unsigned long	ChgKeyStringToUlong(uchar * ptr ,uchar Len);



 