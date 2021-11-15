//
#ifndef _MFRC522_H_
#define _MFRC522_H_
#include "stm32f10x.h"
//
/////////////////////////////////////////////////////////////////////
//MF522������
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

/////////////////////////////////////////////////////////////////////
//Mifare_One��Ƭ������
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����

/////////////////////////////////////////////////////////////////////
//MF522 FIFO���ȶ���
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//MF522�Ĵ�������
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01   //����ֹͣ 
#define     ComIEnReg             0x02    // ʹ�ܺͽ����ж�����
#define     DivlEnReg             0x03    //ʹ�ܺͽ����ж�����
#define     ComIrqReg             0x04    // �ж�����
#define     DivIrqReg             0x05	  //�ж�����
#define     ErrorReg              0x06    //��ʾ��һ��ָ��ִ�еĴ���״̬
#define     Status1Reg            0x07    //ͨ��״̬
#define     Status2Reg            0x08    //���պͷ���״̬
#define     FIFODataReg           0x09	  // 64�ֽ�FIFO����������������
#define     FIFOLevelReg          0x0A	  //FIFO���������Ѵ洢�ֽڵ�����
#define     WaterLevelReg         0x0B	  //FIFO����������Ϳվ���
#define     ControlReg            0x0C	  //���ֿ��ƼĴ���
#define     BitFramingReg         0x0D	 //����λ��֡�ĵ���
#define     CollReg               0x0E	 //������λ��ͻ�ĵ�һ��λ�ĵ�ַ
#define     RFU0F                 0x0F	  //
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11 //���ͺͽ��յ�ͨ��ģʽ������
#define     TxModeReg             0x12 //���ݷ���ʱ�Ĵ�������
#define     RxModeReg             0x13 //���ݽ���ʱ�Ĵ������� 
#define     TxControlReg          0x14 // ��������������TX1��TX2�Ŀ���
#define     TxAutoReg             0x15 //���͵��Ƶ�����
#define     TxSelReg              0x16	//ѡ�������������ڲ��ź�Դ
#define     RxSelReg              0x17	 //�ڲ������������� 
#define     RxThresholdReg        0x18	// λ��������ֵ��ѡ��
#define     DemodReg              0x19	// �����������
#define     RFU1A                 0x1A	 //
#define     RFU1B                 0x1B
#define     RFU1C             	  0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F //
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegH         0x21 //CRC�����MSBλ��LSBλ��ֵ
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24 //���ƿ�ȵ�����
#define     RFU25                 0x25 //
#define     RFCfgReg              0x26	//�������������
#define     GsNReg                0x27	//ѡ���������������� TX1 ��TX2�ڵ���ʱ�ĵ絼ֵ
#define     CWGsCfgReg            0x28	//���� p-driver �޵��Ƶ�����絼
#define     ModGsCfgReg           0x29	//���� p-driver �������Ƶ�����絼
#define     TModeReg              0x2A // �ڲ���ʱ��������
#define     TPrescalerReg         0x2B //
#define     TReloadRegH           0x2C	//16λ��ʱ��������ֵ
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E	//��ʾ16��ʱ���ĵ�ǰֵ
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31	  //�����źŵ�����
#define     TestSel2Reg           0x32	  //�����źŵ����ú� PRBS����
#define     TestPinEnReg          0x33	  //ʹ�� D1 �� D7����������� 
#define     TestPinValueReg       0x34	 //��������DI��D7��������I/O��ʱ�ĵ�ƽֵ
#define     TestBusReg            0x35	//��ʾ�ڲ��������ߵ�״̬
#define     AutoTestReg           0x36	//�����Լ�Ŀ���
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  		  0x3F

/////////////////////////////////////////////////////////////////////
//��MF522ͨѶʱ���صĴ������
/////////////////////////////////////////////////////////////////////
#define MI_OK                     0
#define MI_NOTAGERR               1
#define MI_ERR                    2
//
#define MAX_LEN					  18
//MFRC522 test
extern u8  irq_regdata;
extern u16 wait_count;
extern u8  error_regdata;
extern u8  last_bitsdata;
//
void Delay1_us(vu16 count);
//void STM32_SPI2_Init(void);
void RC522_SPI1_Init(void);
void SPI2_Send(u8 val);
u8 SPI2_Receive(void);
void SPI1_Send(u8 val);
u8 SPI1_Receive(void);
void MFRC522_Initializtion(void);
void Write_MFRC522(u8 addr, u8 val);
u8 Read_MFRC522(u8 addr);
void SetBitMask(u8 reg, u8 mask);
void ClearBitMask(u8 reg, u8 mask);
void AntennaOn(void);
void AntennaOff(void);
void MFRC522_Reset(void);
void MFRC522_Init(void);
u8 MFRC522_ToCard(u8 command, u8 *sendData, u8 sendLen, u8 *backData, u16 *backLen);
u8 MFRC522_Request(u8 reqMode, u8 *TagType);
u8 MFRC522_Anticoll(u8 *serNum);
void CalulateCRC(u8 *pIndata, u8 len, u8 *pOutData);
u8 MFRC522_SelectTag(u8 *serNum);
//u8 MFRC522_Auth(u8 authMode, unsigned char BlockAddr, unsigned char *Sectorkey, unsigned char *serNum);
u8 MFRC522_Read(u8 blockAddr, u8 *recvData);
u8 MFRC522_Write(u8 blockAddr, u8 *writeData);
void MFRC522_Halt(void); 
void MFRC522_Initializtion(void) ;
void MFRC522Test(void);
//
#endif
