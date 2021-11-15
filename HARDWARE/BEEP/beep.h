#ifndef __BEEP_H
#define __BEEP_H

#define		ulong	unsigned 	long
#define		uint	unsigned	short
#define		uchar	unsigned  	char

#define 	BEEP_GPIO     		PE
#define 	BEEP_BIT      		GPIO_Pin_6
#define 	BEEP_Type	     	Out_PP
#define 	BEEP_OUT            PEout(6)  

void	Beep_init(void);
void	BeepOn(unsigned char);
#endif

