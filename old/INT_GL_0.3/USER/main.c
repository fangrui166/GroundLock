

//上升 ：55 00 01 02 78 AA
//下降 ：55 00 01 01 9A AA
//读取状态 :55 00 01 06 19 AA



#include "sys.h"
#include "core_cm3.h"
#include "delay.h"
#include "IO.h"
#include "usart.h"
#include "global.h"
#include "timer.h"

/*看门狗初始化 单位：ms Tout=((4*2^prer)*rlr)/40*/
void IWDG_Init(u16 prer,u16 rlr)
{
	IWDG->KR=0X5555;      
	IWDG->PR=prer;        
	IWDG->RLR=rlr;         
	IWDG->KR=0XAAAA;   
	IWDG->KR=0XCCCC;     
}
/*喂狗函数*/
void IWDG_Feed(void)
{
	IWDG->KR=0XAAAA;                                  
}

//u8 ggg[] = {1,2,3,4,5};
//u8  cc = 0;
//RCC_ClocksTypeDef ClockInfo;
int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //2个抢占优先级  2个响应优先级
	IO_Init();
	uart_init(115200);
	SysTick_Config(SystemCoreClock / 1000);
	init_main();
	IWDG_Init(12,625);  //3 S
	while(1)
	{
		Time1s();
		GL_main();
		Motor_Driver();

		IO_Main();
		LED_play();
		IWDG_Feed();
//    if(SysWaitHMSM(0,0,0,1,0))
//      continue;
//		CTR485 = 1;
//		UartSend(USART2,5,ggg);
		//1 -- 运行  2 -- 升起  3 -- 下降  4 -- 上升遇阻  5 -- 下降遇阻
		Sys_Tick_1s = 0;

    //RCC_GetClocksFreq(&ClockInfo);

		
		//1s任务在上
		if(Sys_Tick_500ms)
		{
			Sys_Tick_500ms = 0;
			switch(sts.work_sts)
			{
				case 1:
					if(led_cmd == 2)
						led_cmd = 0;
					else
						led_cmd = 2;
					break;
				case 2:
				case 3:
					led_cmd = 3;
					break;
				case 4:
				case 5:
					if(led_cmd == 1)
						led_cmd = 0;
					else
						led_cmd = 1;
					break;
			}
		}
	}
}



