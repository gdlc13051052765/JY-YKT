#include "MyDefine.h"
#include "char.h"

unsigned char   	BytesCheckSum(uchar *,uchar);//Ð£ÑéºÍ±È½Ï
unsigned char     BytesComp( uchar * ,uchar *,uchar);//×Ö·û´®±È½Ï£¬ÕýÈ··µ»Ø0
unsigned char	CalCheckSum(uchar * ,uchar);//¼ÆËãÐ£ÑéºÍ£
unsigned char	    BCD_String_Diag(uchar * ,uchar);//BCDÂë×Ö·û´®Õï¶Ï
unsigned long	ChgBCDStringToUlong(uchar * ,uchar);//BCDÂë×Ö·û´®×ª»»ÎªÕûÐÍÊý
void	ChgUlongToBCDString( ulong ,uchar * ,uchar );
unsigned char   HexToBCD(uchar aa );
unsigned char   BCDToHex(uchar aa );
void	ChgIntToStrings(uint,uchar *);
unsigned long	ChgInputToUlong(uchar * ,uchar);//ÊäÈëµÄÊý×Ö×ª»»Îª³¤ÕûÐÎ

unsigned long 	GetU32_HiLo(uchar *);
uint 	GetU16_HiLo(uchar *);
void 	PutU32_HiLo(uchar *,ulong);
void 	PutU16_HiLo(uchar *,uint);
void	FormatBuffer(uchar * ,uchar *);

unsigned long	ChgKeyStringToUlong(uchar * ptr ,uchar Len);

unsigned long	ChgKeyStringToUlong(uchar * ptr ,uchar Len)//BCDÂë×Ö·û´®×ª»»ÎªÕûÐÍÊý
{
	unsigned long		ii=0;
	ulong		jj=1;
	uchar		aa;
	do
	{
		aa=(*(ptr+Len-1));
		ii|=aa*jj;
		jj=jj*0x100;
	}while(--Len);
	return	ii;
}

ulong	ChgStringsToInt(uchar * ptr)
{
	uchar	i;
	uint	kk=1;
	ulong	ii=0;
	uchar	st_data;
	for (i=0;i<5;i++)
	{
		st_data= ptr[4-i];
		ii+=(ulong)kk*st_data;	
		kk=kk*10;
	}	
	return	ii;
}

uchar BytesComp( uchar * CharDptr1,uchar * CharDptr2,uchar CompNum)
{
	uchar 			aa;
	uchar			bb;
	do
	{
		aa=* CharDptr1++;
		bb=* CharDptr2++;
		if (aa!=bb)	return 1; 
	}while (--CompNum);
	return 0;
}
uchar	BytesCheckSum(uchar * ptr,uchar Len)
{
	uchar			aa=0;
	uchar 			bb;
	Len--;
	do
	{
		aa+=* ptr++;
	}while (--Len);	 
	bb= * ptr;
	aa=~aa;
	if (aa!=bb) 
		return 1;
	else
		return 0;
}
uchar	CalCheckSum(uchar  * Ptr,uchar charLen)
{
	uchar 	st_data=0;
	do
	{
		st_data+= * Ptr++;
	}while (--charLen);
	st_data=~st_data;
	return st_data;
}
uchar	BCD_String_Diag(uchar * ptr ,uchar Len)//BCDÂë×Ö·û´®Õï¶Ï
{
	uchar			aa,bb;
	if (!Len)
		return	0;
	do
	{
		aa=* ptr++;
		bb=aa>>4;
		if (bb>9)
			return	1;
		bb=aa & 15;
		if (bb>9)
			return	1;
	}while (--Len);
	return	0;
}
ulong	ChgBCDStringToUlong(uchar * ptr ,uchar Len)//BCDÂë×Ö·û´®×ª»»ÎªÕûÐÍÊý
{
	ulong		ii=0;
	ulong		jj=1;
	uchar		aa;
	do
	{
		aa=BCDToHex(* (ptr+Len-1));
		ii+=aa*jj;
		jj=jj*100;		
	}while (--Len);
	return	ii;
}
void	ChgUlongToBCDString( ulong iii,uchar * ptr,uchar Len)
{
	uchar 		i;
	uchar 		aa;
	ulong 		jj=1;
	for (i=0;i<Len-1;i++)
		jj=jj*100;
 	for (i=0;i<Len;i++)
 	{
 		aa=iii/jj;
		* ptr++=HexToBCD(aa);
		iii=iii%jj;
		jj=jj/100;
 	}	
}

uchar HexToBCD(uchar aa )
{
	return((aa/10)*16+aa%10);
}
uchar BCDToHex(uchar aa )
{
	return((aa/16)*10+aa%16);
}


void	ChgIntToStrings(uint	ii,uchar * ptr)
{
	uint	kk=10000;
	uchar	st_data;
	uchar	i;
	for (i=0;i<5;i++)
	{
		st_data=ii/kk;
	//	ptr[i]=0x30+st_data;
		ptr[i]=st_data;
		ii=ii%kk;
		kk=kk/10;
	}	
}


ulong	ChgInputToUlong(uchar * ptr,uchar Num)//ÊäÈëµÄÊý×Ö×ª»»Îª³¤ÕûÐÎ
{
	uchar	i,st_data,j;
	ulong	iii=0;
	ulong	jjj=100;
	uchar	SumNum=0;
//	uchar	PointX=0xff;
	uchar		bbit=0;
	for (i=0;i<Num;i++)
	{
		st_data=ptr[i];
		if (st_data!=0xff )
		{
			SumNum++;
			if ((st_data&0x80))
			{
				bbit=1;
				for (j=0;j<i;j++)
					jjj=jjj*10;	
			}
		}
		else
			break;
	}
	if (!bbit)
	{
		for (i=0;i<SumNum-1;i++)
			jjj=jjj*10;
	}
	for (i=0;i<SumNum;i++)
	{
		iii+=(ptr[i]&0x0f)*jjj;
		jjj=jjj/10;
		if (!jjj)
			break;
	}
	return	iii;
}
void	FormatBuffer(uchar * ptr ,uchar * Len)
{
	uchar i;
	uchar	j=0;
	uchar	aaa[4];
	uchar		bbit=0;
	memset(aaa,0xff,4);
	for (i=0;i<4;i++)
	{
		if (ptr[i] || bbit)
		{
			bbit=1;
			aaa[j++]=ptr[i];
		}		
	}
	if (!bbit)
	{
		aaa[0]=0;
		j=1;
	}
	memcpy(ptr,aaa,4);
	Len[0]=j;		
}

//È¡ËÄ×Ö½ÚÊý¾Ý(¸ßÎ»ÔÚÇ°)
ulong GetU32_HiLo(uchar * lbuf)
{
	uchar * p_buf;
	ulong  r_buf;
	
	p_buf = lbuf;
	r_buf = (ulong)p_buf[3] + ((ulong)p_buf[2]<<8) + ((ulong)p_buf[1]<<16) + ((ulong)p_buf[0]<<24);
	return r_buf;
} 

//È¡Ë«×Ö½ÚÊý¾Ý(¸ßÎ»ÔÚÇ°)
uint GetU16_HiLo(uchar * lbuf)
{
	uchar * p_buf;
	uint  r_buf;
	
	p_buf = lbuf;
	r_buf = (uint)p_buf[1] + ((uint)p_buf[0]<<8);
	return r_buf;
} 
 
//ÉèÖÃËÄ×Ö½ÚÊý¾Ý(¸ßÎ»ÔÚÇ°)
void PutU32_HiLo(uchar * lbuf,ulong ldata)
{
	lbuf[0]=(uchar)(ldata>>24);
	lbuf[1]=(uchar)(ldata>>16);
	lbuf[2]=(uchar)(ldata>>8);
	lbuf[3]=(uchar)(ldata);
} 

//ÉèÖÃË«×Ö½ÚÊý¾Ý(¸ßÎ»ÔÚÇ°)
void PutU16_HiLo(uchar * lbuf,uint ldata)
{	
	lbuf[0]=(uchar)(ldata>>8);
	lbuf[1]=(uchar)(ldata);
}

//Ë«×Ö½Ú´óÐ¡¶Ë×ª»»
uint16_t DoubleBigToSmall(uint16_t a)
{
	uint16_t c;
	unsigned char b[2];
	b[0] =(unsigned char) (a);
	b[1] = (unsigned char)(a>>8);
	c = (uint16_t)(b[0]<<8) + b[1];
	return c;
}

//ËÄ×Ö½Ú´óÐ¡¶Ë×ª»»
int32_t FourBigToSmall(uint32_t a)
{
	uint32_t c;
	unsigned char b[4];
	b[0] =(unsigned char) (a);
	b[1] = (unsigned char)(a>>8);
	b[2] = (unsigned char)(a>>16);
	b[3] = (unsigned char)(a>>24);
	c = (uint32_t)(b[0]<<24) +(uint32_t)(b[1]<<16)+(uint32_t)(b[2]<<8)+ b[3];
	return c;		
}
 
