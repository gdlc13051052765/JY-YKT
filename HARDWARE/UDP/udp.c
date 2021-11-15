#include "includes.h"
#include 	"MyDefine.h"
#include	"ExternVariableDef.h"
#include	"ExternVoidDef.h"
#include "char.h"

u8 uip_test_mode = 0;

void    uip_polling(void);	
uint	TCPCard_SetNetParameter(void);// ������������ 												
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	
//UDP���Ժ���
void Udp_Serverce(void)
{	
   		uip_polling();	//����uip�¼���������뵽�û������ѭ������  			
}

//UDP��ʼ������
void UDP_Init(void)
{	//	uip_ipaddr_t ipaddr;
	if(tapdev_init())	//��ʼ��ENC28J60 ע�������ʼ��ʧ�ܣ��ͻ���������
	{	 
		DISP_ErrorSub(NETCARD_ERROR);
		while (1);		
	}
	else
	{
		Init_Serial();
	//	memcpy(DispBuffer,"\x00\x39\x5c\x40\x00\x78\x39\x73\x00\x00",10);//Co tCP
	}
	uip_init();	//uIP��ʼ��		
 	/*uip_ipaddr(ipaddr, 192,168,10,199);	//���ñ�������IP��ַ
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, 192,168,10,1); 	//��������IP��ַ(��ʵ������·������IP��ַ)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, 255,255,255,0);	//������������
	uip_setnetmask(ipaddr);	 */
	TCPCard_SetNetParameter();	 //������������

	udp_server_connect();//�������ӵ�UDP Client��,����UDP Server �˿�1600	
}
//uip�¼�������
//���뽫�ú��������û���ѭ��,ѭ������.
void uip_polling(void)
{
	u8 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;	 
	if(timer_ok==0)//����ʼ��һ��
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //����1��0.5��Ķ�ʱ�� 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//����1��10��Ķ�ʱ�� 
	}				 
	uip_len=tapdev_read();	//�������豸��ȡһ��IP��,�õ����ݳ���.uip_len��uip.c�ж���
	if(uip_len>0) 			//������
	{   
		//����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//�Ƿ���IP��? 
		{
			uip_arp_ipin();	//ȥ����̫��ͷ�ṹ������ARP��
			uip_input();   	//IP������
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
			//��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)		    
			if(uip_len>0)//��Ҫ��Ӧ����
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//����arp����,�Ƿ���ARP�����?
		{
			uip_arp_arpin();
 			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len(����2��ȫ�ֱ���)
 			if(uip_len>0)tapdev_send();//��Ҫ��������,��ͨ��tapdev_send����	 
		}
	}else if(timer_expired(&periodic_timer))	//0.5�붨ʱ����ʱ
	{
		timer_reset(&periodic_timer);		//��λ0.5�붨ʱ�� 
		//��������ÿ��TCP����, UIP_CONNSȱʡ��40��  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//����TCPͨ���¼�  
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
	 		if(uip_len>0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#if UIP_UDP	//UIP_UDP 
		//��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10��
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//����UDPͨ���¼�
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
			if(uip_len > 0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#endif 
		//ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}

uint	TCPCard_SetNetParameter(void)// ������������
{
	uip_ipaddr_t ipaddr;
	ulong 	iii=80000;
	uchar	Buffer[24];
	uchar	ParameterDatas[27];
	uchar	i;
	uint UdpCodeTemp;
	RdBytesFromAT24C64(TcpCardDatas_Addr,Buffer,24);
	if (Buffer[0]!=0xa0 || BytesCheckSum(Buffer,24))
	{
		memcpy(ParameterDatas,DefaultNetDatas,14);
		memcpy(ParameterDatas+14,DefaultNetDatas,4);
		memcpy(ParameterDatas+18,DefaultNetDatas+14,8);
	}
	else
	{
		memcpy(ParameterDatas,Buffer+1,14);
		memcpy(ParameterDatas+14,Buffer+1,4);
		memcpy(ParameterDatas+18,Buffer+15,8);
	}
	POSServerPort = (uint)ParameterDatas[4] + ((uint)ParameterDatas[5]<<8);//���ö˿ں�
	uip_ipaddr(ipaddr, ParameterDatas[0],ParameterDatas[1],ParameterDatas[2],ParameterDatas[3]);	//���ñ�������IP��ַ
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, ParameterDatas[6],ParameterDatas[7],ParameterDatas[8],ParameterDatas[9]); 	//��������IP��ַ(��ʵ������·������IP��ַ)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, ParameterDatas[10],ParameterDatas[11],ParameterDatas[12],ParameterDatas[13]);	//������������
	uip_setnetmask(ipaddr);	

	memcpy(RemoteIP,ParameterDatas+20,4); //����Զ��IP

	ClientPort =  ((uint)ParameterDatas[25]<<8)+(uint)ParameterDatas[24]	;
	return UdpCodeTemp;
	
}
