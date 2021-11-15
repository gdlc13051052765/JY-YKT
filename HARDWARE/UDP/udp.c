#include "includes.h"
#include 	"MyDefine.h"
#include	"ExternVariableDef.h"
#include	"ExternVoidDef.h"
#include "char.h"

u8 uip_test_mode = 0;

void    uip_polling(void);	
uint	TCPCard_SetNetParameter(void);// 读出网卡参数 												
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	
//UDP测试函数
void Udp_Serverce(void)
{	
   		uip_polling();	//处理uip事件，必须插入到用户程序的循环体中  			
}

//UDP初始化函数
void UDP_Init(void)
{	//	uip_ipaddr_t ipaddr;
	if(tapdev_init())	//初始化ENC28J60 注意如果初始化失败，就会死在这里
	{	 
		DISP_ErrorSub(NETCARD_ERROR);
		while (1);		
	}
	else
	{
		Init_Serial();
	//	memcpy(DispBuffer,"\x00\x39\x5c\x40\x00\x78\x39\x73\x00\x00",10);//Co tCP
	}
	uip_init();	//uIP初始化		
 	/*uip_ipaddr(ipaddr, 192,168,10,199);	//设置本地设置IP地址
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, 192,168,10,1); 	//设置网关IP地址(其实就是你路由器的IP地址)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, 255,255,255,0);	//设置网络掩码
	uip_setnetmask(ipaddr);	 */
	TCPCard_SetNetParameter();	 //设置网卡参数

	udp_server_connect();//尝试连接到UDP Client端,用于UDP Server 端口1600	
}
//uip事件处理函数
//必须将该函数插入用户主循环,循环调用.
void uip_polling(void)
{
	u8 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;	 
	if(timer_ok==0)//仅初始化一次
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //创建1个0.5秒的定时器 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//创建1个10秒的定时器 
	}				 
	uip_len=tapdev_read();	//从网络设备读取一个IP包,得到数据长度.uip_len在uip.c中定义
	if(uip_len>0) 			//有数据
	{   
		//处理IP数据包(只有校验通过的IP包才会被接收) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//是否是IP包? 
		{
			uip_arp_ipin();	//去除以太网头结构，更新ARP表
			uip_input();   	//IP包处理
			//当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
			//需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)		    
			if(uip_len>0)//需要回应数据
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//处理arp报文,是否是ARP请求包?
		{
			uip_arp_arpin();
 			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len(这是2个全局变量)
 			if(uip_len>0)tapdev_send();//需要发送数据,则通过tapdev_send发送	 
		}
	}else if(timer_expired(&periodic_timer))	//0.5秒定时器超时
	{
		timer_reset(&periodic_timer);		//复位0.5秒定时器 
		//轮流处理每个TCP连接, UIP_CONNS缺省是40个  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//处理TCP通信事件  
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
	 		if(uip_len>0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#if UIP_UDP	//UIP_UDP 
		//轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//处理UDP通信事件
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
			if(uip_len > 0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#endif 
		//每隔10秒调用1次ARP定时器函数 用于定期ARP处理,ARP表10秒更新一次，旧的条目会被抛弃
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}

uint	TCPCard_SetNetParameter(void)// 设置网卡参数
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
	POSServerPort = (uint)ParameterDatas[4] + ((uint)ParameterDatas[5]<<8);//设置端口号
	uip_ipaddr(ipaddr, ParameterDatas[0],ParameterDatas[1],ParameterDatas[2],ParameterDatas[3]);	//设置本地设置IP地址
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, ParameterDatas[6],ParameterDatas[7],ParameterDatas[8],ParameterDatas[9]); 	//设置网关IP地址(其实就是你路由器的IP地址)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, ParameterDatas[10],ParameterDatas[11],ParameterDatas[12],ParameterDatas[13]);	//设置网络掩码
	uip_setnetmask(ipaddr);	

	memcpy(RemoteIP,ParameterDatas+20,4); //设置远端IP

	ClientPort =  ((uint)ParameterDatas[25]<<8)+(uint)ParameterDatas[24]	;
	return UdpCodeTemp;
	
}
