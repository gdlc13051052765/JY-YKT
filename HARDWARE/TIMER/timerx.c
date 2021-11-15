#include "timerx.h"
#include "led.h"
#include "usart.h"
#include "MyDefine.H"
#include "ExternVariableDef.H"
#include "ExternVoidDef.H"
//#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 
unsigned char bufferlc[16] = {0x79,0x50,0x50,0x40,0x00,0x3f,0x6f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
////////////////////////////////////////////////////////////////////////////////// 	  
 
u32 uip_timer=0;//uip 计时器，每10ms增加1.
u32 consume_num;
u32 temp = 0;

//定时器2中断服务程序	 
void TIM2_IRQHandler(void)
{ if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		if(ResetEnc28j60Num)	
			ResetEnc28j60Num--;
	} 
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 				    		  			    	    
}
extern u8 timeJDQcnt;
int needgetbtrec = 0;
uchar getbtrec = 0;
//定时器3中断服务程序用于扫描按键	 
void TIM3_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		T3_TimeCount++;
		Led_Open_time++;
		needgetbtrec++;
		if(needgetbtrec >= 3000)
		{
			needgetbtrec = 0;
			getbtrec = 1;
		}
		consume_num++;
			if(consume_num>200)
			{
			if(timeJDQcnt) timeJDQcnt--;
			else GPIO_ResetBits(GPIOA,GPIO_Pin_0);
				
				consume_num = 0;
			}
		//蜂鸣器计时
		if (DelayReadCardTimeCount)
			DelayReadCardTimeCount--;
			BeepDelayTimeCount++;
		if(BeepTimes)
		{
			
			if(BeepDelayTimeCount>110)
			{
				PDout(7)=0;
			}
			else
			{
				PDout(7)=1;
			}
			if(BeepDelayTimeCount>=220)
			{
				BeepDelayTimeCount=0;
				BeepTimes--;
			}
			
		}
		//led闪烁计时
		if(Led_Open_time <1000)
		{
			PCout(1)=0;
		}
		else if(1000<Led_Open_time <=2000)
		{
			if(Led_Open_time==2000)
			{
				Led_Open_time = 0;
			}
			PCout(1)=1;

		}
		//////////////////键盘///////////////
		if(Forbidden)
		ReadKey_Pause=1;

		if((ReadKey_Pause || !(T3_TimeCount%10)) && !Forbidden)  
		{   
			ReakKeySub();
			temp=ChgKeyStringToUlong(keybuff,4);
		
			if(temp)//有键按下
			{	 //BeepOn(1000);
				if (bitHaveReleaseKey && !bitHaveKey )
				{
					bitHaveReleaseKey=0;
					bitHaveKey=1;
					memset(keybuff,0,4);
					KeyValue=temp;					
				}
			}
			else
				bitHaveReleaseKey=1;   
        		ReadKey_Pause=0;
		}	
	} 	//到这里结束检测按键
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
				    		  			    	    
}
 
//基本定时器2中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM2_Int_Init(u16 arr,u16 psc)
{	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig( TIM2,TIM_IT_Update|TIM_IT_Trigger,ENABLE);//使能定时器6更新触发中断
 
	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
 	
  	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 								 
}

//基本定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3，定时5MS!
void TIM3_Int_Init(u16 arr,u16 psc)
{	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig( TIM3,TIM_IT_Update|TIM_IT_Trigger,ENABLE);//使能定时器6更新触发中断
 
	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
 	
  	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 								 
}
//定时器3打开的函数
void TIM3_OPen_INT(void)
{
	TIM_ClearFlag(TIM3, TIM_FLAG_Update); 	//清中断，以免一启用中断后立即产生中断				
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //使能中断源	
}
//定时器3的关闭函数
void TIM3_Close_INT(void)
{
  	TIM_ClearFlag(TIM3, TIM_FLAG_Update); 	//清中断，以免一启用中断后立即产生中断				
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE); //使能中断源	
}

