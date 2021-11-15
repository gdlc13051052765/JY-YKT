#ifndef __PCF8563_H_
#define __PCF8563_H_	


unsigned char PCF8563_set(unsigned char year,unsigned char month,unsigned char day,unsigned char hour,unsigned char minute,unsigned char second,unsigned char date);
unsigned char PCF8563_read(unsigned char *time);
unsigned char PCF8563_CloseAlarm(void);
unsigned char PCF8563_SetAlarm(unsigned char time,unsigned char set);
void PCF8563_Init(void);
void PCF8563_INT_SERVER_FUN(void);
unsigned char PCF8563_ClearINT(void);

#endif

