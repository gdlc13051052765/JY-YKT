#include "MyDefine.h"
#include "ExternvoidDef.h"
#include "ExternVariableDef.h"
#include "PCF8563.H"
#include "stm32f10x_gpio.h"
#include "GPIO.H"
#include "char.H"
#include "IIC.H"
#include "CalucationFile.H"


#define PCF8563_Read_Add	0xA3
#define PCF8563_Write_Add	0xA2 

//ģ��IIC�ܽ�����
#define 	SWIIC_SDA_GPIO     		PB
#define 	SWIIC_SDA_BIT      		GPIO_Pin_7
#define 	SWIIC_SDA_Type_Out     	Out_PP
#define 	SWIIC_SDA_Type_In     	IPU
#define 	SWIIC_SDA          		PEout(11) 

#define 	SWIIC_CLK_GPIO     		PB
#define 	SWIIC_CLK_BIT      		GPIO_Pin_6
#define 	SWIIC_CLK_Type	     	Out_PP
#define 	SWIIC_CLK          		PEout(10)  
//IO��������
#define CLK_OUT() DK_GPIO_Init(SWIIC_CLK_GPIO,SWIIC_CLK_BIT,SWIIC_CLK_Type);
#define SDA_IN()  DK_GPIO_Init(SWIIC_SDA_GPIO,SWIIC_SDA_BIT,SWIIC_SDA_Type_In);
#define SDA_OUT() DK_GPIO_Init(SWIIC_SDA_GPIO,SWIIC_SDA_BIT,SWIIC_SDA_Type_Out);
//IO��������	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //����SDA 

//IIC���в�������
static void Init_SWIIC(void);                //��ʼ��IIC��IO��				 
static void IIC_Start(void);				//����IIC��ʼ�ź�
static void IIC_Stop(void);	  			//����IICֹͣ�ź�
static void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
static unsigned char IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
static unsigned char IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
static void IIC_Ack(void);					//IIC����ACK�ź�
static void IIC_NAck(void);				//IIC������ACK�ź�
static unsigned char IIC_WriteBuf(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite);
static unsigned char IIC_ReadBuf(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead);
static void IIC_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite);
static unsigned char IIC_ReadOneByte(unsigned short ReadAddr);
/**************************************************************************************
**************************************************************************************/
//�����е���ʱ��������ڲ���Ϊ0��65535
static void SW_EEPROM_Delay_nus(unsigned short nus)
{
	for(;nus>0;nus--)
	{__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();}
}
static void SW_EEPROM_Delay_nms(unsigned short nms)
{
	for(;nms>0;nms--)
	SW_EEPROM_Delay_nus(1000);
}
//���Ӳ����ʼ����ʼ��
void PCF8563_Init(void)
{
	 Init_SWIIC();
}

/*****************************************************************/
/*
********************************************************************/ 
//PCF8563ʱ��ָֹͣ��   i2c_write_n_bytes(unsigned char SLA_W, uint16 ADDR,uint16 N, unsigned char *DAT)
static unsigned char PCF8563_stop(void)
{
 	unsigned char stopcode=0x20;
 	if(0==IIC_WriteBuf(0,&stopcode,1))
		return 0;
 	else return 1;
}
//PCF8563ʱ������ָ��
static unsigned char PCF8563_start(void)
{
 	unsigned char startcode=0x00;
	if(0==IIC_WriteBuf(0,&startcode,1))
		return 0;
 	else return 1;
}
////�жϲ��������TF��־λ
unsigned char PCF8563_ClearINT(void)
{
 	unsigned char temp=0x12;
	if(0==IIC_WriteBuf(1,&temp,1))
		return 0;
 	else return 1;
}

/************************************************************************
PCF8563ʱ������
����˵������BCD�룩
yy:  �꣨0x00��0x99��
mm:	 �£�0x01��0x12��
dd:	 �գ�0x01��0x31��
hh:	 ʱ��0x00��0x23��
mi:	 �֣�0x00��0x59��
ss:	 �루0x00��0x59��
da:	 ���ڣ�0x01��0x07��
************************************************************************/
void Set_Sysdate(unsigned char * ptr)
{
	 unsigned char time[7];
	 time[6]=ptr[0];//��
	 time[5]=ptr[1];//��
	 //time[4]=ptr[2];//����
	 time[3]=ptr[2];//��
	 time[2]=ptr[3];//ʱ
	 time[1]=ptr[4];//��
	 time[0]=ptr[5];//��
	 PCF8563_stop();
	IIC_WriteBuf(2,time,7);
	
	 	PCF8563_start();	   
	
 	 
}

/************************************************************************
PCF8563ʱ�Ӷ�ȡ
����˵������BCD�룩
time:	  ��Ŷ���ʱ���������
time[6]:  �꣨0x00��0x99��
time[5]:  �£�0x01��0x12��
time[3]:  �գ�0x01��0x31��
time[2]:  ʱ��0x00��0x23��
time[1]:  �֣�0x00��0x59��
time[0]:  �루0x00��0x59��
time[4]:  ���ڣ�0x01��0x07��
************************************************************************/
void Read_Sysdate(unsigned char *ptr)
{
	 unsigned char time[7];
	 IIC_ReadBuf(2,time,7);

	ptr[0]=time[6]; //��
    ptr[1]=time[5]&0x1f; //��
    ptr[2]=time[3]&0x3f; //��
    ptr[3]=time[2]&0x3f; //ʱ
    ptr[4]=time[1]&0x7f; //��
	if (time[0]<0x80)
		bitSysTimeDisable=0;
	else
		bitSysTimeDisable=1;
    ptr[5]=time[0]&0x7f; //��
	ptr[6]=time[4]&7;
	  
}

uchar		DiagTimeString(uchar bbit,uchar * ptr)
{
	uchar	 aa;
	uchar 	 bb;
	uchar	 Max;
	uchar	MaxDay[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	if (!bbit)
	{
		if (BCD_String_Diag(ptr,3))
			return	1;	
		aa=BCDToHex(ptr[1]);
		if (!aa || aa>12)
			return	1;
		Max=MaxDay[aa];
		if (aa==2)
		{
			bb=BCDToHex(ptr[0]);
			if (!(bb%4))
				Max=29;	
		}
		aa=BCDToHex(ptr[2]);
		if (!aa || aa>Max)
			return	1;				
	}
	else
	{
		if (BCD_String_Diag(ptr,3))
			return	1;	
		aa=BCDToHex(ptr[0]);
		if ( aa>23)
			return	1;
		aa=BCDToHex(ptr[1]);
		if ( aa>59)
			return	1;
		aa=BCDToHex(ptr[2]);
		if ( aa>59)
			return	1;
	}
	return  0;
}

unsigned char		CheckPF8563(void)
{
	unsigned char i;
	uchar		bbit;
	for (i=0;i<5;i++)
	{
		Read_Sysdate(SysTimeDatas.TimeString);
		bbit=DiagTimeString(0,SysTimeDatas.TimeString);
		bbit=DiagTimeString(0,SysTimeDatas.TimeString);
		if (!bbit)
			bbit=DiagTimeString(1,SysTimeDatas.TimeString+3);
		if (!bbit)
		{
			if (!bitSysTimeDisable)
			{
				memcpy(SysTimeDatasBak.TimeString,SysTimeDatas.TimeString,6);
				return	0;
			}
		}
	}
	return	1;
}


/**********************************************************************
** PCF8563�жϹܽ��жϷ�����,���������д
*********************************************************************/

//��ʼ��ģ������
static void Init_SWIIC(void)
{
	SDA_OUT();				   
	CLK_OUT();
}
//����IIC��ʼ�ź�
static void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	SW_EEPROM_Delay_nus(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	SW_EEPROM_Delay_nus(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
static void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	SW_EEPROM_Delay_nus(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	SW_EEPROM_Delay_nus(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
static unsigned char IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA=1;SW_EEPROM_Delay_nus(1);	   
	IIC_SCL=1;SW_EEPROM_Delay_nus(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
static void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=1;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=1;
	SW_EEPROM_Delay_nus(2);
	IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
static void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		SW_EEPROM_Delay_nus(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		SW_EEPROM_Delay_nus(2); 
		IIC_SCL=0;	
		SW_EEPROM_Delay_nus(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
static unsigned char IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        SW_EEPROM_Delay_nus(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		SW_EEPROM_Delay_nus(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
} 
//��ָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
static unsigned char IIC_ReadOneByte(unsigned short ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
    IIC_Start();  
	IIC_Send_Byte(PCF8563_Write_Add);   //����������ַPCF8563_Read_Add,д���� 	 
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(PCF8563_Read_Add);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��ָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
static void IIC_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
    IIC_Start();  
	IIC_Send_Byte(PCF8563_Write_Add);   //����������ַPCF8563_Write_Add,д���� 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	SW_EEPROM_Delay_nms(10);	 
}
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
static unsigned char IIC_ReadBuf(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=IIC_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
	return 0;
}  
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
static unsigned char IIC_WriteBuf(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
	while(NumToWrite--)
	{
		IIC_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}		
	return 0;
}
