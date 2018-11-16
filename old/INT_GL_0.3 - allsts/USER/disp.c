#include "global.h"
#include "usart.h"

#define LOCK_LIMIT 5   //电磁锁动作



/*电机驱动*/
void Motor_Driver(void)
{
	static u8 cmd_old = 0;
	if(cmd.motor != cmd_old && !cmd_old)
		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	if(cmd.motor != cmd_old && !cmd.motor)
	{
		TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
		PW1 = 0;
		PW2 = 0;
	}
	cmd_old = cmd.motor;
}
/*1S计时*/
void Time1s(void)
{
  if(Sys_Tick_Count > 1000)
	{
		Sys_Tick_Count = 0;
		Sys_Tick_1s = 1;
	}
	else
		Sys_Tick_1s = 0;
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
			sts.err.voice_1b = 0;
		}
    //点播声音指令应答
		if(uart[2].buf[0] == 0xAA && uart[2].buf[1] == 0x02 && 
		   uart[2].buf[2] == 0x41 && uart[2].buf[3] == 0xEF)  //AA 02 41 EF
		{
      sts.voice = 1;
			sts.err.voice_1b = 0;
		}
    //声音播放完毕应答
		if(uart[2].buf[0] == 0xAA && uart[2].buf[1] == 0x02 && 
		   uart[2].buf[2] == 0x80 && uart[2].buf[3] == 0xEF)  //AA 02 80 EF 
		{
      sts.voice = 2;
			sts.err.voice_1b = 0;
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
			sts.err.voice_1b = 1;
      Time_en = 0;     //超时标志清零
    }
  }
}

/*地锁上升处理*/
void Up_Deal()
{
  static u8 Count = 0;    //超时计数
  static u8 Time_en = 0;  //超时机制使能
  /*触发*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 1 && sts.run == 1)
  {
	  sts.up_l = 1;         //开始处理
  }

  /*升起结束判断*/
  if(key == 1 && sts.up_l)
  {
    cmd.motor = 0;        //关电机
    cmd.lock = 0;         //锁卡扣
    sts.work_sts = 2;     //系统状态切换为升起
    sts.up_l = 0;         //内部状态还原
  }
  /*升起遇阻判断*/
  if(sts.sense && sts.up_l)
  {
    cmd.motor = 0;        //关电机
		/*不等待统一处理了*/
		{
			PW1 = 0;
			PW2 = 0;
		}
    sts.work_sts = 4;     //系统状态切换为上升遇阻
		sts.work_old = 0xff;  //保证遇阻处理状态能切进去
    sts.up_l = 0;         //内部状态还原
  }
	/*检测有无车辆*/
	if(sts.up_l == 1)
	{
		cmd.motor = 0;        //关掉电机
	  cmd.lock = 0;         //锁卡扣
    cmd.car_radar = 1;    //雷达开机 -- 切换车状态为未知
    //sts.car_radar = 3;
		sts.up_l = 2;         //状态进阶
	}
	/*播放声音 -- 这一步有超时机制*/
	if(sts.up_l == 2 && !sts.car_radar)
	{
    cmd.car_radar = 0;    //雷达关机
    cmd.motor = 0;        //关掉电机
    sts.voice = 0;        //播放声音 -- 请注意，地锁将要上升
    cmd.voice = 7;     
    Count = 8;            //设定超时时间
    Time_en = 1;
    sts.up_l = 3;         //状态改变
	}
	/*声音播放结束进阶*/
  if(sts.up_l == 3 && sts.voice == 2)
  {
    Time_en = 0;
    sts.up_l = 4;
  }
	/*单独设定一个状态的原因是防止声音播放模块坏掉*/
	/*再次检测有无车辆*/
	if(sts.up_l == 4)
	{
    cmd.car_radar = 1;    //雷达开机 -- 切换车状态为未知
    //sts.car_radar = 3;
		sts.up_l = 5;         //状态进阶
	}
	/*无车后升起地锁*/
	if(sts.up_l == 5 && !sts.car_radar)
	{
    cmd.car_radar = 0; //雷达关机
    cmd.lock = 1;      //打开卡扣
    cmd.motor = 1;     //上升电机
    Time_en = 0;       //超时标志清零
	}

  /*超时机制*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.up_l ++;   //状态加一
      Time_en = 0;     //超时标志清零
    }
  }
}
/*上升遇阻处理*/
void ResisUp_Deal()
{
  static u8 Count = 0;    //超时计数
  static u8 Time_en = 0;  //超时机制使能
  /*触发*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 4)
  {
    sts.resi_l = 1;    //开始处理
  }
  /*播放声音 -- 这一步有超时机制*/
  if(sts.resi_l == 1)
  {
    cmd.motor = 0;     //关掉电机
		if(key != 2)       //判断地锁有无升起来
		{
			sts.voice = 0;   //播放声音 -- 地锁上升遇阻现在将要下降
			cmd.voice = 2;     
		}
		else               //地锁原地未动，但是判断上升遇阻
		{
			sts.voice = 0;  
			cmd.voice = 8;     
		}
    sts.resi_l = 2;    //状态改变
    Count = 8;         //设定超时时间
    Time_en = 1;
  }
  /*等到声音播放结束  -- 有超时机制*/
  if(sts.resi_l == 2 && sts.voice == 2)
  {
    sts.resi_l = 3;    //操作状态进阶
		sts.sense = 0;     //遇阻状态清零
    Time_en = 0;       //超时标志清零
  }
  /*下降地锁 -- 这里不用考虑地锁有没有在原位没动*/
  if(sts.resi_l == 3)
  {
    cmd.lock = 1;      //打开卡扣
    cmd.motor = 2;     //下降电机
    Time_en = 0;       //超时标志清零
  }
  if(sts.resi_l == 3 && (key == 2 || sts.sense))
  {
    sts.resi_l = 4;    //状态进阶
  }
  /*设定超时时间*/
  if(sts.resi_l == 4)
  {
    cmd.motor = 0;     //关掉电机
    cmd.lock = 0;      //锁卡扣
    Count = 20;        //设定超时时间
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
/*上升OK处理*/
void UpOK_Deal()
{
  static u8 Count = 0;    //超时计数
  static u8 Time_en = 0;  //超时机制使能
  /*触发*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 2)
  {
    sts.upok_l = 1;      //开始处理
  }
  /*播放声音*/
  if(sts.upok_l == 1)
  {
    cmd.lock = 0;        //锁卡扣
    if(sts.init != 2)
    {
      sts.voice = 0;     //播放声音 -- 初始化完成，欢迎使用
      cmd.voice = 1; 
			sts.init = 2;      //初始化标志置位
    }
    else
    {
      sts.voice = 0;     //播放声音 -- 欢迎再次使用
      cmd.voice = 6; 
    }
    Count = 5;           //设定超时时间
    Time_en = 1;
    sts.upok_l = 2;      //状态改变
  }
  /*等到声音播放结束*/
  if(sts.upok_l == 2 && sts.voice == 2)
  {
    sts.upok_l = 3;      //操作状态进阶
    Time_en = 0;         //超时标志清零
  }
  /*超时机制*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.upok_l ++;   //状态加一
      Time_en = 0;     //超时标志清零
    }
  }
}
/*地锁下降处理*/
void Down_Deal()
{
  static u8 Count = 0;    //超时计数
  static u8 Time_en = 0;  //超时机制使能
  /*触发*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 1 && sts.run == 2)
  {
    sts.down_l = 1;         //开始处理
  }

  /*下降结束判断*/
  if(key == 2 && sts.down_l)
  {
    cmd.motor = 0;        //关电机
    cmd.lock = 0;         //锁卡扣
    sts.work_sts = 3;     //系统状态切换为下降
    sts.down_l = 0;       //内部状态还原
  }
  /*下降遇阻判断*/
  if(sts.sense && sts.down_l)
  {
    cmd.motor = 0;        //关电机
		/*不等待循环关电机了*/
		{
			PW1 = 0;
			PW2 = 0;
		}
    sts.work_sts = 5;     //系统状态切换为下降遇阻
	  sts.work_old = 0xff;  //保证遇阻处理状态能进去
    sts.down_l = 0;       //内部状态还原
  }
  /*播放声音 -- 这一步有超时机制*/
  if(sts.down_l == 1)
  {
    cmd.motor = 0;        //关掉电机
		if(sts.residown_l == 6)//播放声音 -- 请注意，地锁将要下降
		{
			sts.voice = 0;       
			cmd.voice = 9;     
		}
		else
		{
			sts.voice = 0;        //播放声音 -- 欢迎使用，地锁将要下降
			cmd.voice = 5;     
		}
    sts.down_l = 2;       //状态改变
    Count = 5;            //设定超时时间
    Time_en = 1;
  }
  if(sts.down_l == 2 && sts.voice == 2)
  {
    sts.down_l = 3;
    Time_en = 0;
  }
  if(sts.down_l == 3)
  {
    cmd.lock = 1;      //打开卡扣
    cmd.motor = 2;     //下降电机
    Time_en = 0;       //超时标志清零
  }

  /*超时机制*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.down_l ++;   //状态加一
      Time_en = 0;     //超时标志清零
    }
  }
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
  /*播放声音 -- 这一步有超时机制*/
  if(sts.residown_l == 1)
  {
    cmd.motor = 0;     //关掉电机
    sts.voice = 0;     //播放声音 -- 地锁下降遇阻
    cmd.voice = 4;     
    sts.residown_l = 2;//状态改变
    Count = 5;         //设定超时时间
    Time_en = 1;
  }
  /*等到声音播放结束*/
  if(sts.residown_l == 2 && sts.voice == 2)
  {
    sts.residown_l = 3;//操作状态进阶
		sts.sense = 0;     //遇阻状态清零
    Time_en = 0;       //超时标志清零
  }
  /*等待超时时间到后下降地锁*/
  if(sts.residown_l == 3)
  {
    cmd.motor = 0;     //关掉电机
    cmd.lock = 0;      //锁卡扣
    Count = 20;         //设定超时时间
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
/*下降OK处理*/
void DownOK_Deal()
{
  static u8 Count = 0;    //超时计数
  static u8 Time_en = 0;  //超时机制使能
  /*触发*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 3)
  {
    sts.downok_l = 1;      //开始处理
  }
  /*播放声音*/
  if(sts.downok_l == 1)
  {
    cmd.lock = 0;        //锁卡扣
    sts.voice = 0;       //播放声音 -- 地锁下降成功，请泊车
    cmd.voice = 3; 
    Count = 5;           //设定超时时间
    Time_en = 1;
    sts.downok_l = 2;    //状态改变
  }
  /*等到声音播放结束*/
  if(sts.downok_l == 2 && sts.voice == 2)
  {
    sts.downok_l = 3;    //操作状态进阶
    Time_en = 0;         //超时标志清零
  }
  /*超时机制*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.downok_l ++;   //状态加一
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

/*卡扣处理函数*/
void Lock_Deal()
{
	static u8 count = 0;
	static u8 limit = LOCK_LIMIT;
	//--不用卡扣  2017-12-26加
	if(cmd.lock == 1)
	{
		if(Sys_Tick_1s)
			count ++;
		if(limit == LOCK_LIMIT)
		{
			Lock_ON(LOCK);
			sts.lock = 1;
		}
		else
		{
			Lock_OFF(LOCK);
			//sts.lock = 2;   //2017年-11-11  屏蔽  去掉了电磁锁所以屏蔽
		}
		if(count >= limit)
		{
			count = 0;
			if(limit == LOCK_LIMIT)
				limit = 2;
			else
				limit = LOCK_LIMIT;
		}
	}
	else if(!cmd.lock)
	{
		limit = LOCK_LIMIT;
		Lock_OFF(LOCK);
		count = 0;
	}
}

/*命令解析函数*/
void Cmd_Analy()
{
	if(sts.init != 2 && cmd.ctrl)
	{
	  cmd.ctrl = 0;
	  cmd.ctrl= 0;
	}
	/*上升解析*/
	if(cmd.ctrl == 1)
	{
    if((sts.work_sts == 1 && sts.run == 1) || sts.work_sts == 4)
    ;
    else
    {
      /*停掉电机*/
      {
        PW1 = 0;
        PW2= 0;
      }
			/*下降遇阻的状态清零*/
			sts.residown_l = 0;
      sts.work_sts = 1;
      sts.run = 1;
			sts.work_old = 0xff; //保证肯定能执行到
      sts.work_old = 0;
    }
	}
	/*下降解析*/
	if(cmd.ctrl == 2)
	{
    if(sts.work_sts == 1 && sts.run == 2 || sts.work_sts == 5)
    ;
    else
    {
      /*停掉电机*/
      {
        PW1 = 0;
        PW2= 0;
      }
			/*上升遇阻的状态清零*/
			sts.resi_l = 0;
      sts.work_sts = 1;
      sts.run = 2;
			sts.work_old = 0xff; //保证肯定能执行到
      sts.work_old = 0;
    }
	}
	cmd.ctrl = 0;
}





/*地锁处理主函数*/
void GL_main(void)
{
	//init_GL();
	voice_deal();         //声音驱动
	GL_Forced();          //强制抬起降下处理函数
	Lock_Deal();          //电磁机构驱动
  Interface();          //接口函数
	Start();              //初始化
	Cmd_Analy();          //命令解析
	Up_Deal();            //地锁上升处理
	ResisUp_Deal();       //地锁上升遇阻处理
	UpOK_Deal();          //上升OK处理
	Down_Deal();          //地锁下降处理
	ResisDown_Deal();     //下降遇阻处理
	DownOK_Deal();        //下降OK处理
	if(!cmd.sts_copy)     //copy管理
		sts.work_old = sts.work_sts;
	else
		cmd.sts_copy = 0;
}

/*全局初始化*/
void init_main(void)
{
	sts.car_radar = 3;    //雷达检测初始化为阻态状态
	CAR_MODLE = 3;        //不检测车位    
}





