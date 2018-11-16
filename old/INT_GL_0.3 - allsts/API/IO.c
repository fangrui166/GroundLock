#include "IO.h"
#include "global.h"
#include "delay.h"
RDAR_STRUCT radar;
/*************************************************************
  Function:    
  Description: 地锁 升起、下降信号读取引脚初始化
  Calls:       
  Called By:   
  Input:       
  Output:      
  Return:      
  Others:      初始化 PA5、PA6
*************************************************************/
void KEY_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 

 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //---PB5 PB6
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}
/*************************************************************
  Function:    KEY_Scan
  Description: 地锁 升起、下降信号读取引脚读取
  Calls:       
  Called By:   
  Input:       
  Output:      
  Return:      
  Others:      初始化 PA5、PA6  一次只能读取一个引脚
*************************************************************/
u8 KEY_Scan(u8 mode)
{	 
	static uint8_t key_check = 0;
  /*过流检测 -- 去抖*/	
	if(SENSE)
	{
		SysDelayHMSM(0,0,0,500);
		if(SENSE)
		{
			SysDelayHMSM(0,0,0,500);
			if(SENSE)
				;//sts.sense = 1;
		}
	}
	else
		sts.sense = 0;
  /*地磁车位检测*/
  //从无车到有车
  if(NC_ERR && CAR_MODLE == 2) //高电平代表无异常
  {
    if(NC_STS && sts.car_radar != 1)
    {
      SysDelayHMSM(0,0,0,10);
      if(NC_STS)
      {
        SysDelayHMSM(0,0,0,10);
				if(NC_STS)
          sts.car_radar = 1;
      }
    }
    //从有车到无车
    else if(!NC_STS && sts.car_radar)
    {
      SysDelayHMSM(0,0,0,10);
      if(!NC_STS)
      {
        SysDelayHMSM(0,0,0,10);
				if(!NC_STS)
          sts.car_radar = 0;
      }
    }
  }
	//从电磁检测转换到超声波检测
  else if(!NC_ERR && CAR_MODLE == 2)
  {
    CAR_MODLE = 1;
  }
	
	
	//从超声波检测转换到电磁检测
	if(NC_ERR && CAR_MODLE == 1)
	{
		CAR_MODLE = 2;
	}
	//地磁故障
	if(!NC_ERR)
		sts.err.geo_1b = 1;
	else if(NC_ERR)
		sts.err.geo_1b = 0;
	
	//限位开关检测
	key_check = 0;
	if(mode && (SW_UP || SW_DOWN))
	{
		//SysDelayHMSM(0,0,0,10);
		if(SW_UP)
			key_check = 1;
			//return 1;
		else if(SW_DOWN)
			key_check = 2;
			//return 2;
	}	    
	
	//限位板故障判断
	if(SW_UP && SW_DOWN && !sts.err.limit_board_1b)
	{
	  SysDelayHMSM(0,0,0,10);
		if(SW_UP && SW_DOWN)
		{
			sts.err.limit_board_1b = 1;
		}
	}
	else
    sts.err.limit_board_1b = 0;
 	return key_check;
}
/*************************************************************
  Function:    OutIo_Init
  Description: 输出引脚初始化
  Calls:       
  Called By:   
  Input:       
  Output:      
  Return:      
  Others:      初始化 PA1
*************************************************************/
void OutIo_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//初始化PA1 -- error led PA4 -- 485
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4;				     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					     
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
	
	//初始化PB15 -- 电磁锁  PB1 -- 功放 PB7 -- 雷达 
	//------PB3  -- TRIG1   PB5 -- TRIG2
	//------PB12 -- PW2     PB13 -- PW1
	//------PB8
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_1 | GPIO_Pin_3  | GPIO_Pin_7 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_8;				      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOB, &GPIO_InitStructure);					     			          
	GPIO_ResetBits(GPIOB,GPIO_Pin_15 | GPIO_Pin_7 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_8 | GPIO_Pin_3);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
}



/*************************************************************
  Function:    IO_Init
  Description: 输入输出引脚初始化
  Calls:       
  Called By:   
  Input:       
  Output:      
  Return:      
  Others:      
*************************************************************/
void IO_Init(void)
{
	KEY_Init();
	OutIo_Init();
}
/*************************************************************
  Function:    Radar_Deal
  Description: 测距
  Calls:       
  Called By:   
  Input:       
  Output:      
  Return:      
  Others:      
*************************************************************/
void Radar_Deal(void)
{
  static u8 cmd_old = 0;       //历史值存储
	static u8 count_car = 0;     //有无车计数
	static u8 car_store;         //历史值存储
	static u8 car_modle_old;     //车位检测历史值
	u32 Temp_Destance = 0;
	/*命令触发*/
	if(radar.cmd == 1 && ((SysTickCnt - sts.stick_radar) >= 100))
	{
	  radar.cmd = 0;
		Lock_ON(TRIG_1);
    sts.stick_radar = SysTickCnt;     
		memset(radar.sts,1,2);         //雷达状态置位
		memset(radar.Time,5,2);        //雷达超时复位
	  //TIM4_Cap_Init(0XFFFF,72-1);	 //以1Mhz的频率计数 
	  TIM3_Cap_Init(0XFFFF,72-1);	   //以1Mhz的频率计数 
		TIM_Cmd(TIM3,ENABLE ); 	       //使能定时器3
		//TIM_Cmd(TIM4,ENABLE ); 	  //使能定时器4
		SysDelayHMSM(0,0,0,5);
    Lock_OFF(TRIG_1);
    //Lock_OFF(TRIG_2);
	}
	/*通道1雷达数据*/
  if(radar.ECHO_Sts[0] & 0X80)
	{
	  TIM_Cmd(TIM3,DISABLE);
		Temp_Destance = ((radar.ECHO_Sts[0] & 0X3F) * 65536 ) + radar.ECHO_Val[0];
		//探测距离大于600cm 或者 小于20cm
		if((Temp_Destance >= 35000) || (Temp_Destance < 1100))
		{
			radar.Destance[0] = 0;
		}
		else
			radar.Destance[0] = Temp_Destance * 340 / 10000 / 2;
		radar.ECHO_Sts[0] = 0;
		radar.ECHO_Val[0] = 0;
		radar.sts[0] = 2;
	}
	/*通道二雷达数据*/
  /*if(radar.ECHO_Sts[1] & 0X80)
	{
		Temp_Destance = ((radar.ECHO_Sts[1] & 0X3F) * 65536 ) + radar.ECHO_Val[1];
		//探测距离大于600cm 或者 小于20cm
		if((Temp_Destance >= 35000) || (Temp_Destance < 1100))
		{
			radar.Destance[1] = 0;
		}
		else
			radar.Destance[1] = Temp_Destance * 340 / 10000 / 2;
		radar.ECHO_Sts[1] = 0;
		radar.ECHO_Val[1] = 0;
		radar.sts[1] = 2;
	  TIM_Cmd(TIM4,DISABLE ); 	//使能定时器3
	}*/
	/*有无车接口输出*/
	if(radar.sts[0] == 2)// && radar.sts[1] == 2)
	{
	  radar.sts[0] = 0;
		//memset(radar.sts,0,2);
		if(!radar.Destance[0])// && !radar.Destance[1])
			car_store |= (1 << count_car);     //历史值存储
		/*else if(!radar.Destance[1])
    {
			if(radar.Destance[0] <= CAR_DISTANCE && radar.Destance[0])
				car_store |= (1 << count_car);   //历史值存储
				
			else if(radar.Destance[0] > CAR_DISTANCE && radar.Destance[0])
				car_store &= (0 << count_car);   //历史值存储
		}
		else if(!radar.Destance[0])
    {
			if(radar.Destance[1] <= CAR_DISTANCE && radar.Destance[1])
			  car_store |= (1 << count_car);   //历史值存储
			else if(radar.Destance[1] > CAR_DISTANCE && radar.Destance[1])
				car_store &= (0 << count_car);   //历史值存储
		}*/
		else
		{
			if(radar.Destance[0] <= CAR_DISTANCE)// || radar.Destance[1] <= CAR_DISTANCE)
				car_store |= (1 << count_car);   //历史值存储
			else
				car_store &= (0 << count_car);   //历史值存储
		}
		/*输出结果*/
		if(count_car >= 4)
		{				
			count_car = 0;
			if(CAR_MODLE == 1)
			{
				radar.cmd = 1;
				if(car_store && 0x1f != 0)
					sts.car_radar = 1;
				else
					sts.car_radar = 0;
		  }
		}
		else
		{
			radar.cmd = 1;
			count_car ++;
		}
	}
	/*触发超声波检测 -- 情景：地磁坏掉，超声波立即接替检测车位*/
	if((car_modle_old != CAR_MODLE) && (CAR_MODLE == 1))
	{
	  radar.cmd = 1;
	}
	car_modle_old = CAR_MODLE;

	/*有无车检测触发 -- 间隔5S执行一次
	if(Sys_Tick_1s && cmd.car_radar && CAR_MODLE == 1)
	{
		check_count ++;
		if(check_count >= 3)
		{
			check_count = 0;
			radar.cmd = 1;
		}
	}*/
	
//	/*车辆检测触发*/
//	if(cmd_old != cmd.car_radar && cmd.car_radar && CAR_MODLE == 1)
//	{
//		radar.cmd = 1;
//		count_car = 0; //存储计数清零
//    
//	}
//	if(!cmd.car_radar)
//	{
//		radar.cmd = 0;
//	}
//	cmd_old = cmd.car_radar;
	
	//2017 - 11 - 12  加   不使用雷达测距
	//2018 - 01 - 15  加   增加禁止判断车位标志
	if(CAR_MODLE == 3)
	  sts.car_radar = 0;
}
/*************************************************************
  Function:    IO_Init
  Description: 输入输出引脚初始化
  Calls:       
  Called By:   
  Input:       
  Output:      
  Return:      
  Others:      
*************************************************************/
void IO_Main(void)
{
	key = KEY_Scan(1);
	Radar_Deal();
}
/*************************************************************
  Function:    LED_ON  LED_OFF
  Description: 引脚输出控制
  Calls:       
  Called By:   
  Input:       
  Output:      
  Return:      
  Others:      
*************************************************************/
void LED_ON(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  GPIO_ResetBits(GPIOx,GPIO_Pin);
}
void LED_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  GPIO_SetBits(GPIOx,GPIO_Pin);
}
void Lock_ON(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  GPIO_SetBits(GPIOx,GPIO_Pin);
}
void Lock_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  GPIO_ResetBits(GPIOx,GPIO_Pin);
}





