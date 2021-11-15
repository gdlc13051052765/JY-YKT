
	
extern void	SetCommModeSub(void);
extern void	SetConsumModeEnableSub(void);//设置消费方式允许位
extern void	LookSysPatameter(void);//察看系统参数,0-波特率,底金，日限额，使用的扇区号，版本号，消费记录条数等
extern void	SetSysDateTime(ulong);//设置日期时间
extern void	SetConsumMode(ulong);//设置消费模式
extern void	SetSysPrince(uchar,ulong);//设置单价
extern void	LookRecordDatas(ulong);//察看最近的消费记录	
extern void	LookRecordNum(uchar Sort);//察看消费记录条///
extern uchar	SelectDate(uchar * ptr);//选择消费日期
extern	void	ChgInputToLedBuffer(uchar LedX,uchar * ptr,uchar Num);//将输入的字符转换为显示字符
extern void	ChgUlongToLongDispBuffer(ulong,uchar *);




