

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
int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //2个抢占优先级  2个响应优先级
	IO_Init();
	uart_init(115200);
	SysTick_Config(SystemCoreClock / 1000);
	TIM2_Int_Init(200,35);  //arr * psc / 72000000 = 0.0001 
	//TIM4_Cap_Init(0XFFFF,72-1);	//以1Mhz的频率计数 
	TIM3_Cap_Init(0XFFFF,72-1);	  //以1Mhz的频率计数 
	init_main();
	//IWDG_Init(12,625);          //3 S
	RADAR_ENABLE
	VOICE_ENABLE
	
	while(1)
	{
		Time1s();
		GL_main();
		Motor_Driver();

		IO_Main();
		//IWDG_Feed();
    if(SysWaitHMSM(0,0,0,1,0))
      continue;
    ERROR = ~ERROR;
		//CTR485 = 1;
		//UartSend(USART2,5,ggg);
	}
}



