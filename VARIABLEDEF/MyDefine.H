
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"


#define		ulong	unsigned 	long
#define		uint	unsigned  short
#define		uchar	unsigned  char

#define		MAXCARDPRINTERNUM	262144 //最大卡号

#define		RECORD_SIZE		    32 //每条记录占的字节数
#define		RECORD_BLOCK_NUM	60//60 //用于存储消费记录的块数
#define		RECORDNUM_BLOCK		128 //每块存储的记录数 (4096/RECORD_SIZE)
#define		MAXRECORD_NUM	    7680//最多的存储记录数
#define   SAVERECORD_BAK_USEBLOCK	   70//存储交易数据的块
#define 	BT_RECORD_START		(62*0x1000)


#define     FLASH_BLOCK_NUM	   128//4M的FLASH总共128个块每块32KB
//一下几个数据存在最后的8页中对应着原来存储程序的第三页存储的数据
#define		MenuName_Addr				(SAVERECORD_BAK_USEBLOCK+1)*0x1000//菜名
#define		SortPrince_Addr			(SAVERECORD_BAK_USEBLOCK+3)*0x1000//价格方案

#define		PurseKind_Addr						(SAVERECORD_BAK_USEBLOCK+2)*0x1000//身份钱包顺序
#define		PurseEnable_Addr					(SAVERECORD_BAK_USEBLOCK+4)*0x1000//身份钱包顺序
#define   RecordDatasToBuffer_Addr 	(SAVERECORD_BAK_USEBLOCK+5)*0x1000 //备份记录的指针 


#define		CONSUM_SIMPLE		0 //简易
#define		CONSUM_NUM			1 //菜号
#define		CONSUM_RATION		2 //定额
#define		CONSUM_MONEY		3 //金额
#define		CONSUM_MENU			4 //菜单设置方式

#define		DEFAULT_CONSUM_MODE	CONSUM_MONEY //默认的消费方式

//zjx_change_201206
#define		SETB_PSAM_RST		bitWrittingFlash=1;CtrlChar&=0xfd;P0=CtrlChar;CS_573_CTRL=1;CS_573_CTRL=0;bitWrittingFlash=0
#define		CLR_PSAM_RST		bitWrittingFlash=1;CtrlChar|=0x02;P0=CtrlChar;CS_573_CTRL=1;CS_573_CTRL=0;bitWrittingFlash=0


#define		STX		2
#define		ETX		3

//zjx_change_201206
#define		CHG485TORECEIVE			bitWrittingFlash=1;CtrlChar&=0xfb;P0=CtrlChar;CS_573_CTRL=1;CS_573_CTRL=0;bitWrittingFlash=0
#define		CHG485TOSEND			bitWrittingFlash=1;CtrlChar|=0x04;P0=CtrlChar;CS_573_CTRL=1;CS_573_CTRL=0;bitWrittingFlash=0

////错误信息定义
#define		SYS_OK					0
#define		CARD_OK					0
#define		SYS_RC500_ERROR			1//读卡模块 
#define		SYS_FLASH_ERROR			2//FLASH
#define		SYS_24C256_ERROR		3//24C256
#define		SYS_24C64_ERROR			4//24C64
#define		CARD_BATCH_ERROR		5//批次
#define		CARD_NUMBER_ERROR		6//卡号不合法(1.BCD 2.超范围)
#define		CARD_LOSTCARDNUM		7//挂失卡
#define		CARD_DATA_ERROR			8//用户数据错误(1.卡数据校验和错2.卡折扣为0)
#define		CARD_LITTLEMONEY		9//金额不足
#define		CARD_OVERDATE				10//1.卡过期2.卡未启用3.黑名单有效月4.钱包有效期5.日期格式
#define		SYS_SAVERECORDFULL	11//记录满
#define   SYS_PINCHECK_FAIL   12//系统校验失败
#define		CARD_LIMIT_NUM			13//限次(1.无匹配方案2.限次)
#define		CARD_CONSUM_OVER		14//超出消费限额(1.单笔2.规划范围3.日限4.月限)
#define		SYS_8563_ERROR			15//上电检测时钟模块
#define		NETCARD_ERROR				16//TCP网卡初试化失败
#define		TIME_ERROR					17//消费中检测合法性
#define		CARD_STATUS_ERROR		18 //卡状态错误
//--------------------------------------------------
//zjx_change_mark
//新添加的错误
#define     MATCHCODE_ERR        19//PSAM初始化错误(1.复位2.PPS失败3.一卡通目录4.触点接触不良)
#define     CPU_SELFILE_FAIL        20//选文件失败(1.主目录2.一卡通应用3.钱包文件)
#define     CPU_READFILE_FAIL       21//读文件失败(1.公共信息2.累计文件3.记录)
#define     CPU_REDABALANCE_FAIL    22//读余额失败
#define			Uncert_State						23//写卡、交易失败
#define     CARD_SAME_ERROR	        24//非同一张卡错误(1.序列号2.卡印刷号)
#define 		GET_PROVE_FAIL		    	25//取交易认证失败
#define     PSAM_MAC1_FAIL		    	26//MAC1
#define     PSAM_CHECK_MAC2		    	27//MAC2
#define	    PSAM_RESET_ERROR          28//匹配字错
#define	    PSAM_FILEPARA_ERR       29//PSAM错误(1.一卡通目录2.参数读取3.站点为0)
#define			NO_PURSE_ERROR					30 //钱包没设置
#define			SYS_CONSUM_ESC					31 //钱包没设置
#define			No_Use					        32 //不允许此身份消费
#define			No_ip					        	33 //IP丢失

#define     Consume_Ok             16
#define     Consume_Err            17
#define     XP_Start               18
#define     Clr_Pos                20
#define     Real_Ok                21
 

//不显示的状态
#define     PSAM_COM_ERROR          0xf1//PSAM通讯错误(1.长度2.无响应3.响应错误) 
#define     CPU_WRITEPURSE_FAIL     0xf2//写累计失败
#define     CONSUM_PROCE_FAIL	    	0xf3//交易失败

#define     CARD_SELF_ERROR         28 //不能写入数据卡
#define     CARD_BT_ERROR         	29

#define		CARD_CHGERROR			0xfe   
#define			CARD_NOCARD							0xff


/////////////通讯命令定义//////////////////////
#define		RD_ADDR_COMD	   	    0x30//读站点

#define   DOWNLODER_COMD        0x19  //下载

#define		POS_RST_COMD			0x10//POS复位
#define		SET_BATCH_COMD		0xb1//设置批次是否有效
#define		RD_BATCH_COMD			0xb2//读出批次是否有效
//=============================================================
#define		RD_USERCODE_COMD			0x32//上传匹配字
#define		SET_USERCODE_COMD			0x22//匹配字下载
#define		SET_CALCARDKEY_COMD		0xb3 //下载写卡的加密密钥
#define		RD_CALCARDKEY_COMD		0xb4 //读出写卡的加密密钥
#define		SET_RDCARDCODE_COMD		0x6c//下载读卡密码
#define		RD_RDCARDCODE_COMD		0x5c//读出读卡密码
#define		RD_COMMSECTOR_COMD		0x7c//读出公共区的扇区号			
#define		SET_COMMSECTOR_COMD		0x7d//设置公共区的山区号
//=============================================================
#define		RD_MINMONEY_COMD   			0x33//上传底金
#define		SET_MINMONEY_COMD   		0x23//底金下载
#define		RD_BLKNAME_TIME_COMD    0x5D//读出黑名单有效期
#define		SET_BLKNAMETIME_COMD   	0x6D//下载黑名单有效期
#define		CLR_PURSE_COMD					0x9c
#define		SET_PURSE_COMD					0x9b//设置钱包
#define		RD_TIME2_COMD   				0x3F//查询时间，
#define		SET_TIME2_COMD   				0x2F//时间下载，
#define		ADD_BLKNUM_COMD					0x2c//增加黑名单
#define		DEL_BLKNUM_COMD					0x4c//删除黑名单
#define		CLR_BLKNUM_COMD					0x44//清除黑名单
#define		DEL_ADD_BLKNUM_COMD			0x4d//加或减黑名单
#define		CLR_POSDATAS_COMD				0x4a//清除POS数据
#define		RD_POSSTATUS_COMD				0x71//读出POS状态
#define		RD_RECORD_COMD   				0x37//上传记录
#define		DEL_RECORD_COMD   			0x47//删除已上传的记录
#define		INIT_RECORD_PTR_COMD   	0x5B//初始化复采指针
#define		RD_RERECORD_COMD	    	0x3C//复采记录
#define		DEL_RERECORD_COMD  			0x48//复采移指针
#define		RD_CONSUMMONEY_COMD			0x101//读出消费额
#define		SET_ENCRYPTKEY_COMD			0x102//下载传输密钥
#define		RD_ENCRYPTKEY_COMD			0x103//读出传输密钥
#define		LOAD_PROGAM_COMD  			0xf0//下载程序
#define		SET_MENU_COMD						0x126 //设置菜单
#define		CLR_NENU_COMD						0x146 //清菜单
#define		RD_MENU_COMD						0x136 //读出菜单
#define		SET_MENUNAME_COMD				0xdd//设置菜名
#define		CLR_MENUNAME_COMD				0xde//清除菜名
#define		SET_SORTPRINCE_COMD		0x95 //下载价格方案
#define		RD_SORTPRINCE_COMD		0x96 //读出价格方案
#define		CLR_SORTPRINCE_COMD		0x97 //清除价格方案
///AD by lc增加折扣费率命令
#define		UP_REBATE_COMD		0xA5 //上传折扣费率
#define		SET_REBATE_COMD		0xA4 //下载折扣费率
#define		CLR_REBATE_COMD		0xA3 //清除折扣费率
#define		SET_DAYLIMET_COMD 0x2e //设置日限额
#define		RD_DAYLIMET_COMD	0x3e //读取日限额

#define		BTREQUEST_COMD			0x210//补贴请求
#define		BTRECORD_COMD			0x211//传送补贴记录


#define		OK							0
#define		ReDatas_Error		1 //接受的数据错
#define		ReComd_Error		2 //接受的命令错
#define		RdDatas_Error   3//读数据错
#define		WrDatas_Error   4//写数据错
#define		NoDatas_Error		5//无可读出的数据
#define		Running_Status	6//正在执行
#define		Running_Error		7//无法执行

///////////////////////////键盘按键定义//////////////////
//第一行:
	#define		KEY_SIMPLE		0x00000001
	#define		KEY_RATION		0x00010000 
	#define		KEY_NUM				0x00020000
	#define		KEY_MONEY			0x00001000	
//第二行:
	#define		KEY_1			0x10000000
	#define		KEY_2			0x01000000
	#define		KEY_3			0x00100000
	#define		KEY_MENU	0x00002000
//第三行:
	#define		KEY_4			0x20000000
	#define		KEY_5			0x02000000
	#define		KEY_6			0x00200000
	#define		KEY_ESC		0x00004000
//第四行:
	#define		KEY_7			0x40000000
	#define		KEY_8			0x04000000
	#define		KEY_9			0x00400000
	#define		KEY_MUL		0x00040000
//第五行:
	#define		KEY_0				0x80000000
	#define		KEY_POINT		0x08000000   //小数点,
	#define		KEY_ADD			0x00800000
	#define		KEY_ENTER		0x00008000
//其它功能键	
	#define		KEY_0_9			0xF7710000
	#define		KEY_ANY			0xffffffff
	#define		KEY_NO			0x00000000
		
///////////////////AT24C64地址定义////////////////////
#define		MainCode_Addr			0//站点号0-3
#define		RemoteFlagAddr		   4//大小站点号标识

#define		ReadCardCode_Addr		10 //读卡密码10-17
#define		UserSector_Addr			18 //公共区的扇区18-20

#define		CardEnableMonths_Addr	21 //最大允许消费间隔,21-23
#define		ConsumModeEnable_Addr	29 //消费方式允许 29-31

#define		CalCardKey_Addr			32 //写卡密码的加密密钥 32-41

#define		CardMinBalance_Addr		42 //卡底金;0xa0+3字节底金＋校验;42-46
#define		PinEnable_Addr			47 //超出限额是否需要输入密码 0xa0--0X55-需要输入--校验,47-49
#define		PurseEnableSet_Addr		50 //应用钱包有效设置,50-64,不超过13个钱包
#define		TcpCardDatas_Addr		80 //网络参数设置80-103 
#define		CommModeChar_Addr		107//通讯模式设置,485--TCP/IP,107-109
#define		BatchEnable_Addr		110 //批次是否有效设置,32个字节标识255批
#define		Comm_EncryptKey_Addr	144 //传输密钥;144-153
#define		SimplePrince_Addr		154 //简易消费单价(预留12个 空间12*4=48), 154-201
#define		UserCode_Addr				202	//匹配字 202-207
#define		MenuPrince_Addr     250//菜号消费单价(最多250个，空间250*4=280)210-1210
#define		DayLimetMoney_Addr	210 //日限额;0xa0+3字节底金＋校验;500-508
#define   CanBuTie_Addr				2000
//#define   LoadFlagAddr        1000//
//#define		Rebate_Addr         1500//身份折扣地址(99) 1500-1600 
//#define		Rebate_Flag_Addr    1601//身份折扣地址(99) 1601-1604 


#define		APDU_RESTX		0x02
#define 	BAT_GPIO    PCin(3)  //电池检测

//参数存储的flash地址
#define MainCodeAddress    				0x08061000
#define UserSectorFlashAdd  			MainCodeAddress+1*2//公共扇区号
#define UserCodeFlashAdd  				UserSectorFlashAdd+1*2//匹配字
#define CardKeyCodeFlashAdd				UserCodeFlashAdd+4*2//公共山区密码
#define CalCardKeyFlashAdd				CardKeyCodeFlashAdd+6*2//卡密钥
#define CardMinBalanceFlashAdd		CalCardKeyFlashAdd+8*2//卡底金
#define DayLimetMoneyFlashAdd   	CardMinBalanceFlashAdd+1*2//卡日限额
#define CardEnableMonthsFlasgAdd	DayLimetMoneyFlashAdd+2*2//黑名单有效月
#define ConsumModeEnableFlashAdd	CardEnableMonthsFlasgAdd+1*2//消费方式允许位
#define BatchEnableFlashAdd				ConsumModeEnableFlashAdd+1*2//批次
#define CommEncryptKeyFlasAdd			BatchEnableFlashAdd+32*2//传输密钥
#define CommModeCharFlashAdd			CommEncryptKeyFlasAdd+8*2//通讯方式
#define RemoteFlagFlashAdd				CommModeCharFlashAdd +1*2//远端绑定标记
#define BuTieFlashAdd							RemoteFlagFlashAdd +1*2//补贴标记
#define TcpCardDatasFlashAdd			BuTieFlashAdd +1*2

