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
 
u32 uip_timer=0;//uip ��ʱ����ÿ10ms����1.
u32 consume_num;
u32 temp = 0;

//��ʱ��2�жϷ������	 
void TIM2_IRQHandler(void)
{ if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		if(ResetEnc28j60Num)	
			ResetEnc28j60Num--;
	} 
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 				    		  			    	    
}
extern u8 timeJDQcnt;
int needgetbtrec = 0;
uchar getbtrec = 0;
//��ʱ��3�жϷ����������ɨ�谴��	 
void TIM3_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
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
		//��������ʱ
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
		//led��˸��ʱ
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
		//////////////////����///////////////
		if(Forbidden)
		ReadKey_Pause=1;

		if((ReadKey_Pause || !(T3_TimeCount%10)) && !Forbidden)  
		{   
			ReakKeySub();
			temp=ChgKeyStringToUlong(keybuff,4);
		
			if(temp)//�м�����
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
	} 	//�����������ⰴ��
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
				    		  			    	    
}
 
//������ʱ��2�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM2_Int_Init(u16 arr,u16 psc)
{	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig( TIM2,TIM_IT_Update|TIM_IT_Trigger,ENABLE);//ʹ�ܶ�ʱ��6���´����ж�
 
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����
 	
  	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 								 
}

//������ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3����ʱ5MS!
void TIM3_Int_Init(u16 arr,u16 psc)
{	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig( TIM3,TIM_IT_Update|TIM_IT_Trigger,ENABLE);//ʹ�ܶ�ʱ��6���´����ж�
 
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
 	
  	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 								 
}
//��ʱ��3�򿪵ĺ���
void TIM3_OPen_INT(void)
{
	TIM_ClearFlag(TIM3, TIM_FLAG_Update); 	//���жϣ�����һ�����жϺ����������ж�				
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //ʹ���ж�Դ	
}
//��ʱ��3�Ĺرպ���
void TIM3_Close_INT(void)
{
  	TIM_ClearFlag(TIM3, TIM_FLAG_Update); 	//���жϣ�����һ�����жϺ����������ж�				
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE); //ʹ���ж�Դ	
}

