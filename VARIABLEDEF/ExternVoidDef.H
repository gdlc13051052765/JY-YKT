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

//uipÊÂ¼ş´¦Àíº¯Êı
//±ØĞë½«¸Ãº¯Êı²åÈëÓÃ»§Ö÷Ñ­»·,Ñ­»·µ÷ÓÃ.
extern  void uip_polling(void);

extern	void	SetDoubleMainCode(void);//ÉèÖÃ´óĞ¡Õ¾µãºÅÍ¨Ñ¶Ğ­Òé
extern	void	SetConsumModeEnableSub(void);//ÉèÖÃÏû·Ñ·½Ê½ÔÊĞíÎ»
extern	void	LookSysPatameter(void);//²ì¿´ÏµÍ³²ÎÊı
extern	void	SetSysDateTime(ulong);//ÉèÖÃÈÕÆÚÊ±¼ä
extern	void	SetConsumMode(ulong);//ÉèÖÃÏû·ÑÄ£Ê½
extern	void	SetSysPrince(uchar,ulong);//ÉèÖÃµ¥¼Û
extern	void	LookRecordDatas(ulong);	
extern	void	SetCommModeSub(void);

extern	void	SetPurseSectorSub(void);//ÉèÖÃ×ÓÇ®°üÊÇ·ñÓĞĞ§
extern	void	SetSuperCardSub(void);

extern	void	ConsumSub(void);//Ïû·ÑÖ÷³ÌĞò

extern	uchar	SearchPurseBalnanceDatas(uchar,uchar,uchar *,uchar *);//²éÕÒÇ®°ü
extern	uchar	CurrentConsumMoneyDiag(uchar);//¿¨ÖÖÀàºÍÏû·Ñ¶îÕï¶Ï
extern	uchar	ReadCardBalanceDatas(uchar );//¶Á³ö¶ÔÓ¦ÉÈÇøµÄ¿¨Óà¶î
extern	uchar	InputSumConsumMoney(uchar,ulong);//Ïû·ÑÄ£Ê½

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
extern	void	Disp_Ver(uchar);//ÏÔÊ¾°æ±¾ºÅ

extern	void	ChgIntToDispBuffer(uint,uchar * );
extern	void	DISP_WriteError(uchar * );//ÏÔÊ¾Ğ´¿¨´íÎó
extern	void	DispOver(void);

extern  void  Disp_Gpio(void);
extern	void	Disp_Balance(uchar *);//ÏÔÊ¾Óà¶î
extern	void	DispDate(uchar * );//ÏÔÊ¾ÈÕÆÚ
extern	void	DispTime(uchar *);//ÏÔÊ¾Ê±¼ä
extern	void	ChgUlongToDispBuffer(ulong ,uchar *);
extern	void	Disp_Clr_Ram(void);//ÏÔÊ¾Çå¿ÕPos;
extern	void	DispMinBalance(void);//ÏÔÊ¾µ×½ğ
extern  void	DispPutCard(void);
extern	void	ChgUlongToLongDispBuffer(ulong ,uchar *);

extern	ulong	ScanKeySub(ulong);
extern	ulong	ScanKeySub3(ulong);
extern	uchar	ChgKeyValueToChar(ulong);//½«¼üÖµ×ª»»Îª0-9
extern	void	ReadSysParameter(uchar);


extern	uchar	Check_SST_39SF040(void) ;   //Read ID Code
extern	void	Erase_Entire_Chip(void);  
extern	void	Erase_One_Sector(ulong);
extern	void	Flash_Write_Bytes( ulong,uchar * , unsigned short);//Ğ´Èë¶à¸ö×Ö½Ú
extern	void	Flash_Rd_Bytes (ulong,uchar * ,unsigned short);//´ÓÖĞ¶Á³ö¶à¸ö×Ö½Ú
extern void 	SPI_Flash_Text(void);
extern void 	SPI_Flash_Init(void);


extern	uchar	Check_24C64(uint);
extern	uchar	RdByte_24C256(uint);
extern	void	WrPage_24(uint,uchar);//Ò³Ğ´ÈëÍ¬ÑùµÄ×Ö½Ú64¸ö
extern	void	WrByte_24C256(uint,uchar);
extern  void 	Clr_24C02(void);
extern  uchar	Check_BlackCardNum(ulong); 
extern 	void 	check_24C256(void);
extern void 	AT24C64_WriteOneByte(unsigned short ,unsigned char );

extern	void	Init_X25045(void);
extern	uchar	RdBytesFromAT24C64(uint,uchar  *,uint);   //Á¬Ğø¶Á³ö¶à×Ö½Ú
extern	uchar WrBytesToAT24C64(uint,uchar *,uint);    //Ğ´Èë¶à×Ö½Ú
extern	uchar	Check_25045(uint);
extern	void 	WrPageTo25045(uint,uchar);//Ò³Ğ´ÈëÏàÍ¬µÄ×Ö½Ú,Ã¿Ò³4×Ö½Ú

extern	uchar	CheckPF8563(void);
extern	void 	Set_Sysdate(uchar *ptr);//ÉèÖÃÏµÍ³ÈÕÆÚ
extern	void 	Read_Sysdate(uchar *ptr);//¶Á³öÏµÍ³ÈÕÆÚ
extern  void	SetMainCodeSub(void);//ÊÖ¶¯ÉèÖÃÕ¾µãºÅ

extern	uchar   BytesCheckSum(uchar *,uchar);//Ğ£ÑéºÍ±È½Ï
extern	uchar   BytesComp( uchar * ,uchar *,uchar);//×Ö·û´®±È½Ï£¬ÕıÈ··µ»Ø0
extern	uchar		BCD_String_Diag(uchar * ,uchar);//BCDÂë×Ö·û´®Õï¶Ï
extern	uchar   BCDToHex(uchar  );
extern	uchar   HexToBCD(uchar  );
extern	ulong		ChgBCDStringToUlong(uchar * ,uchar);//BCDÂë×Ö·û´®×ª»»ÎªÕûĞÍÊı
extern	uchar		CalCheckSum(uchar * ,uchar);//¼ÆËãĞ£ÑéºÍ£
extern	void		ChgUlongToBCDString( ulong ,uchar * ,uchar );
extern	void		ChgIntToStrings(uint	ii,uchar * ptr);
extern	ulong		ChgInputToUlong(uchar * ,uchar);//ÊäÈëµÄÊı×Ö×ª»»Îª³¤ÕûĞÎ
extern	ulong 	GetU32_HiLo(uchar *);
extern	uint 		GetU16_HiLo(uchar *);
extern	void 		PutU32_HiLo(uchar *,ulong);
extern	void 		PutU16_HiLo(uchar *,uint);
extern	void		FormatBuffer(uchar * ,uchar *);
extern uint16_t DoubleBigToSmall(uint16_t );
extern int32_t  FourBigToSmall(uint32_t a);
extern ulong		ChgStringsToInt(uchar * ptr);

extern	void	InitSaveRecordSub(uchar);//³õÊ¼»¯Ö¸Õë
extern	void	SaveRecordSub(uchar);//´æ´¢Ïû·Ñ¼ÇÂ¼
extern	uchar	RecordDatasToBuffer(uchar,uchar *);//´æ´¢¼ÇÂ¼µÄÄÚÈİĞ´Èë»º´æ
extern	uchar	RecordFullDiag(void);//¼ÇÂ¼´æ´¢ÊÇ·ñÒÑÂúÕï¶Ï

extern	uchar	Init_NetCard(void);
extern	uchar	TCPCardReceiveDiag(void);//TCP¿¨½ÓÊÕÊı¾İ°üÕûÀí
extern	uint	TCPCard_SetNetParameter(void);// ¶Á³öÍø¿¨²ÎÊı 	
extern	void	UART550_SendBytes(uchar * ,uchar );
extern	uchar UART550_RecByte(uint,uchar *);//zjx_change_201206
extern	void 	Init550_ISO7816 (uchar );
extern	void	UART550_SendByte(uchar);

extern	uint	Cal_CRC_Sub(uchar * , uint ); 

extern	void	ChgTimeToRecordDatas(uchar * ,uchar * );
extern	void	ChgRecordDatasToTime(uchar * ,uchar * );

extern  uchar check_RFCPU_ResetInfo(uchar *, void *);
extern	uchar	RequestCard(void);//¿¨¸´Î»ĞÅÏ¢
extern	char 	ISO7816_TRANSCEIVE(unsigned char, void *, unsigned char *, void *);//7816Êı¾İÍ¨Ñ¶
extern	uint  IC7816_relay_station(uchar,uchar, void *, uchar *, void *);
extern	uint 	CPU_Select_File_AID(uchar, uchar * ,uchar *,uchar *);//AIDÑ¡ÔñÎÄ¼ş

extern	char 	ISO7816_PSAM(unsigned char , unsigned char *, unsigned char *, unsigned char *);
extern  uchar check_PSAM_ResetInfo(void);
extern 	char  CV522Pcd_ResetState(void);
//extern  void	PSAM_PowerDown(void);

extern	uchar ReadCardCommonDatas(void);
extern	uchar CPU_Get_Challenge(uchar *, uchar *);//ÖÕ¶Ë´Ó¿¨ÉÏ¶ÁÈ¡Ëæ»úÊı
extern	uint 	CPU_Verify_PIN(uchar,uchar,uchar,uchar *);
extern	uchar ConsumPocess(void);
extern	uchar CheckCardPrinterNum(void);

extern	uchar WriteCardLjdata_Cpu(uchar);
extern	uchar WriteCardLjdata(void);
extern	uchar	Sure_ConsumSucce(void);
extern	void	Disp_Hello(void);

extern	uchar PSAM_Init_Dir(void);
extern	void	SaveRecordBakSub(void);//±£´æ´íÎó¼ÇÂ¼µÄ±¸·İ

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
extern	uchar	ReadCardCommonDatas_M1 (void);//¶Á³ö¹«¹²ÇøµÄÊı¾İ
extern	uchar	ReadCard_DaySumConsumMoney_M1(void);//¶Á³öÀÛ¼ÆÈÕÏû·Ñ¶î
extern	uchar	ReadCardBalanceDatas_M1(uchar );//¶Á³ö¶ÔÓ¦ÉÈÇøµÄ¿¨Óà¶î
extern	uchar	WriteBalanceToCard_M1(uchar,uchar);//Ğ´¿¨
extern	void	CalPurseKeyCode(uchar *,uchar *,uchar *,uchar *);//¼ÆËãÇ®°üµÄĞ´¿¨ÃÜÂë
extern	uchar	ReWriteCardSub_M1(uchar);

extern	uchar ReadCardCommonDatas_CPU(void);
extern	uchar	ReadCard_DaySumConsumMoney_CPU(void);//¶Á³öÀÛ¼ÆÈÕÏû·Ñ¶î
extern	uchar	ReadCardBalanceDatas_CPU(uchar PurseNum);//¶ÁÓà¶î
extern	uchar ConsumPocess_Cpu(void);

extern	uchar	Detect_Card(void);

extern	void	CalEncryptDatas(uchar bbit,uchar * S_Datas ,uchar * KeyDatas ,uchar *  T_Datas ,uchar Len );
extern	void	FindPosConsumCountSub(void);//²éÕÒ×îºóÒ»Ìõ¼ÇÂ¼µÄÁ÷Ë®ºÅ
extern	uchar ReWriteCardSub_Cpu(void);
extern	uchar	ReadCard_DaySumConsumMoney(void);//¶Á³öÀÛ¼ÆÈÕÏû·Ñ¶î

extern uchar ReadMIF_Mode(void);

extern u8		Check_SPI_FLASH(void);//FLASHÓ²¼ş×Ô¼ìº¯Êı¶ÁIDºÅ
extern u8 	Check_24C64(uint16_t);
extern u8 	Check_24C256(uint16_t);
extern u8 	ENC28J60_Get_EREVID(void);
extern void Receive_Data_Double (void);
extern void Clr_WatchDog(void);
extern void	WatchDog_Gpio_init(void);

extern void	Clr_PosDatas(void);

extern unsigned char AT24CXX_Write(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite);
//extern unsigned char RdBytesFromAT24C64(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead) ;

extern void Usart2ToPrint(void);//´®¿Ú2¹¤×÷ÔÚ´òÓ¡»úÄ£Ê½
extern void	JDQOn(void);
extern void	JDQClose(void);
extern void OpedDoor(void);
extern void ChangeLedBright(uchar );//ĞŞ¸ÄÊıÂë¹ÜÁÁ¶È
 //¶ÁÈ¡Ğ¾Æ¬ID
extern void GetLockCode(void);
extern void	ClrConsumDatasBak(void);//²Á³ı´íÎó¼ÇÂ¼
extern uint	ClearSaveRecord(uchar);//È¡Ïû´æ´¢Ïû·Ñ¼ÇÂ¼

extern void SetRemoteIp(void) ;//ÉèÖÃÊÇ·ñ¿ªÆôÔ¶¶ËIP
extern void UdpSend(void);
extern void SetBeepOn(void);
extern void	Beep_init_Close(void);
extern void UdpSendStx(void);
extern void Time_Update(void);
extern unsigned char AT24C64_ReadOneByte(unsigned short );
//¶Á³öÉí·İ¶ÔÓ¦µÄÕÛ¿ÛÖµ
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