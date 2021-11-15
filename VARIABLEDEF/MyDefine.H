
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"


#define		ulong	unsigned 	long
#define		uint	unsigned  short
#define		uchar	unsigned  char

#define		MAXCARDPRINTERNUM	262144 //��󿨺�

#define		RECORD_SIZE		    32 //ÿ����¼ռ���ֽ���
#define		RECORD_BLOCK_NUM	60//60 //���ڴ洢���Ѽ�¼�Ŀ���
#define		RECORDNUM_BLOCK		128 //ÿ��洢�ļ�¼�� (4096/RECORD_SIZE)
#define		MAXRECORD_NUM	    7680//���Ĵ洢��¼��
#define   SAVERECORD_BAK_USEBLOCK	   70//�洢�������ݵĿ�
#define 	BT_RECORD_START		(62*0x1000)


#define     FLASH_BLOCK_NUM	   128//4M��FLASH�ܹ�128����ÿ��32KB
//һ�¼������ݴ�������8ҳ�ж�Ӧ��ԭ���洢����ĵ���ҳ�洢������
#define		MenuName_Addr				(SAVERECORD_BAK_USEBLOCK+1)*0x1000//����
#define		SortPrince_Addr			(SAVERECORD_BAK_USEBLOCK+3)*0x1000//�۸񷽰�

#define		PurseKind_Addr						(SAVERECORD_BAK_USEBLOCK+2)*0x1000//����Ǯ��˳��
#define		PurseEnable_Addr					(SAVERECORD_BAK_USEBLOCK+4)*0x1000//����Ǯ��˳��
#define   RecordDatasToBuffer_Addr 	(SAVERECORD_BAK_USEBLOCK+5)*0x1000 //���ݼ�¼��ָ�� 


#define		CONSUM_SIMPLE		0 //����
#define		CONSUM_NUM			1 //�˺�
#define		CONSUM_RATION		2 //����
#define		CONSUM_MONEY		3 //���
#define		CONSUM_MENU			4 //�˵����÷�ʽ

#define		DEFAULT_CONSUM_MODE	CONSUM_MONEY //Ĭ�ϵ����ѷ�ʽ

//zjx_change_201206
#define		SETB_PSAM_RST		bitWrittingFlash=1;CtrlChar&=0xfd;P0=CtrlChar;CS_573_CTRL=1;CS_573_CTRL=0;bitWrittingFlash=0
#define		CLR_PSAM_RST		bitWrittingFlash=1;CtrlChar|=0x02;P0=CtrlChar;CS_573_CTRL=1;CS_573_CTRL=0;bitWrittingFlash=0


#define		STX		2
#define		ETX		3

//zjx_change_201206
#define		CHG485TORECEIVE			bitWrittingFlash=1;CtrlChar&=0xfb;P0=CtrlChar;CS_573_CTRL=1;CS_573_CTRL=0;bitWrittingFlash=0
#define		CHG485TOSEND			bitWrittingFlash=1;CtrlChar|=0x04;P0=CtrlChar;CS_573_CTRL=1;CS_573_CTRL=0;bitWrittingFlash=0

////������Ϣ����
#define		SYS_OK					0
#define		CARD_OK					0
#define		SYS_RC500_ERROR			1//����ģ�� 
#define		SYS_FLASH_ERROR			2//FLASH
#define		SYS_24C256_ERROR		3//24C256
#define		SYS_24C64_ERROR			4//24C64
#define		CARD_BATCH_ERROR		5//����
#define		CARD_NUMBER_ERROR		6//���Ų��Ϸ�(1.BCD 2.����Χ)
#define		CARD_LOSTCARDNUM		7//��ʧ��
#define		CARD_DATA_ERROR			8//�û����ݴ���(1.������У��ʹ�2.���ۿ�Ϊ0)
#define		CARD_LITTLEMONEY		9//����
#define		CARD_OVERDATE				10//1.������2.��δ����3.��������Ч��4.Ǯ����Ч��5.���ڸ�ʽ
#define		SYS_SAVERECORDFULL	11//��¼��
#define   SYS_PINCHECK_FAIL   12//ϵͳУ��ʧ��
#define		CARD_LIMIT_NUM			13//�޴�(1.��ƥ�䷽��2.�޴�)
#define		CARD_CONSUM_OVER		14//���������޶�(1.����2.�滮��Χ3.����4.����)
#define		SYS_8563_ERROR			15//�ϵ���ʱ��ģ��
#define		NETCARD_ERROR				16//TCP�������Ի�ʧ��
#define		TIME_ERROR					17//�����м��Ϸ���
#define		CARD_STATUS_ERROR		18 //��״̬����
//--------------------------------------------------
//zjx_change_mark
//�����ӵĴ���
#define     MATCHCODE_ERR        19//PSAM��ʼ������(1.��λ2.PPSʧ��3.һ��ͨĿ¼4.����Ӵ�����)
#define     CPU_SELFILE_FAIL        20//ѡ�ļ�ʧ��(1.��Ŀ¼2.һ��ͨӦ��3.Ǯ���ļ�)
#define     CPU_READFILE_FAIL       21//���ļ�ʧ��(1.������Ϣ2.�ۼ��ļ�3.��¼)
#define     CPU_REDABALANCE_FAIL    22//�����ʧ��
#define			Uncert_State						23//д��������ʧ��
#define     CARD_SAME_ERROR	        24//��ͬһ�ſ�����(1.���к�2.��ӡˢ��)
#define 		GET_PROVE_FAIL		    	25//ȡ������֤ʧ��
#define     PSAM_MAC1_FAIL		    	26//MAC1
#define     PSAM_CHECK_MAC2		    	27//MAC2
#define	    PSAM_RESET_ERROR          28//ƥ���ִ�
#define	    PSAM_FILEPARA_ERR       29//PSAM����(1.һ��ͨĿ¼2.������ȡ3.վ��Ϊ0)
#define			NO_PURSE_ERROR					30 //Ǯ��û����
#define			SYS_CONSUM_ESC					31 //Ǯ��û����
#define			No_Use					        32 //����������������
#define			No_ip					        	33 //IP��ʧ

#define     Consume_Ok             16
#define     Consume_Err            17
#define     XP_Start               18
#define     Clr_Pos                20
#define     Real_Ok                21
 

//����ʾ��״̬
#define     PSAM_COM_ERROR          0xf1//PSAMͨѶ����(1.����2.����Ӧ3.��Ӧ����) 
#define     CPU_WRITEPURSE_FAIL     0xf2//д�ۼ�ʧ��
#define     CONSUM_PROCE_FAIL	    	0xf3//����ʧ��

#define     CARD_SELF_ERROR         28 //����д�����ݿ�
#define     CARD_BT_ERROR         	29

#define		CARD_CHGERROR			0xfe   
#define			CARD_NOCARD							0xff


/////////////ͨѶ�����//////////////////////
#define		RD_ADDR_COMD	   	    0x30//��վ��

#define   DOWNLODER_COMD        0x19  //����

#define		POS_RST_COMD			0x10//POS��λ
#define		SET_BATCH_COMD		0xb1//���������Ƿ���Ч
#define		RD_BATCH_COMD			0xb2//���������Ƿ���Ч
//=============================================================
#define		RD_USERCODE_COMD			0x32//�ϴ�ƥ����
#define		SET_USERCODE_COMD			0x22//ƥ��������
#define		SET_CALCARDKEY_COMD		0xb3 //����д���ļ�����Կ
#define		RD_CALCARDKEY_COMD		0xb4 //����д���ļ�����Կ
#define		SET_RDCARDCODE_COMD		0x6c//���ض�������
#define		RD_RDCARDCODE_COMD		0x5c//������������
#define		RD_COMMSECTOR_COMD		0x7c//������������������			
#define		SET_COMMSECTOR_COMD		0x7d//���ù�������ɽ����
//=============================================================
#define		RD_MINMONEY_COMD   			0x33//�ϴ��׽�
#define		SET_MINMONEY_COMD   		0x23//�׽�����
#define		RD_BLKNAME_TIME_COMD    0x5D//������������Ч��
#define		SET_BLKNAMETIME_COMD   	0x6D//���غ�������Ч��
#define		CLR_PURSE_COMD					0x9c
#define		SET_PURSE_COMD					0x9b//����Ǯ��
#define		RD_TIME2_COMD   				0x3F//��ѯʱ�䣬
#define		SET_TIME2_COMD   				0x2F//ʱ�����أ�
#define		ADD_BLKNUM_COMD					0x2c//���Ӻ�����
#define		DEL_BLKNUM_COMD					0x4c//ɾ��������
#define		CLR_BLKNUM_COMD					0x44//���������
#define		DEL_ADD_BLKNUM_COMD			0x4d//�ӻ��������
#define		CLR_POSDATAS_COMD				0x4a//���POS����
#define		RD_POSSTATUS_COMD				0x71//����POS״̬
#define		RD_RECORD_COMD   				0x37//�ϴ���¼
#define		DEL_RECORD_COMD   			0x47//ɾ�����ϴ��ļ�¼
#define		INIT_RECORD_PTR_COMD   	0x5B//��ʼ������ָ��
#define		RD_RERECORD_COMD	    	0x3C//���ɼ�¼
#define		DEL_RERECORD_COMD  			0x48//������ָ��
#define		RD_CONSUMMONEY_COMD			0x101//�������Ѷ�
#define		SET_ENCRYPTKEY_COMD			0x102//���ش�����Կ
#define		RD_ENCRYPTKEY_COMD			0x103//����������Կ
#define		LOAD_PROGAM_COMD  			0xf0//���س���
#define		SET_MENU_COMD						0x126 //���ò˵�
#define		CLR_NENU_COMD						0x146 //��˵�
#define		RD_MENU_COMD						0x136 //�����˵�
#define		SET_MENUNAME_COMD				0xdd//���ò���
#define		CLR_MENUNAME_COMD				0xde//�������
#define		SET_SORTPRINCE_COMD		0x95 //���ؼ۸񷽰�
#define		RD_SORTPRINCE_COMD		0x96 //�����۸񷽰�
#define		CLR_SORTPRINCE_COMD		0x97 //����۸񷽰�
///AD by lc�����ۿ۷�������
#define		UP_REBATE_COMD		0xA5 //�ϴ��ۿ۷���
#define		SET_REBATE_COMD		0xA4 //�����ۿ۷���
#define		CLR_REBATE_COMD		0xA3 //����ۿ۷���
#define		SET_DAYLIMET_COMD 0x2e //�������޶�
#define		RD_DAYLIMET_COMD	0x3e //��ȡ���޶�

#define		BTREQUEST_COMD			0x210//��������
#define		BTRECORD_COMD			0x211//���Ͳ�����¼


#define		OK							0
#define		ReDatas_Error		1 //���ܵ����ݴ�
#define		ReComd_Error		2 //���ܵ������
#define		RdDatas_Error   3//�����ݴ�
#define		WrDatas_Error   4//д���ݴ�
#define		NoDatas_Error		5//�޿ɶ���������
#define		Running_Status	6//����ִ��
#define		Running_Error		7//�޷�ִ��

///////////////////////////���̰�������//////////////////
//��һ��:
	#define		KEY_SIMPLE		0x00000001
	#define		KEY_RATION		0x00010000 
	#define		KEY_NUM				0x00020000
	#define		KEY_MONEY			0x00001000	
//�ڶ���:
	#define		KEY_1			0x10000000
	#define		KEY_2			0x01000000
	#define		KEY_3			0x00100000
	#define		KEY_MENU	0x00002000
//������:
	#define		KEY_4			0x20000000
	#define		KEY_5			0x02000000
	#define		KEY_6			0x00200000
	#define		KEY_ESC		0x00004000
//������:
	#define		KEY_7			0x40000000
	#define		KEY_8			0x04000000
	#define		KEY_9			0x00400000
	#define		KEY_MUL		0x00040000
//������:
	#define		KEY_0				0x80000000
	#define		KEY_POINT		0x08000000   //С����,
	#define		KEY_ADD			0x00800000
	#define		KEY_ENTER		0x00008000
//�������ܼ�	
	#define		KEY_0_9			0xF7710000
	#define		KEY_ANY			0xffffffff
	#define		KEY_NO			0x00000000
		
///////////////////AT24C64��ַ����////////////////////
#define		MainCode_Addr			0//վ���0-3
#define		RemoteFlagAddr		   4//��Сվ��ű�ʶ

#define		ReadCardCode_Addr		10 //��������10-17
#define		UserSector_Addr			18 //������������18-20

#define		CardEnableMonths_Addr	21 //����������Ѽ��,21-23
#define		ConsumModeEnable_Addr	29 //���ѷ�ʽ���� 29-31

#define		CalCardKey_Addr			32 //д������ļ�����Կ 32-41

#define		CardMinBalance_Addr		42 //���׽�;0xa0+3�ֽڵ׽�У��;42-46
#define		PinEnable_Addr			47 //�����޶��Ƿ���Ҫ�������� 0xa0--0X55-��Ҫ����--У��,47-49
#define		PurseEnableSet_Addr		50 //Ӧ��Ǯ����Ч����,50-64,������13��Ǯ��
#define		TcpCardDatas_Addr		80 //�����������80-103 
#define		CommModeChar_Addr		107//ͨѶģʽ����,485--TCP/IP,107-109
#define		BatchEnable_Addr		110 //�����Ƿ���Ч����,32���ֽڱ�ʶ255��
#define		Comm_EncryptKey_Addr	144 //������Կ;144-153
#define		SimplePrince_Addr		154 //�������ѵ���(Ԥ��12�� �ռ�12*4=48), 154-201
#define		UserCode_Addr				202	//ƥ���� 202-207
#define		MenuPrince_Addr     250//�˺����ѵ���(���250�����ռ�250*4=280)210-1210
#define		DayLimetMoney_Addr	210 //���޶�;0xa0+3�ֽڵ׽�У��;500-508
#define   CanBuTie_Addr				2000
//#define   LoadFlagAddr        1000//
//#define		Rebate_Addr         1500//�����ۿ۵�ַ(99) 1500-1600 
//#define		Rebate_Flag_Addr    1601//�����ۿ۵�ַ(99) 1601-1604 


#define		APDU_RESTX		0x02
#define 	BAT_GPIO    PCin(3)  //��ؼ��

//�����洢��flash��ַ
#define MainCodeAddress    				0x08061000
#define UserSectorFlashAdd  			MainCodeAddress+1*2//����������
#define UserCodeFlashAdd  				UserSectorFlashAdd+1*2//ƥ����
#define CardKeyCodeFlashAdd				UserCodeFlashAdd+4*2//����ɽ������
#define CalCardKeyFlashAdd				CardKeyCodeFlashAdd+6*2//����Կ
#define CardMinBalanceFlashAdd		CalCardKeyFlashAdd+8*2//���׽�
#define DayLimetMoneyFlashAdd   	CardMinBalanceFlashAdd+1*2//�����޶�
#define CardEnableMonthsFlasgAdd	DayLimetMoneyFlashAdd+2*2//��������Ч��
#define ConsumModeEnableFlashAdd	CardEnableMonthsFlasgAdd+1*2//���ѷ�ʽ����λ
#define BatchEnableFlashAdd				ConsumModeEnableFlashAdd+1*2//����
#define CommEncryptKeyFlasAdd			BatchEnableFlashAdd+32*2//������Կ
#define CommModeCharFlashAdd			CommEncryptKeyFlasAdd+8*2//ͨѶ��ʽ
#define RemoteFlagFlashAdd				CommModeCharFlashAdd +1*2//Զ�˰󶨱��
#define BuTieFlashAdd							RemoteFlagFlashAdd +1*2//�������
#define TcpCardDatasFlashAdd			BuTieFlashAdd +1*2
