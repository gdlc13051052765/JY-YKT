 
#ifndef __APP_CFG_H
#define __APP_CFG_H	

/*******************设置优先级*******************/
 #define STARTUP_TASK_PRIO 4 //主任务优先级
 #define TASK_UDP_PRIO 5	 //UDP任务优先级
 #define TASK_LED_PRIO 6	 //LED任务优先级

 /************设置栈大小 ************/
 #define STARTUP_TASK_STK_SIZE 	1200  
 #define TASK_UDP_STK_SIZE 		512  
 #define TASK_LED_STK_SIZE 		80  

#endif


























