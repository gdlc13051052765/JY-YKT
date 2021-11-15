#include "char.h"
//extern	void	delay_ms(uint32_t);
extern	void	Delay_10us(uchar);
extern	void	BeepOn(uchar);
extern  void	BeepClose(void);
extern	void	JMP_Isp(void);
extern	uchar	DiagTimeString(uchar,uchar *);
extern	void	Clr_PosSub(uchar,uchar);

extern  void 	u485_GPIO_INT(void);
extern  void 	USART_COMSEL_GPIO_INIT(void);
extern  void 	COMSEL_485Mode(void);
extern  void  COMSEL_232Mode(void);
extern 	void 	SelWifiMode(void);
extern	void	Init_Serial(void);
extern	uchar	SerAPDU_Analyse(uchar );
extern	void	SerialSendChar (uchar );
extern	void	UART_ReceiveSub(void);
extern	void	UART_SendSub(void);
extern  void 	DK_USART_Init(ulong,uchar);
extern  void 	USART_Send_Char(unsigned char,unsigned char);
extern  void 	USART_PUTS_Arry(unsigned char USARTx,unsigned char *buffer,unsigned int len) ;
extern	void	TCP_Send_STX(void);
extern  uchar	Udp_Analyse(uchar )	;

//uip�¼���������
//���뽫�ú��������û���ѭ��,ѭ������.
extern  void uip_polling(void);

extern	void	SetDoubleMainCode(void);//���ô�Сվ���ͨѶЭ��
extern	void	SetConsumModeEnableSub(void);//�������ѷ�ʽ����λ
extern	void	LookSysPatameter(void);//�쿴ϵͳ����
extern	void	SetSysDateTime(ulong);//��������ʱ��
extern	void	SetConsumMode(ulong);//��������ģʽ
extern	void	SetSysPrince(uchar,ulong);//���õ���
extern	void	LookRecordDatas(ulong);	
extern	void	SetCommModeSub(void);

extern	void	SetPurseSectorSub(void);//������Ǯ���Ƿ���Ч
extern	void	SetSuperCardSub(void);

extern	void	ConsumSub(void);//����������

extern	uchar	SearchPurseBalnanceDatas(uchar,uchar,uchar *,uchar *);//����Ǯ��
extern	uchar	CurrentConsumMoneyDiag(uchar);//����������Ѷ����
extern	uchar	ReadCardBalanceDatas(uchar );//������Ӧ�����Ŀ����
extern	uchar	InputSumConsumMoney(uchar,ulong);//����ģʽ

extern	uchar	Mf500PiccRequest(uchar , uchar *);
extern	uchar	Mf500PiccAnticoll (uchar ,uchar *);
extern	uchar Mf500PiccCascAnticoll (uchar ,uchar ,uchar *);                     
extern	uchar	Mf500PiccCascSelect(uchar select_code, uchar *, uchar *); 

extern	uchar 	Mf500HostCodeKey(uchar *, uchar *); 
extern	uchar 	MFRC522_Auth(uchar ,uchar ,   uchar *,  uchar *);                     
extern	uchar 	MFRC522_Read(uchar ,uchar *);
extern	uchar 	MFRC522_Write(uchar ,uchar *);

extern	void	DISP_ErrorSub(uchar);
extern	void	DispMainCode(void);
extern	void	DispSumConsumMoney(uchar,uchar,ulong);
extern	void	Disp_Ver(uchar);//��ʾ�汾��

extern	void	ChgIntToDispBuffer(uint,uchar * );
extern	void	DISP_WriteError(uchar * );//��ʾд������
extern	void	DispOver(void);

extern  void  Disp_Gpio(void);
extern	void	Disp_Balance(uchar *);//��ʾ���
extern	void	DispDate(uchar * );//��ʾ����
extern	void	DispTime(uchar *);//��ʾʱ��
extern	void	ChgUlongToDispBuffer(ulong ,uchar *);
extern	void	Disp_Clr_Ram(void);//��ʾ���Pos;
extern	void	DispMinBalance(void);//��ʾ�׽�
extern  void	DispPutCard(void);
extern	void	ChgUlongToLongDispBuffer(ulong ,uchar *);

extern	ulong	ScanKeySub(ulong);
extern	ulong	ScanKeySub3(ulong);
extern	uchar	ChgKeyValueToChar(ulong);//����ֵת��Ϊ0-9
extern	void	ReadSysParameter(uchar);


extern	uchar	Check_SST_39SF040(void) ;   //Read ID Code
extern	void	Erase_Entire_Chip(void);  
extern	void	Erase_One_Sector(ulong);
extern	void	Flash_Write_Bytes( ulong,uchar * , unsigned short);//д�����ֽ�
extern	void	Flash_Rd_Bytes (ulong,uchar * ,unsigned short);//���ж�������ֽ�
extern void 	SPI_Flash_Text(void);
extern void 	SPI_Flash_Init(void);


extern	uchar	Check_24C64(uint);
extern	uchar	RdByte_24C256(uint);
extern	void	WrPage_24(uint,uchar);//ҳд��ͬ�����ֽ�64��
extern	void	WrByte_24C256(uint,uchar);
extern  void 	Clr_24C02(void);
extern  uchar	Check_BlackCardNum(ulong); 
extern 	void 	check_24C256(void);
extern void 	AT24C64_WriteOneByte(unsigned short ,unsigned char );

extern	void	Init_X25045(void);
extern	uchar	RdBytesFromAT24C64(uint,uchar  *,uint);   //�����������ֽ�
extern	uchar WrBytesToAT24C64(uint,uchar *,uint);    //д����ֽ�
extern	uchar	Check_25045(uint);
extern	void 	WrPageTo25045(uint,uchar);//ҳд����ͬ���ֽ�,ÿҳ4�ֽ�

extern	uchar	CheckPF8563(void);
extern	void 	Set_Sysdate(uchar *ptr);//����ϵͳ����
extern	void 	Read_Sysdate(uchar *ptr);//����ϵͳ����
extern  void	SetMainCodeSub(void);//�ֶ�����վ���

extern	uchar   BytesCheckSum(uchar *,uchar);//У��ͱȽ�
extern	uchar   BytesComp( uchar * ,uchar *,uchar);//�ַ����Ƚϣ���ȷ����0
extern	uchar		BCD_String_Diag(uchar * ,uchar);//BCD���ַ������
extern	uchar   BCDToHex(uchar  );
extern	uchar   HexToBCD(uchar  );
extern	ulong		ChgBCDStringToUlong(uchar * ,uchar);//BCD���ַ���ת��Ϊ������
extern	uchar		CalCheckSum(uchar * ,uchar);//����У��ͣ
extern	void		ChgUlongToBCDString( ulong ,uchar * ,uchar );
extern	void		ChgIntToStrings(uint	ii,uchar * ptr);
extern	ulong		ChgInputToUlong(uchar * ,uchar);//���������ת��Ϊ������
extern	ulong 	GetU32_HiLo(uchar *);
extern	uint 		GetU16_HiLo(uchar *);
extern	void 		PutU32_HiLo(uchar *,ulong);
extern	void 		PutU16_HiLo(uchar *,uint);
extern	void		FormatBuffer(uchar * ,uchar *);
extern uint16_t DoubleBigToSmall(uint16_t );
extern int32_t  FourBigToSmall(uint32_t a);
extern ulong		ChgStringsToInt(uchar * ptr);

extern	void	InitSaveRecordSub(uchar);//��ʼ��ָ��
extern	void	SaveRecordSub(uchar);//�洢���Ѽ�¼
extern	uchar	RecordDatasToBuffer(uchar,uchar *);//�洢��¼������д�뻺��
extern	uchar	RecordFullDiag(void);//��¼�洢�Ƿ��������

extern	uchar	Init_NetCard(void);
extern	uchar	TCPCardReceiveDiag(void);//TCP���������ݰ�����
extern	uint	TCPCard_SetNetParameter(void);// ������������ 	
extern	void	UART550_SendBytes(uchar * ,uchar );
extern	uchar UART550_RecByte(uint,uchar *);//zjx_change_201206
extern	void 	Init550_ISO7816 (uchar );
extern	void	UART550_SendByte(uchar);

extern	uint	Cal_CRC_Sub(uchar * , uint ); 

extern	void	ChgTimeToRecordDatas(uchar * ,uchar * );
extern	void	ChgRecordDatasToTime(uchar * ,uchar * );

extern  uchar check_RFCPU_ResetInfo(uchar *, void *);
extern	uchar	RequestCard(void);//����λ��Ϣ
extern	char 	ISO7816_TRANSCEIVE(unsigned char, void *, unsigned char *, void *);//7816����ͨѶ
extern	uint  IC7816_relay_station(uchar,uchar, void *, uchar *, void *);
extern	uint 	CPU_Select_File_AID(uchar, uchar * ,uchar *,uchar *);//AIDѡ���ļ�

extern	char 	ISO7816_PSAM(unsigned char , unsigned char *, unsigned char *, unsigned char *);
extern  uchar check_PSAM_ResetInfo(void);
extern 	char  CV522Pcd_ResetState(void);
//extern  void	PSAM_PowerDown(void);

extern	uchar ReadCardCommonDatas(void);
extern	uchar CPU_Get_Challenge(uchar *, uchar *);//�ն˴ӿ��϶�ȡ�����
extern	uint 	CPU_Verify_PIN(uchar,uchar,uchar,uchar *);
extern	uchar ConsumPocess(void);
extern	uchar CheckCardPrinterNum(void);

extern	uchar WriteCardLjdata_Cpu(uchar);
extern	uchar WriteCardLjdata(void);
extern	uchar	Sure_ConsumSucce(void);
extern	void	Disp_Hello(void);

extern	uchar PSAM_Init_Dir(void);
extern	void	SaveRecordBakSub(void);//��������¼�ı���

//zjx_change_20110905
extern	void 	LED_DispDatas(uchar bbit,uchar * ptr);
extern	void	ChgInputToLedBuffer(uchar LedX,uchar * ptr,uchar Num);
extern	ulong	ChgKeyStringToUlong(uchar * ,uchar);
extern	void	ReakKeySub(void);
extern	void 	LED_DispDatas_all(uchar *);
extern	void 	LED_OneByte_Disp(uchar,uchar);
extern	void 	LED_NumFalsh_Disp(uchar,uchar,unsigned long);
extern	void 	Led_Wait_Disp(void);
extern	void	Disp_AllOn(void);
extern	void	Disp_AllOff(void);
extern	void	DispConsumModeEnable(uchar ,uchar );
extern	void	SetNetDatas(void);
extern  void	DispNetDatas(uchar);

//zjx_add_20111014
extern	uchar	CardAuthKeySub(uchar,uchar); 
extern	uchar	ReadCardCommonDatas_M1 (void);//����������������
extern	uchar	ReadCard_DaySumConsumMoney_M1(void);//�����ۼ������Ѷ�
extern	uchar	ReadCardBalanceDatas_M1(uchar );//������Ӧ�����Ŀ����
extern	uchar	WriteBalanceToCard_M1(uchar,uchar);//д��
extern	void	CalPurseKeyCode(uchar *,uchar *,uchar *,uchar *);//����Ǯ����д������
extern	uchar	ReWriteCardSub_M1(uchar);

extern	uchar ReadCardCommonDatas_CPU(void);
extern	uchar	ReadCard_DaySumConsumMoney_CPU(void);//�����ۼ������Ѷ�
extern	uchar	ReadCardBalanceDatas_CPU(uchar PurseNum);//�����
extern	uchar ConsumPocess_Cpu(void);

extern	uchar	Detect_Card(void);

extern	void	CalEncryptDatas(uchar bbit,uchar * S_Datas ,uchar * KeyDatas ,uchar *  T_Datas ,uchar Len );
extern	void	FindPosConsumCountSub(void);//�������һ����¼����ˮ��
extern	uchar ReWriteCardSub_Cpu(void);
extern	uchar	ReadCard_DaySumConsumMoney(void);//�����ۼ������Ѷ�

extern uchar ReadMIF_Mode(void);

extern u8		Check_SPI_FLASH(void);//FLASHӲ���Լ캯����ID��
extern u8 	Check_24C64(uint16_t);
extern u8 	Check_24C256(uint16_t);
extern u8 	ENC28J60_Get_EREVID(void);
extern void Receive_Data_Double (void);
extern void Clr_WatchDog(void);
extern void	WatchDog_Gpio_init(void);

extern void	Clr_PosDatas(void);

extern unsigned char AT24CXX_Write(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite);
//extern unsigned char RdBytesFromAT24C64(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead) ;

extern void Usart2ToPrint(void);//����2�����ڴ�ӡ��ģʽ
extern void	JDQOn(void);
extern void	JDQClose(void);
extern void OpedDoor(void);
extern void ChangeLedBright(uchar );//�޸����������
 //��ȡоƬID
extern void GetLockCode(void);
extern void	ClrConsumDatasBak(void);//���������¼
extern uint	ClearSaveRecord(uchar);//ȡ���洢���Ѽ�¼

extern void SetRemoteIp(void) ;//�����Ƿ���Զ��IP
extern void UdpSend(void);
extern void SetBeepOn(void);
extern void	Beep_init_Close(void);
extern void UdpSendStx(void);
extern void Time_Update(void);
extern unsigned char AT24C64_ReadOneByte(unsigned short );
//�������ݶ�Ӧ���ۿ�ֵ
extern uchar Read_Rebate(void);
extern void AT24CXX_WriteSome(u16,u8 );
extern void WrByte_24C256_Some(unsigned short ,unsigned char );
extern void UdpSendChar(unsigned char *,unsigned int  );
extern void Text_24C256(void);
extern void Text_24C64(void);
extern void InitNvic(void);
extern void COMSEL_NoMode(void);
extern void udp_demo_connection_close(void);
extern void Load_Net_Parameters(void);
extern void W5500_Initialization(void);
extern void SetValue(void);
extern void PlayNum(u8 num);
extern void Usart2ToMP3(void);
extern void writeSysParameterToFlash(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);
uint16_t FLASH_ReadHalfWord(uint32_t address);
/****************/  