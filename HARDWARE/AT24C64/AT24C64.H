
#ifndef __AT512HD_H
#define __AT512HD_H
				   

extern void Init_AT24C512(void);
extern  unsigned char AT24C512_ReadOneByte(unsigned short ReadAddr);
extern  void WrByteToAT24C512(unsigned short WriteAddr,unsigned char DataToWrite);
extern unsigned char RdBytesFromAT24C64(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead);
extern unsigned char WrBytesToAT24C64(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite);
	 

#endif

