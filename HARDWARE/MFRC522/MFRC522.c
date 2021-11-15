#include "MFRC522.h"
#include "gpio.h"
#include "includes.h"
#include "MyDefine.h"
#include "char.h"
#include "ExternVariableDef.h"
#include "MoNiSPI.H"	

//�ܽ�����


//test
u8  irq_regdata;
u16 wait_count;
u8  error_regdata;
u8  last_bitsdata;

//MFRC522������
u8  mfrc552pidbuf[18];
u8  card_pydebuf[2];
//u8  card_key0Abuf[6]={0xA0,0xA1,0xA2,0xA3,0xA4,0xA5};
u8  card_key0Abuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};
u8  card_writebuf[16]={0x12,0x34,0x52,0x73,0x84,0xa5,0x36,0xf7,0xe8,0xa9,10,11,12,13,14,15};
u8  card_readbuf[18];
//
void Delay1_us(u16 count)
{
  u16 i;
  
  for(i=0;i<count;i++)
  {
		u8 us=12;
		
		while(us--);
  }
}

//
void SPI1_Send(u8 val)  
{ 
RC522_WriteByte(val);
}
//
u8 SPI1_Receive(void)  
{ 
 u8 temp; 
	temp = RC522_ReadByte();
  return temp;  
}
//������������MFRC522��ĳһ�Ĵ���дһ���ֽ�����
//���������addr--�Ĵ�����ַ��val--Ҫд���ֵ
void Write_MFRC522(u8 addr, u8 val) 
{
	//��ַ��ʽ��0XXXXXX0  
RC522_XCS_L();  
	SPI1_Send((addr<<1)&0x7E);  
	SPI1_Send(val);    
	RC522_XCS_H() ;

}
//������������MFRC522��ĳһ�Ĵ�����һ���ֽ�����
//���������addr--�Ĵ�����ַ
//�� �� ֵ�����ض�ȡ����һ���ֽ����� 
u8 Read_MFRC522(u8 addr) 
{  
	u8 val;
	//��ַ��ʽ��1XXXXXX0   
	RC522_XCS_L();     
	SPI1_Send(((addr<<1)&0x7E)|0x80);   
	val=SPI1_Receive();    
	RC522_XCS_H() ;
	//   
	return val;  

}
//������������ֻ���ܶ�дλ��Ч
//������������RC522�Ĵ���λ
//���������reg--�Ĵ�����ַ;mask--��λֵ
void SetBitMask(u8 reg, u8 mask)   
{     
	u8 tmp=0;  
	tmp=Read_MFRC522(reg);     
	Write_MFRC522(reg,tmp|mask);  // set bit mask 
}
//������������RC522�Ĵ���λ
//���������reg--�Ĵ�����ַ;mask--��λֵ
void ClearBitMask(u8 reg, u8 mask)   
{     
	u8 tmp=0;
	//     
	tmp=Read_MFRC522(reg);     
	Write_MFRC522(reg,tmp&(~mask));  //clear bit mask 
}
//������������������,ÿ��������ر����߷���֮��Ӧ������1ms�ļ��
void AntennaOn(void) 
{  
	u8 temp;
	//   
	temp=Read_MFRC522(TxControlReg);  
	if ((temp&0x03)==0)  
	{   
		SetBitMask(TxControlReg,0x03);  
	}
}
//�����������ر�����,ÿ��������ر����߷���֮��Ӧ������1ms�ļ��
void AntennaOff(void) 
{  
	ClearBitMask(TxControlReg,0x03);
}
//������������λMFRC522
void MFRC522_Reset(void) 
{ 	
	//�⸴λ���Բ���
	RC522_XRESET_H();
	Delay1_us(10);
	RC522_XRESET_L();
	Delay1_us(10);
	RC522_XRESET_H();
	Delay1_us(10); 

	Write_MFRC522(0x01,0x0F); //soft reset
  while(Read_MFRC522(0x27) != 0x88); //wait chip start ok	
	//�ڸ�λ   
	Write_MFRC522(CommandReg, PCD_RESETPHASE); 
	Write_MFRC522(TModeReg,0x8D);

}
//
void MFRC522_Initializtion(void) 
{	
	 
	MFRC522_Reset();         
	//Timer: TPrescaler*TreloadVal/6.78MHz = 0xD3E*0x32/6.78=25ms     
	Write_MFRC522(TModeReg,0x8D);				//TAuto=1Ϊ�Զ�����ģʽ����ͨ��Э��Ӱ�򡣵�4λΪԤ��Ƶֵ�ĸ�4λ
	//aa = Read_MFRC522(TModeReg);
	//Write_MFRC522(TModeReg,0x1D);				//TAutoRestart=1Ϊ�Զ����ؼ�ʱ��0x0D3E��0.5ms�Ķ�ʱ��ֵ//test    
	Write_MFRC522(TPrescalerReg,0x3E); 	//Ԥ��Ƶֵ�ĵ�8λ     
	Write_MFRC522(TReloadRegL,0x32);		//�������ĵ�8λ                
	Write_MFRC522(TReloadRegH,0x00);		//�������ĸ�8λ       
	Write_MFRC522(TxAutoReg,0x40); 			//100%ASK     
	Write_MFRC522(ModeReg,0x3D); 				//CRC��ʼֵ0x6363
	Write_MFRC522(CommandReg,0x00);			//����MFRC522  
	//Write_MFRC522(RFCfgReg, 0x7F);    //RxGain = 48dB���ڿ���Ӧ����      
	AntennaOn();          							//������ 
}
//����������RC522��ISO14443��ͨѶ
//���������command--MF522������
//					sendData--ͨ��RC522���͵���Ƭ������
//					sendLen--���͵����ݳ���
//					BackData--���յ��Ŀ�Ƭ��������
//					BackLen--�������ݵ�λ����
//�� �� ֵ���ɹ�����MI_O
u8 MFRC522_ToCard(u8 command, u8 *sendData, u8 sendLen, u8 *backData, u16 *backLen) 
{
	u8  status=MI_ERR;
	u8  irqEn=0x00;
	u8  waitIRq=0x00;
	u8  lastBits;
	u8  n;
	u16 i;
	//������Ԥ���жϲ���
	switch (command)     
	{         
		case PCD_AUTHENT:  		//��֤����   
			irqEn 	= 0x12;			//    
			waitIRq = 0x10;			//    
			break;
		case PCD_TRANSCEIVE: 	//����FIFO������      
			irqEn 	= 0x77;			//    
			waitIRq = 0x30;			//    
			break;      
		default:    
			break;     
	}
	//
	Write_MFRC522(ComIEnReg, irqEn|0x80);		//�����ж�����     
	ClearBitMask(ComIrqReg, 0x80);  				//��������ж�����λ               	
	SetBitMask(FIFOLevelReg, 0x80);  				//FlushBuffer=1, FIFO��ʼ��
	Write_MFRC522(CommandReg, PCD_IDLE); 		//ʹMFRC522����   
	//��FIFO��д������     
	for (i=0; i<sendLen; i++)
		Write_MFRC522(FIFODataReg, sendData[i]);
	//ִ������
	Write_MFRC522(CommandReg, command);
	//���߷�������     
	if (command == PCD_TRANSCEIVE)					//����ǿ�Ƭͨ�����MFRC522��ʼ�����߷�������      
		SetBitMask(BitFramingReg, 0x80);  		//StartSend=1,transmission of data starts      
	//�ȴ������������     
	i = 1000; //i����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms     
	do      
	{  
		//OSTimeDlyHMSM(0,0,0,1);		
		n = Read_MFRC522(ComIrqReg); 
		//OSTimeDlyHMSM(0,0,0,1);			
		i--;		     
	}while ((i!=0) && !(n&0x01) && !(n&waitIRq));	//��������˳�n=0x64
	//ֹͣ����
	ClearBitMask(BitFramingReg, 0x80);   		//StartSend=0
	//�����25ms�ڶ�����
	if (i != 0)     
	{            
		if(!(Read_MFRC522(ErrorReg) & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr         
		{            
			if (n & irqEn & 0x01)			//                  
				status = MI_NOTAGERR;		//
			//
			if (command == PCD_TRANSCEIVE)             
			{                 
				n = Read_MFRC522(FIFOLevelReg);		//n=0x02                
				lastBits = Read_MFRC522(ControlReg) & 0x07;	//lastBits=0               
				if (lastBits!=0)                         
					*backLen = (n-1)*8 + lastBits; 
				else
					*backLen = n*8;									//backLen=0x10=16
				//
				if (n == 0)                         
				 	n = 1;                        
				if (n > MAX_LEN)         
				 	n = MAX_LEN;
				//
				for (i=0; i<n; i++)                 
					backData[i] = Read_MFRC522(FIFODataReg); 
			}
			//
			status = MI_OK;		
		}
		else
			status = MI_ERR;
	}	
	//
	Write_MFRC522(ControlReg,0x80);				//timer stops     
	Write_MFRC522(CommandReg, PCD_IDLE);	//
	//
	//
	return status;
}
//����������Ѱ������ȡ�����ͺ�
//���������reqMode--Ѱ����ʽ
//					TagType--���ؿ�Ƭ����
//					0x4400 = Mifare_UltraLight
//					0x0400 = Mifare_One(S50)
//					0x0200 = Mifare_One(S70)
//					0x0800 = Mifare_Pro(X)
//					0x4403 = Mifare_DESFire
//�� �� ֵ���ɹ�����MI_OK	
u8 Mf500PiccRequest(u8 reqMode, u8 *TagType)
{  
	u8  status;    
	u16 backBits;   //���յ�������λ��
	//   
	Write_MFRC522(BitFramingReg, 0x07);  //TxLastBists = BitFramingReg[2..0]   
	TagType[0] = reqMode;  
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits); 
	// 
	if ((status != MI_OK) || (backBits != 0x10))  
	{       
		status = MI_ERR;
	}
	//  
	return status; 
}
//��������������ͻ��⪡��ȡѡ�п�Ƭ�Ŀ����к�
//���������serNum--����4�ֽڿ����к�,��5�ֽ�ΪУ���ֽ�
//�� �� ֵ���ɹ�����MI_OK
u8 MFRC522_Anticoll(u8 *serNum) 
{     
	u8  status;     
	u8  i;     
	u8  serNumCheck=0;     
	u16 unLen;
	u8  CardUID[8];
	//           
	ClearBitMask(Status2Reg, 0x08);  			//TempSensclear     
	ClearBitMask(CollReg,0x80);   				//ValuesAfterColl  
	Write_MFRC522(BitFramingReg, 0x00);  	//TxLastBists = BitFramingReg[2..0]
	serNum[0] = PICC_ANTICOLL1;     
	serNum[1] = 0x20;     
	status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, CardUID, &unLen);
	//      
	if (status == MI_OK)
	{   
		//У�鿨���к�   
		for(i=0;i<4;i++)   
			serNumCheck^=CardUID[i];
		//
		if(serNumCheck!=CardUID[i])        
			status=MI_ERR;
		memcpy(serNum,CardUID,4);
	}
	SetBitMask(CollReg,0x80);  //ValuesAfterColl=1
	//      
	return status;
}
//������������MF522����CRC
//���������pIndata--Ҫ����CRC�����ݪ�len--���ݳ��Ȫ�pOutData--�����CRC���
void CalulateCRC(u8 *pIndata, u8 len, u8 *pOutData) 
{     
	u16 i;
	u8  n;
	//      
	ClearBitMask(DivIrqReg, 0x04);   			//CRCIrq = 0     
	SetBitMask(FIFOLevelReg, 0x80);   		//��FIFOָ��     
	Write_MFRC522(CommandReg, PCD_IDLE);   
	//��FIFO��д������      
	for (i=0; i<len; i++)
		Write_MFRC522(FIFODataReg, *(pIndata+i));
	//��ʼRCR����
	Write_MFRC522(CommandReg, PCD_CALCCRC);
	//�ȴ�CRC�������     
	i = 1000;     
	do      
	{         
		n = Read_MFRC522(DivIrqReg);         
		i--;   
	}while ((i!=0) && !(n&0x04));   //CRCIrq = 1
	//��ȡCRC������     
	pOutData[0] = Read_MFRC522(CRCResultRegL);     
	pOutData[1] = Read_MFRC522(CRCResultRegH);
	Write_MFRC522(CommandReg, PCD_IDLE);
}
//����������ѡ������ȡ���洢������
//���������serNum--���뿨���к�
//�� �� ֵ���ɹ����ؿ�����
u8 MFRC522_SelectTag(u8 *serNum) 
{     
	u8  i;     
	u8  status;     
	volatile u8  size;     
	u16 recvBits;     
	u8  buffer[9];
	//     
	buffer[0] = PICC_ANTICOLL1;	//��ײ��1     
	buffer[1] = 0x70;
	buffer[6] = 0x00;						     
	for (i=0; i<4; i++)					
	{
		buffer[i+2] = *(serNum+i);	//buffer[2]-buffer[5]Ϊ�����к�
		buffer[6]  ^=	*(serNum+i);	//��У����
	}
	//
	CalulateCRC(buffer, 7, &buffer[7]);	//buffer[7]-buffer[8]ΪRCRУ����
	ClearBitMask(Status2Reg,0x08);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
	//
	if ((status == MI_OK) && (recvBits == 0x18))    
		size = buffer[0];     
	else    
		size = 0;
	//	     
	return status; 
}
//������������֤��Ƭ����
//���������authMode--������֤ģʽ
//					0x60 = ��֤A��Կ
//					0x61 = ��֤B��Կ
//					BlockAddr--���ַ
//					Sectorkey--��������
//					serNum--��Ƭ���кŪ�4�ֽ�
//�� �� ֵ���ɹ�����MI_OK
u8 MFRC522_Auth(u8 authMode, u8 BlockAddr, u8 *Sectorkey, u8 *serNum) 
{     
	u8  status;     
	u16 recvBits;     
	u8  i;  
	u8  buff[12];    
	//��֤ģʽ+���ַ+��������+�����к�     
	buff[0] = authMode;		//��֤ģʽ     
	buff[1] = BlockAddr;	//���ַ     
	for (i=0; i<6; i++)
		buff[i+2] = *(Sectorkey+i);	//��������
	//
	for (i=0; i<4; i++)
		buff[i+8] = *(serNum+i);		//�����к�
	//
	status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
	//      
	if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
		status = MI_ERR;
	//
	return status;
}
//������������������
//���������blockAddr--���ַ;recvData--�����Ŀ�����
//�� �� ֵ���ɹ�����MI_OK
u8 MFRC522_Read(u8 blockAddr, u8 *recvData) 
{     
	u8  status;     
	u16 unLen;
	//      
	recvData[0] = PICC_READ;     
	recvData[1] = blockAddr;     
	CalulateCRC(recvData,2, &recvData[2]);     
	status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
	//
	if ((status != MI_OK) || (unLen != 0x90))
		status = MI_ERR;
	//
	return status;
}
//����������д������
//���������blockAddr--���ַ;writeData--���д16�ֽ�����
//�� �� ֵ���ɹ�����MI_OK
u8 MFRC522_Write(u8 blockAddr, u8 *writeData) 
{     
	u8  status;     
	u16 recvBits;     
	u8  i;  
	u8  buff[18];
	//           
	buff[0] = PICC_WRITE;     
	buff[1] = blockAddr;     
	CalulateCRC(buff, 2, &buff[2]);     
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
	//
	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
		status = MI_ERR;
	//
	if (status == MI_OK)     
	{         
		for (i=0; i<16; i++)  //��FIFOд16Byte����                     
			buff[i] = *(writeData+i);
		//                     
		CalulateCRC(buff, 16, &buff[16]);         
		status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);           
		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))               
			status = MI_ERR;         
	}          
	return status;
}
//�������������Ƭ��������״̬
void MFRC522_Halt(void) 
{    
	u16 unLen;     
	u8  buff[4];
	//       
	buff[0] = PICC_HALT;     
	buff[1] = 0;     
	CalulateCRC(buff, 2, &buff[2]);       
	MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}
//


///////////////////////////////////////////////////////////////////////
//                      C O D E   K E Y S
///////////////////////////////////////////////////////////////////////
u8 Mf500HostCodeKey(  unsigned char * uncoded, unsigned char *	coded)   
{
//	uchar   status = MI_OK;
	u8   cnt = 0;
	u8   ln  = 0;     // low nibble
	u8   hn  = 0;     // high nibble
   	for (cnt = 0; cnt < 6; cnt++)
   	{
		ln = uncoded[cnt] & 0x0F;
		hn = uncoded[cnt] >> 4;
		coded[cnt * 2 + 1]     =  (~ln << 4) | ln;
		coded[cnt * 2 ] =  (~hn << 4) | hn;
  	}
   	return MI_OK;
}

/*------------------------------------------------
���RF CPU����λ��Ϣ
type_A Ѱ��
------------------------------------------------*/
unsigned char check_RFCPU_ResetInfo(unsigned char * RcvLen, void * Rcvdata)
{
	unsigned char i;
	unsigned char status;
	
	MFRC522_Initializtion();
	delay_ms(10);
	for (i=0;i<3;i++)
	{	TypeA_Sort = 0xff;
		status=Mf500PiccRequest(0x52,CardType);
	//	status=Mf500PiccRequest(0x52,CardType);
		if (!status)
		{
			if ((CardType[0]==0x04 || CardType[0]==0x02)&& !CardType[1])//04-s50;02-s70
				TypeA_Sort=0;//M1��
			else if (CardType[0]==0x08 && !CardType[1])//CPU
				TypeA_Sort=1;//CPU��
			else
				return CARD_NOCARD;

			status=MFRC522_Anticoll(CardSerialNum);	//��ײ����	
			if (!status)
			{
				status=MFRC522_SelectTag(CardSerialNum);	//ѡ��
				if (!status)
					break;
			}	
		}
		if(status)
		{
			status = status;
			MFRC522_Initializtion();
	delay_ms(10);
			
		}
	}

	if (status)//�޿�
		return CARD_NOCARD; 
	//�п�
	if (!TypeA_Sort)//04-s50;02-s70
		return	CARD_OK;
}

//MFRC522���Ժ���
void MFRC522Test(void)
{
	volatile u8 status;
	//
	status = 0xff;
	status=Mf500PiccRequest(0x52, card_pydebuf);			//Ѱ��
	//
//	if(status==0)		//���������
//	{
//		//BeepOn();
//		status=MFRC522_Anticoll(CardSerialNum);			//��ײ����			
//		status=MFRC522_SelectTag(CardSerialNum);	//ѡ��
//		status=MFRC522_Auth(0x60, 4, card_key0Abuf, CardSerialNum);	//�鿨
//	//		status=MFRC522_Auth(0x60, 4, card_key0Abuf, CardSerialNum);	//�鿨
//	//	status=MFRC522_Write(4, card_writebuf);				//д����д��ҪС�ģ��ر��Ǹ����Ŀ�3��
//	//	status=MFRC522_Read(4, card_readbuf);					//����		
//		MFRC522_Halt();															//ʹ����������״̬

//		//�����к��ԣ����һ�ֽ�Ϊ����У����
//	
//	}	
}
//522�Լ�ĺ����ɹ�����0��ʧ�ܷ���1
char CV522Pcd_ResetState(void)
{
	char aa;
	  
	MFRC522_Reset();         
	//Timer: TPrescaler*TreloadVal/6.78MHz = 0xD3E*0x32/6.78=25ms     
	Write_MFRC522(TModeReg,0x8D);				//TAuto=1Ϊ�Զ�����ģʽ����ͨ��Э��Ӱ�򡣵�4λΪԤ��Ƶֵ�ĸ�4λ
	aa = Read_MFRC522(TModeReg);
	if(aa == 0x8d)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}