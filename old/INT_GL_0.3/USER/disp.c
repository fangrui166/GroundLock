#include "global.h"
#include "usart.h"

#define LOCK_LIMIT 5   //电磁锁动作



/*电机驱动*/
void Motor_Driver(void)
{
	switch(cmd.motor)
	{
		case 1:
			PW2 = 0;
			PW1 = 1;
			break;
		case 2:
			PW2 = 1;
			PW1 = 0;
			break;
		default:
		  PW1 = 1;
      PW2 = 1;
		  break;
	}
}
/*1S计时*/
void Time1s(void)
{
  if(Sys_Tick_Count > 1000)
	{
		Sys_Tick_Count = 0;
		Sys_Tick_1s = 1;
	}
  
}
/*延时函数*/
/*************************************************************************
  Time Delay
 *************************************************************************/
void SysDelayHMSM (uint8_t hours,uint8_t minutes,uint8_t seconds,uint16_t ms)
{
  unsigned int temp;

  temp = SysTickCnt;
  while ((SysTickCnt - temp) < (3600000*(uint32_t)hours+60000*(uint32_t)minutes+1000*(uint32_t)seconds+ms));
}

uint8_t SysWaitHMSM (uint8_t timescnt,uint8_t hours,uint8_t minutes,uint8_t seconds,uint16_t ms)
{
  if (SysTickCnt - SysWait_Times[timescnt] < (3600000*(uint32_t)hours+60000*(uint32_t)minutes+1000*(uint32_t)seconds+ms))
    return 1; 
  else
  {
    SysWait_Times[timescnt] = SysTickCnt; 
    return 0;
  }
}

/*声音操作*/
void voice_deal()
{
	if(cmd.voice)
	{
		if(cmd.voice != 10)
		{
			if(cmd.voice == 9)
				cmd.voice = 1;
			else
			  cmd.voice ++;
		}
		
		switch(cmd.voice)
		{
			case 1:
				VOICE_NO1;
				break;
			case 2:
				VOICE_NO2;
				break;
			case 3:
				VOICE_NO3;
				break;
			case 4:
				VOICE_NO4;
				break;
			case 5:
				VOICE_NO5;
				break;
			case 6:
				VOICE_NO6;
				break;
			case 7:
				VOICE_NO7;
				break;
			case 8:
				VOICE_NO8;
				break;
			case 9:
				VOICE_NO9;
				break;
			case 10:
				VOICE_SET;
				break;
		}
		cmd.voice = 0;
	}

	if(uart[2].Sts == 1)
	{
		uart[2].Sts = 0;
		uart[2].length = 0;

		//配置声音最大的回复
		if(uart[2].buf[0] == 0xAA && uart[2].buf[1] == 0x02 && 
		   uart[2].buf[2] == 0x31 && uart[2].buf[3] == 0xEF)  //AA 02 31 EF
		{
		  sts.voice = 2;
		}
    //点播声音指令应答
		if(uart[2].buf[0] == 0xAA && uart[2].buf[1] == 0x02 && 
		   uart[2].buf[2] == 0x41 && uart[2].buf[3] == 0xEF)  //AA 02 41 EF
		{
      sts.voice = 1;
		}
    //声音播放完毕应答
		if(uart[2].buf[0] == 0xAA && uart[2].buf[1] == 0x02 && 
		   uart[2].buf[2] == 0x80 && uart[2].buf[3] == 0xEF)  //AA 02 80 EF 
		{
      sts.voice = 2;
		}
    memset(uart[2].buf,0,10);
	}
}


/*初始化判断*/
void init_GL(void)
{
	if(sts.init != 3)
	{
		if(!sts.car_radar)       //没有车
		{
			cmd.car_radar = 0;     //雷达关机
			if(key == 1 && !sts.init)
			{
				cmd.lock = 0;        //卡扣锁定
				if(!cmd.voice)
					cmd.voice = 1;     //语音播报初始化完毕
				if(sts.voice == 2)
				{
					cmd.voice = 0;
					sts.voice = 0;     
					sts.init = 2;      //初始化完成  
				}
			}
			else
			{
				if(sts.init != 2)
					sts.init = 1;  
				if(!cmd.voice)
					cmd.voice = 7;     //语音播报地锁将要上升
				if(sts.voice == 2 && sts.init != 2)
				{
					cmd.voice = 0;
					//sts.voice = 0;     
					cmd.lock = 1;      //卡扣解锁
					if(sts.lock == 1)  //锁扣已打开  动作电机
					{
						cmd.motor = 1;
					}
					else
						cmd.motor = 0;
					if(key == 1)
					{
						sts.voice = 0;   
						cmd.voice = 1;     //语音播报初始化完毕
						sts.init = 2;  
					}
				}
				else if(sts.voice == 2)
				{
					sts.init = 3;
					sts.voice = 0; 
					cmd.voice = 0;
					cmd.motor = 0;
					cmd.lock = 0;
				}

			}
		}
		else
			cmd.car_radar = 1;     //雷达开机
	}
		
}

/*雷达开机*/
void Start()
{
	static uint8_t voice_set = 0;
  static u8 Count = 0;    //超时计数
  static u8 Time_en = 0;  //超时机制使能
	
	/*声音设置成最大*/
	if(!voice_set && sts.voice == 2)
	{
		voice_set = 1;
	}

	/*声音初始化为最大*/
	if(!voice_set && !Count)
	{
		cmd.voice = 10; 
		sts.voice = 0;
		Count = 15;
		Time_en = 1;
	}

	if(!sts.init && voice_set)
  {
    sts.init = 1;          //初始化中
		cmd.lock = 0;          //锁卡扣
		sts.work_sts = 1;      //模式切换为运行 -- 上升
    sts.run = 1;   
  }
  
  /*超时机制*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      voice_set = 1;   //超时置位 -- 防止模块坏掉死循环在这里
      Time_en = 0;     //超时标志清零
    }
  }
}

/*地锁上升处理*/
void Up_Deal()
{
  /*触发*/
  if((sts.work_old != sts.work_sts) && (sts.work_sts == 1) && (sts.run == 1))
	  sts.up_l = 1;         //开始处理

  /*升起结束判断*/
  if(key == 1 && sts.up_l)
  {
    cmd.motor = 0;        //关电机
		
    sts.work_sts = 2;     //系统状态切换为升起
    sts.up_l = 0;         //内部状态还原
  }
  /*升起遇阻判断*/
  if(sts.sense && sts.up_l)
  {
    cmd.motor = 0;        //关电机
		/*不等待统一处理了*/
		{
			PW1 = 1;
			PW2 = 1;
		}
    sts.work_sts = 4;     //系统状态切换为上升遇阻
		sts.work_old = 0xff;  //保证遇阻处理状态能切进去
    sts.up_l = 0;         //内部状态还原
  }
	/*无车后升起地锁*/
	if(sts.up_l == 1)
	{
    cmd.car_radar = 0; //雷达关机
    cmd.motor = 1;     //上升电机
	}
}
/*上升遇阻处理*/
void ResisUp_Deal()
{
  static u8 Count = 0;    //超时计数
  static u8 Time_en = 0;  //超时机制使能
	static uint32_t sys_stick = 0; 
  /*触发*/
  if((sts.work_old != sts.work_sts) && (sts.work_sts == 4))
    sts.resi_l = 1;    //开始处理
  /*下降地锁 -- 这里不用考虑地锁有没有在原位没动*/
  if(sts.resi_l == 1)
  {
    cmd.motor = 2;     //下降电机
		sys_stick = SysTickCnt;
		sts.resi_l = 2;
  }
  if((sts.resi_l == 2) && (key == 2 || sts.sense) && ((SysTickCnt - sys_stick) > 500))
  {
    sts.resi_l = 4;    //状态进阶
  }
  /*设定超时时间*/
  if(sts.resi_l == 4)
  {
    cmd.motor = 0;     //关掉电机
    Count = 10;        //设定超时时间
    Time_en = 1;
		sts.resi_l = 5;    //状态进阶
  }
  /*上升地锁  -- 无超时机制*/
  if(sts.resi_l == 6)
  {
    Time_en = 0;         //超时标志清零
		sts.work_sts = 1;
		sts.run = 1;
		cmd.sts_copy = 1;    //禁止历史状态赋值
		
		sts.resi_l = 0;    //状态清零
  }

  /*超时机制*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.resi_l ++;   //状态加一
      Time_en = 0;     //超时标志清零
    }
  }
    
}
/*地锁下降处理*/
void Down_Deal()
{
  /*触发*/
  if((sts.work_old != sts.work_sts) && (sts.work_sts == 1) && (sts.run == 2))
    sts.down_l = 1;         //开始处理

  /*下降结束判断*/
  if(key == 2 && sts.down_l)
  {
    cmd.motor = 0;        //关电机
    sts.work_sts = 3;     //系统状态切换为下降
    sts.down_l = 0;       //内部状态还原
  }
  /*下降遇阻判断*/
  if(sts.sense && sts.down_l)
  {
    cmd.motor = 0;        //关电机
		/*不等待循环关电机了*/
		{
			PW1 = 1;
			PW2 = 1;
		}
    sts.work_sts = 5;     //系统状态切换为下降遇阻
	  sts.work_old = 0xff;  //保证遇阻处理状态能进去
    sts.down_l = 0;       //内部状态还原
  }

  if(sts.down_l == 1)
    cmd.motor = 2;     //下降电机

}
/*下降遇阻处理*/
void ResisDown_Deal()
{
  static u8 Count = 0;    //超时计数
  static u8 Time_en = 0;  //超时机制使能
  /*触发*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 5)
  {
    sts.residown_l = 1;   //开始处理
  }

  /*等待超时时间到后下降地锁*/
  if(sts.residown_l == 1)
  {
    cmd.motor = 0;     //关掉电机
    Count = 10;         //设定超时时间
    Time_en = 1;
		sts.residown_l = 4;//状态进阶
  }
  /*下降地锁  -- 无超时机制*/
  if(sts.residown_l == 5)
  {
    Time_en = 0;         //超时标志清零
		sts.work_sts = 1 ;
		sts.run = 2;
		cmd.sts_copy = 1;
		sts.residown_l = 6;//状态进阶
		
  }

  /*超时机制*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.residown_l ++;   //状态加一
      Time_en = 0;     //超时标志清零
    }
  }
}

/*地锁被强制升起 按下判断*/
void GL_Forced()
{
  static u8 Count = 0;    //超时计数
	/*地锁升起状态被按下*/
	if(sts.work_sts == 2 && key != 1 && !Count)
	{
		Count = 5;
	}
	/*地锁下降状态被抬起*/
	if(sts.work_sts == 3 && key != 2 && !Count)
	{
		Count = 5;
	}
  /*超时机制*/
  if(Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
			//重新上升
			if(sts.work_sts == 2 && key != 1)
			{
				sts.work_sts = 1;
				sts.run = 1;
			}
			//重新下降
			else if(sts.work_sts == 3 && key != 2)
			{
				sts.work_sts = 1;
				sts.run = 2;
			}	
    }
  }
}



/*命令解析函数*/
void Cmd_Analy()
{
	/*上升解析*/
	if(cmd.ctrl == 1)
	{
    if((sts.work_sts == 1 && sts.run == 1) || sts.work_sts == 4)
    ;
    else
    {
      /*停掉电机*/
      {
        PW1 = 1;
        PW2= 1;
      }
			/*下降遇阻的状态清零*/
			sts.residown_l = 0;
      sts.work_sts = 1;
      sts.run = 1;
			sts.work_old = 0xff; //保证肯定能执行到
    }
	}
	/*下降解析*/
	if(cmd.ctrl == 2)
	{
    if((sts.work_sts == 1 && sts.run == 2) || sts.work_sts == 5)
    ;
    else
    {
      /*停掉电机*/
      {
        PW1 = 1;
        PW2= 1;
      }
			/*上升遇阻的状态清零*/
			sts.resi_l = 0;
      sts.work_sts = 1;
      sts.run = 2;
			sts.work_old = 0xff; //保证肯定能执行到
    }
	}
	cmd.ctrl = 0;
}





/*地锁处理主函数*/
void GL_main(void)
{  
	Interface();          //接口函数
	Cmd_Analy();          //命令解析
	GL_Forced();          //强制抬起降下处理函数
	Up_Deal();            //地锁上升处理
	ResisUp_Deal();       //地锁上升遇阻处理
	Down_Deal();          //地锁下降处理
	ResisDown_Deal();     //下降遇阻处理
	if(!cmd.sts_copy)     //copy管理
		sts.work_old = sts.work_sts;
	else
		cmd.sts_copy = 0;
}

/*全局初始化*/
void init_main(void)
{
	sts.work_sts = 1;     //模式切换为运行 -- 上升
	sts.run = 1;   
}





