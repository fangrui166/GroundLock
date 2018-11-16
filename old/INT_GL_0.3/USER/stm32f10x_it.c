/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "global.h"
 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}


/*滴答定时器1ms中断*/
void SysTick_Handler(void)
{
	SysTickCnt ++;      //滴答定时器
	Sys_Tick_Count ++;  //1s专用计数     
  if(!(SysTickCnt % 500))
    Sys_Tick_500ms = 1;	
}

/*定时器2中断服务程*/  
void TIM2_IRQHandler(void)    //100 HZ -- 5MS
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志 
	}
}
/*定时器3中断服务程*/
void TIM3_IRQHandler(void)  
{
 	if((radar.ECHO_Sts[0] & 0X80)==0)//还未成功捕获	
	{	  
		if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
		{	    
			if(radar.ECHO_Sts[0] & 0X40)//已经捕获到高电平了
			{
				if((radar.ECHO_Sts[0] & 0X01) == 0X01)//高电平太长了
				{
					radar.ECHO_Sts[0] |= 0X80;//标记成功捕获了一次
					radar.ECHO_Val[0] = 0XFFFF;
				}
				else 
					radar.ECHO_Sts[0] ++;
			}	 
		}
		if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
		{	
			if(radar.ECHO_Sts[0] & 0X40)		//捕获到一个下降沿 		
			{	  			
				radar.ECHO_Sts[0] |= 0X80;		//标记成功捕获到一次高电平脉宽
				radar.ECHO_Val[0] = TIM_GetCapture1(TIM3);
				TIM_OC1PolarityConfig(TIM3,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
			}
			else  								//还未开始,第一次捕获上升沿
			{
				radar.ECHO_Sts[0] = 0;			//清空
				radar.ECHO_Val[0] = 0;
				TIM_SetCounter(TIM3,0);
				radar.ECHO_Sts[0] |= 0X40;		//标记捕获到了上升沿
				TIM_OC1PolarityConfig(TIM3,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
			}		    
		}	
		/*无sensor超时处理*/
		if(radar.Time[0])
		{
			radar.Time[0] --;
		}
		if(!radar.Time[0])
		{
			radar.ECHO_Sts[0] |= 0X80;//标记成功捕获了一次
			radar.ECHO_Val[0] = 0XFFFF;
		}
 	}
  sts.stick_radar = SysTickCnt;   //侦测间隔用
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
}
/*定时器4中断服务程*/
void TIM4_IRQHandler(void)   
{
 	if((radar.ECHO_Sts[1] & 0X80)==0)//还未成功捕获	
	{	  
		if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
		{	    
			if(radar.ECHO_Sts[1] & 0X40)//已经捕获到高电平了
			{
				if((radar.ECHO_Sts[1] & 0X01) == 0X01)//高电平太长了
				{
					radar.ECHO_Sts[1] |= 0X80;//标记成功捕获了一次
					radar.ECHO_Val[1] = 0XFFFF;
				}
				else 
					radar.ECHO_Sts[1] ++;
			}	 
		}
		if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
		{	
			if(radar.ECHO_Sts[1] & 0X40)		//捕获到一个下降沿 		
			{	  			
				radar.ECHO_Sts[1] |= 0X80;		//标记成功捕获到一次高电平脉宽
				radar.ECHO_Val[1] = TIM_GetCapture1(TIM4);
				TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
			}
			else  								//还未开始,第一次捕获上升沿
			{
				radar.ECHO_Sts[1] = 0;			//清空
				radar.ECHO_Val[1] = 0;
				TIM_SetCounter(TIM4,0);
				radar.ECHO_Sts[1] |= 0X40;		//标记捕获到了上升沿
				TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
			}		    
		}		
		/*无sensor超时处理*/
		if(radar.Time[1])
		{
			radar.Time[1] --;
		}
		if(!radar.Time[1])
		{
			radar.ECHO_Sts[1] |= 0X80;//标记成功捕获了一次
			radar.ECHO_Val[1] = 0XFFFF;
		}		
 	}
 
	TIM_ClearITPendingBit(TIM4, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
