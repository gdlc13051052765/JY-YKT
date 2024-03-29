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

//uip事件处理函数
//必须将该函数插入用户主循环,循环调用.
extern  void uip_polling(void);

extern	void	SetDoubleMainCode(void);//设置大小站点号通讯协议
extern	void	SetConsumModeEnableSub(void);//设置消费方式允许位
extern	void	LookSysPatameter(void);//察看系统参数
extern	void	SetSysDateTime(ulong);//设置日期时间
extern	void	SetConsumMode(ulong);//设置消费模式
extern	void	SetSysPrince(uchar,ulong);//设置单价
extern	void	LookRecordDatas(ulong);	
extern	void	SetCommModeSub(void);

extern	void	SetPurseSectorSub(void);//设置子钱包是否有效
extern	void	SetSuperCardSub(void);

extern	void	ConsumSub(void);//消费主程序

extern	uchar	SearchPurseBalnanceDatas(uchar,uchar,uchar *,uchar *);//查找钱包
extern	uchar	CurrentConsumMoneyDiag(uchar);//卡种类和消费额诊断
extern	uchar	ReadCardBalanceDatas(uchar );//读出对应扇区的卡余额
extern	uchar	InputSumConsumMoney(uchar,ulong);//消费模式

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
extern	void	Disp_Ver(uchar);//显示版本号

extern	void	ChgIntToDispBuffer(uint,uchar * );
extern	void	DISP_WriteError(uchar * );//显示写卡错误
extern	void	DispOver(void);

extern  void  Disp_Gpio(void);
extern	void	Disp_Balance(uchar *);//显示余额
extern	void	DispDate(uchar * );//显示日期
extern	void	DispTime(uchar *);//显示时间
extern	void	ChgUlongToDispBuffer(ulong ,uchar *);
extern	void	Disp_Clr_Ram(void);//显示清空Pos;
extern	void	DispMinBalance(void);//显示底金
extern  void	DispPutCard(void);
extern	void	ChgUlongToLongDispBuffer(ulong ,uchar *);

extern	ulong	ScanKeySub(ulong);
extern	ulong	ScanKeySub3(ulong);
extern	uchar	ChgKeyValueToChar(ulong);//将键值转换为0-9
extern	void	ReadSysParameter(uchar);


extern	uchar	Check_SST_39SF040(void) ;   //Read ID Code
extern	void	Erase_Entire_Chip(void);  
extern	void	Erase_One_Sector(ulong);
extern	void	Flash_Write_Bytes( ulong,uchar * , unsigned short);//写入多个字节
extern	void	Flash_Rd_Bytes (ulong,uchar * ,unsigned short);//从中读出多个字节
extern void 	SPI_Flash_Text(void);
extern void 	SPI_Flash_Init(void);


extern	uchar	Check_24C64(uint);
extern	uchar	RdByte_24C256(uint);
extern	void	WrPage_24(uint,uchar);//页写入同样的字节64个
extern	void	WrByte_24C256(uint,uchar);
extern  void 	Clr_24C02(void);
extern  uchar	Check_BlackCardNum(ulong); 
extern 	void 	check_24C256(void);
extern void 	AT24C64_WriteOneByte(unsigned short ,unsigned char );

extern	void	Init_X25045(void);
extern	uchar	RdBytesFromAT24C64(uint,uchar  *,uint);   //连续读出多字节
extern	uchar WrBytesToAT24C64(uint,uchar *,uint);    //写入多字节
extern	uchar	Check_25045(uint);
extern	void 	WrPageTo25045(uint,uchar);//页写入相同的字节,每页4字节

extern	uchar	CheckPF8563(void);
extern	void 	Set_Sysdate(uchar *ptr);//设置系统日期
extern	void 	Read_Sysdate(uchar *ptr);//读出系统日期
extern  void	SetMainCodeSub(void);//手动设置站点号

extern	uchar   BytesCheckSum(uchar *,uchar);//校验和比较
extern	uchar   BytesComp( uchar * ,uchar *,uchar);//字符串比较，正确返回0
extern	uchar		BCD_String_Diag(uchar * ,uchar);//BCD码字符串诊断
extern	uchar   BCDToHex(uchar  );
extern	uchar   HexToBCD(uchar  );
extern	ulong		ChgBCDStringToUlong(uchar * ,uchar);//BCD码字符串转换为整型数
extern	uchar		CalCheckSum(uchar * ,uchar);//计算校验和�
extern	void		ChgUlongToBCDString( ulong ,uchar * ,uchar );
extern	void		ChgIntToStrings(uint	ii,uchar * ptr);
extern	ulong		ChgInputToUlong(uchar * ,uchar);//输入的数字转换为长整形
extern	ulong 	GetU32_HiLo(uchar *);
extern	uint 		GetU16_HiLo(uchar *);
extern	void 		PutU32_HiLo(uchar *,ulong);
extern	void 		PutU16_HiLo(uchar *,uint);
extern	void		FormatBuffer(uchar * ,uchar *);
extern uint16_t DoubleBigToSmall(uint16_t );
extern int32_t  FourBigToSmall(uint32_t a);
extern ulong		ChgStringsToInt(uchar * ptr);

extern	void	InitSaveRecordSub(uchar);//初始化指针
extern	void	SaveRecordSub(uchar);//存储消费记录
extern	uchar	RecordDatasToBuffer(uchar,uchar *);//存储记录的内容写入缓存
extern	uchar	RecordFullDiag(void);//记录存储是否已满诊断

extern	uchar	Init_NetCard(void);
extern	uchar	TCPCardReceiveDiag(void);//TCP卡接收数据包整理
extern	uint	TCPCard_SetNetParameter(void);// 读出网卡参数 	
extern	void	UART550_SendBytes(uchar * ,uchar );
extern	uchar UART550_RecByte(uint,uchar *);//zjx_change_201206
extern	void 	Init550_ISO7816 (uchar );
extern	void	UART550_SendByte(uchar);

extern	uint	Cal_CRC_Sub(uchar * , uint ); 

extern	void	ChgTimeToRecordDatas(uchar * ,uchar * );
extern	void	ChgRecordDatasToTime(uchar * ,uchar * );

extern  uchar check_RFCPU_ResetInfo(uchar *, void *);
extern	uchar	RequestCard(void);//卡复位信息
extern	char 	ISO7816_TRANSCEIVE(unsigned char, void *, unsigned char *, void *);//7816数据通讯
extern	uint  IC7816_relay_station(uchar,uchar, void *, uchar *, void *);
extern	uint 	CPU_Select_File_AID(uchar, uchar * ,uchar *,uchar *);//AID选择文件

extern	char 	ISO7816_PSAM(unsigned char , unsigned char *, unsigned char *, unsigned char *);
extern  uchar check_PSAM_ResetInfo(void);
extern 	char  CV522Pcd_ResetState(void);
//extern  void	PSAM_PowerDown(void);

extern	uchar ReadCardCommonDatas(void);
extern	uchar CPU_Get_Challenge(uchar *, uchar *);//终端从卡上读取随机数
extern	uint 	CPU_Verify_PIN(uchar,uchar,uchar,uchar *);
extern	uchar ConsumPocess(void);
extern	uchar CheckCardPrinterNum(void);

extern	uchar WriteCardLjdata_Cpu(uchar);
extern	uchar WriteCardLjdata(void);
extern	uchar	Sure_ConsumSucce(void);
extern	void	Disp_Hello(void);

extern	uchar PSAM_Init_Dir(void);
extern	void	SaveRecordBakSub(void);//保存错误记录的备份

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
extern	uchar	ReadCardCommonDatas_M1 (void);//读出公共区的数据
extern	uchar	ReadCard_DaySumConsumMoney_M1(void);//读出累计日消费额
extern	uchar	ReadCardBalanceDatas_M1(uchar );//读出对应扇区的卡余额
extern	uchar	WriteBalanceToCard_M1(uchar,uchar);//写卡
extern	void	CalPurseKeyCode(uchar *,uchar *,uchar *,uchar *);//计算钱包的写卡密码
extern	uchar	ReWriteCardSub_M1(uchar);

extern	uchar ReadCardCommonDatas_CPU(void);
extern	uchar	ReadCard_DaySumConsumMoney_CPU(void);//读出累计日消费额
extern	uchar	ReadCardBalanceDatas_CPU(uchar PurseNum);//读余额
extern	uchar ConsumPocess_Cpu(void);

extern	uchar	Detect_Card(void);

extern	void	CalEncryptDatas(uchar bbit,uchar * S_Datas ,uchar * KeyDatas ,uchar *  T_Datas ,uchar Len );
extern	void	FindPosConsumCountSub(void);//查找最后一条记录的流水号
extern	uchar ReWriteCardSub_Cpu(void);
extern	uchar	ReadCard_DaySumConsumMoney(void);//读出累计日消费额

extern uchar ReadMIF_Mode(void);

extern u8		Check_SPI_FLASH(void);//FLASH硬件自检函数读ID号
extern u8 	Check_24C64(uint16_t);
extern u8 	Check_24C256(uint16_t);
extern u8 	ENC28J60_Get_EREVID(void);
extern void Receive_Data_Double (void);
extern void Clr_WatchDog(void);
extern void	WatchDog_Gpio_init(void);

extern void	Clr_PosDatas(void);

extern unsigned char AT24CXX_Write(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite);
//extern unsigned char RdBytesFromAT24C64(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead) ;

extern void Usart2ToPrint(void);//串口2工作在打印机模式
extern void	JDQOn(void);
extern void	JDQClose(void);
extern void OpedDoor(void);
extern void ChangeLedBright(uchar );//修改数码管亮度
 //读取芯片ID
extern void GetLockCode(void);
extern void	ClrConsumDatasBak(void);//擦除错误记录
extern uint	ClearSaveRecord(uchar);//取消存储消费记录

extern void SetRemoteIp(void) ;//设置是否开启远端IP
extern void UdpSend(void);
extern void SetBeepOn(void);
extern void	Beep_init_Close(void);
extern void UdpSendStx(void);
extern void Time_Update(void);
extern unsigned char AT24C64_ReadOneByte(unsigned short );
//读出身份对应的折扣值
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