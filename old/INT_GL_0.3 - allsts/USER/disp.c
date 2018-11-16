#include "global.h"
#include "usart.h"

#define LOCK_LIMIT 5   //���������



/*�������*/
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
/*1S��ʱ*/
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
/*��ʱ����*/
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

/*��������*/
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

		//�����������Ļظ�
		if(uart[2].buf[0] == 0xAA && uart[2].buf[1] == 0x02 && 
		   uart[2].buf[2] == 0x31 && uart[2].buf[3] == 0xEF)  //AA 02 31 EF
		{
		  sts.voice = 2;
			sts.err.voice_1b = 0;
		}
    //�㲥����ָ��Ӧ��
		if(uart[2].buf[0] == 0xAA && uart[2].buf[1] == 0x02 && 
		   uart[2].buf[2] == 0x41 && uart[2].buf[3] == 0xEF)  //AA 02 41 EF
		{
      sts.voice = 1;
			sts.err.voice_1b = 0;
		}
    //�����������Ӧ��
		if(uart[2].buf[0] == 0xAA && uart[2].buf[1] == 0x02 && 
		   uart[2].buf[2] == 0x80 && uart[2].buf[3] == 0xEF)  //AA 02 80 EF 
		{
      sts.voice = 2;
			sts.err.voice_1b = 0;
		}
    memset(uart[2].buf,0,10);
	}
}


/*��ʼ���ж�*/
void init_GL(void)
{
	if(sts.init != 3)
	{
		if(!sts.car_radar)       //û�г�
		{
			cmd.car_radar = 0;     //�״�ػ�
			if(key == 1 && !sts.init)
			{
				cmd.lock = 0;        //��������
				if(!cmd.voice)
					cmd.voice = 1;     //����������ʼ�����
				if(sts.voice == 2)
				{
					cmd.voice = 0;
					sts.voice = 0;     
					sts.init = 2;      //��ʼ�����  
				}
			}
			else
			{
				if(sts.init != 2)
					sts.init = 1;  
				if(!cmd.voice)
					cmd.voice = 7;     //��������������Ҫ����
				if(sts.voice == 2 && sts.init != 2)
				{
					cmd.voice = 0;
					//sts.voice = 0;     
					cmd.lock = 1;      //���۽���
					if(sts.lock == 1)  //�����Ѵ�  �������
					{
						cmd.motor = 1;
					}
					else
						cmd.motor = 0;
					if(key == 1)
					{
						sts.voice = 0;   
						cmd.voice = 1;     //����������ʼ�����
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
			cmd.car_radar = 1;     //�״￪��
	}
		
}

/*�״￪��*/
void Start()
{
	static uint8_t voice_set = 0;
  static u8 Count = 0;    //��ʱ����
  static u8 Time_en = 0;  //��ʱ����ʹ��
	
	/*�������ó����*/
	if(!voice_set && sts.voice == 2)
	{
		voice_set = 1;
	}

	/*������ʼ��Ϊ���*/
	if(!voice_set && !Count)
	{
		cmd.voice = 10; 
		sts.voice = 0;
		Count = 15;
		Time_en = 1;
	}

	if(!sts.init && voice_set)
  {
    sts.init = 1;          //��ʼ����
		cmd.lock = 0;          //������
		sts.work_sts = 1;      //ģʽ�л�Ϊ���� -- ����
    sts.run = 1;   
  }
  
  /*��ʱ����*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      voice_set = 1;   //��ʱ��λ -- ��ֹģ�黵����ѭ��������
			sts.err.voice_1b = 1;
      Time_en = 0;     //��ʱ��־����
    }
  }
}

/*������������*/
void Up_Deal()
{
  static u8 Count = 0;    //��ʱ����
  static u8 Time_en = 0;  //��ʱ����ʹ��
  /*����*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 1 && sts.run == 1)
  {
	  sts.up_l = 1;         //��ʼ����
  }

  /*��������ж�*/
  if(key == 1 && sts.up_l)
  {
    cmd.motor = 0;        //�ص��
    cmd.lock = 0;         //������
    sts.work_sts = 2;     //ϵͳ״̬�л�Ϊ����
    sts.up_l = 0;         //�ڲ�״̬��ԭ
  }
  /*���������ж�*/
  if(sts.sense && sts.up_l)
  {
    cmd.motor = 0;        //�ص��
		/*���ȴ�ͳһ������*/
		{
			PW1 = 0;
			PW2 = 0;
		}
    sts.work_sts = 4;     //ϵͳ״̬�л�Ϊ��������
		sts.work_old = 0xff;  //��֤���账��״̬���н�ȥ
    sts.up_l = 0;         //�ڲ�״̬��ԭ
  }
	/*������޳���*/
	if(sts.up_l == 1)
	{
		cmd.motor = 0;        //�ص����
	  cmd.lock = 0;         //������
    cmd.car_radar = 1;    //�״￪�� -- �л���״̬Ϊδ֪
    //sts.car_radar = 3;
		sts.up_l = 2;         //״̬����
	}
	/*�������� -- ��һ���г�ʱ����*/
	if(sts.up_l == 2 && !sts.car_radar)
	{
    cmd.car_radar = 0;    //�״�ػ�
    cmd.motor = 0;        //�ص����
    sts.voice = 0;        //�������� -- ��ע�⣬������Ҫ����
    cmd.voice = 7;     
    Count = 8;            //�趨��ʱʱ��
    Time_en = 1;
    sts.up_l = 3;         //״̬�ı�
	}
	/*�������Ž�������*/
  if(sts.up_l == 3 && sts.voice == 2)
  {
    Time_en = 0;
    sts.up_l = 4;
  }
	/*�����趨һ��״̬��ԭ���Ƿ�ֹ��������ģ�黵��*/
	/*�ٴμ�����޳���*/
	if(sts.up_l == 4)
	{
    cmd.car_radar = 1;    //�״￪�� -- �л���״̬Ϊδ֪
    //sts.car_radar = 3;
		sts.up_l = 5;         //״̬����
	}
	/*�޳����������*/
	if(sts.up_l == 5 && !sts.car_radar)
	{
    cmd.car_radar = 0; //�״�ػ�
    cmd.lock = 1;      //�򿪿���
    cmd.motor = 1;     //�������
    Time_en = 0;       //��ʱ��־����
	}

  /*��ʱ����*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.up_l ++;   //״̬��һ
      Time_en = 0;     //��ʱ��־����
    }
  }
}
/*�������账��*/
void ResisUp_Deal()
{
  static u8 Count = 0;    //��ʱ����
  static u8 Time_en = 0;  //��ʱ����ʹ��
  /*����*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 4)
  {
    sts.resi_l = 1;    //��ʼ����
  }
  /*�������� -- ��һ���г�ʱ����*/
  if(sts.resi_l == 1)
  {
    cmd.motor = 0;     //�ص����
		if(key != 2)       //�жϵ�������������
		{
			sts.voice = 0;   //�������� -- ���������������ڽ�Ҫ�½�
			cmd.voice = 2;     
		}
		else               //����ԭ��δ���������ж���������
		{
			sts.voice = 0;  
			cmd.voice = 8;     
		}
    sts.resi_l = 2;    //״̬�ı�
    Count = 8;         //�趨��ʱʱ��
    Time_en = 1;
  }
  /*�ȵ��������Ž���  -- �г�ʱ����*/
  if(sts.resi_l == 2 && sts.voice == 2)
  {
    sts.resi_l = 3;    //����״̬����
		sts.sense = 0;     //����״̬����
    Time_en = 0;       //��ʱ��־����
  }
  /*�½����� -- ���ﲻ�ÿ��ǵ�����û����ԭλû��*/
  if(sts.resi_l == 3)
  {
    cmd.lock = 1;      //�򿪿���
    cmd.motor = 2;     //�½����
    Time_en = 0;       //��ʱ��־����
  }
  if(sts.resi_l == 3 && (key == 2 || sts.sense))
  {
    sts.resi_l = 4;    //״̬����
  }
  /*�趨��ʱʱ��*/
  if(sts.resi_l == 4)
  {
    cmd.motor = 0;     //�ص����
    cmd.lock = 0;      //������
    Count = 20;        //�趨��ʱʱ��
    Time_en = 1;
		sts.resi_l = 5;    //״̬����
  }
  /*��������  -- �޳�ʱ����*/
  if(sts.resi_l == 6)
  {
    Time_en = 0;         //��ʱ��־����
		sts.work_sts = 1;
		sts.run = 1;
		cmd.sts_copy = 1;    //��ֹ��ʷ״̬��ֵ
		
		sts.resi_l = 0;    //״̬����
  }

  /*��ʱ����*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.resi_l ++;   //״̬��һ
      Time_en = 0;     //��ʱ��־����
    }
  }
    
}
/*����OK����*/
void UpOK_Deal()
{
  static u8 Count = 0;    //��ʱ����
  static u8 Time_en = 0;  //��ʱ����ʹ��
  /*����*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 2)
  {
    sts.upok_l = 1;      //��ʼ����
  }
  /*��������*/
  if(sts.upok_l == 1)
  {
    cmd.lock = 0;        //������
    if(sts.init != 2)
    {
      sts.voice = 0;     //�������� -- ��ʼ����ɣ���ӭʹ��
      cmd.voice = 1; 
			sts.init = 2;      //��ʼ����־��λ
    }
    else
    {
      sts.voice = 0;     //�������� -- ��ӭ�ٴ�ʹ��
      cmd.voice = 6; 
    }
    Count = 5;           //�趨��ʱʱ��
    Time_en = 1;
    sts.upok_l = 2;      //״̬�ı�
  }
  /*�ȵ��������Ž���*/
  if(sts.upok_l == 2 && sts.voice == 2)
  {
    sts.upok_l = 3;      //����״̬����
    Time_en = 0;         //��ʱ��־����
  }
  /*��ʱ����*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.upok_l ++;   //״̬��һ
      Time_en = 0;     //��ʱ��־����
    }
  }
}
/*�����½�����*/
void Down_Deal()
{
  static u8 Count = 0;    //��ʱ����
  static u8 Time_en = 0;  //��ʱ����ʹ��
  /*����*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 1 && sts.run == 2)
  {
    sts.down_l = 1;         //��ʼ����
  }

  /*�½������ж�*/
  if(key == 2 && sts.down_l)
  {
    cmd.motor = 0;        //�ص��
    cmd.lock = 0;         //������
    sts.work_sts = 3;     //ϵͳ״̬�л�Ϊ�½�
    sts.down_l = 0;       //�ڲ�״̬��ԭ
  }
  /*�½������ж�*/
  if(sts.sense && sts.down_l)
  {
    cmd.motor = 0;        //�ص��
		/*���ȴ�ѭ���ص����*/
		{
			PW1 = 0;
			PW2 = 0;
		}
    sts.work_sts = 5;     //ϵͳ״̬�л�Ϊ�½�����
	  sts.work_old = 0xff;  //��֤���账��״̬�ܽ�ȥ
    sts.down_l = 0;       //�ڲ�״̬��ԭ
  }
  /*�������� -- ��һ���г�ʱ����*/
  if(sts.down_l == 1)
  {
    cmd.motor = 0;        //�ص����
		if(sts.residown_l == 6)//�������� -- ��ע�⣬������Ҫ�½�
		{
			sts.voice = 0;       
			cmd.voice = 9;     
		}
		else
		{
			sts.voice = 0;        //�������� -- ��ӭʹ�ã�������Ҫ�½�
			cmd.voice = 5;     
		}
    sts.down_l = 2;       //״̬�ı�
    Count = 5;            //�趨��ʱʱ��
    Time_en = 1;
  }
  if(sts.down_l == 2 && sts.voice == 2)
  {
    sts.down_l = 3;
    Time_en = 0;
  }
  if(sts.down_l == 3)
  {
    cmd.lock = 1;      //�򿪿���
    cmd.motor = 2;     //�½����
    Time_en = 0;       //��ʱ��־����
  }

  /*��ʱ����*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.down_l ++;   //״̬��һ
      Time_en = 0;     //��ʱ��־����
    }
  }
}
/*�½����账��*/
void ResisDown_Deal()
{
  static u8 Count = 0;    //��ʱ����
  static u8 Time_en = 0;  //��ʱ����ʹ��
  /*����*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 5)
  {
    sts.residown_l = 1;   //��ʼ����
  }
  /*�������� -- ��һ���г�ʱ����*/
  if(sts.residown_l == 1)
  {
    cmd.motor = 0;     //�ص����
    sts.voice = 0;     //�������� -- �����½�����
    cmd.voice = 4;     
    sts.residown_l = 2;//״̬�ı�
    Count = 5;         //�趨��ʱʱ��
    Time_en = 1;
  }
  /*�ȵ��������Ž���*/
  if(sts.residown_l == 2 && sts.voice == 2)
  {
    sts.residown_l = 3;//����״̬����
		sts.sense = 0;     //����״̬����
    Time_en = 0;       //��ʱ��־����
  }
  /*�ȴ���ʱʱ�䵽���½�����*/
  if(sts.residown_l == 3)
  {
    cmd.motor = 0;     //�ص����
    cmd.lock = 0;      //������
    Count = 20;         //�趨��ʱʱ��
    Time_en = 1;
		sts.residown_l = 4;//״̬����
  }
  /*�½�����  -- �޳�ʱ����*/
  if(sts.residown_l == 5)
  {
    Time_en = 0;         //��ʱ��־����
		sts.work_sts = 1 ;
		sts.run = 2;
		cmd.sts_copy = 1;
		sts.residown_l = 6;//״̬����
		
  }

  /*��ʱ����*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.residown_l ++;   //״̬��һ
      Time_en = 0;     //��ʱ��־����
    }
  }
}
/*�½�OK����*/
void DownOK_Deal()
{
  static u8 Count = 0;    //��ʱ����
  static u8 Time_en = 0;  //��ʱ����ʹ��
  /*����*/
  if(sts.work_old != sts.work_sts && sts.work_sts == 3)
  {
    sts.downok_l = 1;      //��ʼ����
  }
  /*��������*/
  if(sts.downok_l == 1)
  {
    cmd.lock = 0;        //������
    sts.voice = 0;       //�������� -- �����½��ɹ����벴��
    cmd.voice = 3; 
    Count = 5;           //�趨��ʱʱ��
    Time_en = 1;
    sts.downok_l = 2;    //״̬�ı�
  }
  /*�ȵ��������Ž���*/
  if(sts.downok_l == 2 && sts.voice == 2)
  {
    sts.downok_l = 3;    //����״̬����
    Time_en = 0;         //��ʱ��־����
  }
  /*��ʱ����*/
  if(Time_en && Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
      sts.downok_l ++;   //״̬��һ
      Time_en = 0;     //��ʱ��־����
    }
  }
}

/*������ǿ������ �����ж�*/
void GL_Forced()
{
  static u8 Count = 0;    //��ʱ����
	/*��������״̬������*/
	if(sts.work_sts == 2 && key != 1 && !Count)
	{
		Count = 5;
	}
	/*�����½�״̬��̧��*/
	if(sts.work_sts == 3 && key != 2 && !Count)
	{
		Count = 5;
	}
  /*��ʱ����*/
  if(Count && Sys_Tick_1s)
  {
    Count --;
    if(!Count)
    {
			//��������
			if(sts.work_sts == 2 && key != 1)
			{
				sts.work_sts = 1;
				sts.run = 1;
			}
			//�����½�
			else if(sts.work_sts == 3 && key != 2)
			{
				sts.work_sts = 1;
				sts.run = 2;
			}	
    }
  }
}

/*���۴�����*/
void Lock_Deal()
{
	static u8 count = 0;
	static u8 limit = LOCK_LIMIT;
	//--���ÿ���  2017-12-26��
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
			//sts.lock = 2;   //2017��-11-11  ����  ȥ���˵������������
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

/*�����������*/
void Cmd_Analy()
{
	if(sts.init != 2 && cmd.ctrl)
	{
	  cmd.ctrl = 0;
	  cmd.ctrl= 0;
	}
	/*��������*/
	if(cmd.ctrl == 1)
	{
    if((sts.work_sts == 1 && sts.run == 1) || sts.work_sts == 4)
    ;
    else
    {
      /*ͣ�����*/
      {
        PW1 = 0;
        PW2= 0;
      }
			/*�½������״̬����*/
			sts.residown_l = 0;
      sts.work_sts = 1;
      sts.run = 1;
			sts.work_old = 0xff; //��֤�϶���ִ�е�
      sts.work_old = 0;
    }
	}
	/*�½�����*/
	if(cmd.ctrl == 2)
	{
    if(sts.work_sts == 1 && sts.run == 2 || sts.work_sts == 5)
    ;
    else
    {
      /*ͣ�����*/
      {
        PW1 = 0;
        PW2= 0;
      }
			/*���������״̬����*/
			sts.resi_l = 0;
      sts.work_sts = 1;
      sts.run = 2;
			sts.work_old = 0xff; //��֤�϶���ִ�е�
      sts.work_old = 0;
    }
	}
	cmd.ctrl = 0;
}





/*��������������*/
void GL_main(void)
{
	//init_GL();
	voice_deal();         //��������
	GL_Forced();          //ǿ��̧���´�����
	Lock_Deal();          //��Ż�������
  Interface();          //�ӿں���
	Start();              //��ʼ��
	Cmd_Analy();          //�������
	Up_Deal();            //������������
	ResisUp_Deal();       //�����������账��
	UpOK_Deal();          //����OK����
	Down_Deal();          //�����½�����
	ResisDown_Deal();     //�½����账��
	DownOK_Deal();        //�½�OK����
	if(!cmd.sts_copy)     //copy����
		sts.work_old = sts.work_sts;
	else
		cmd.sts_copy = 0;
}

/*ȫ�ֳ�ʼ��*/
void init_main(void)
{
	sts.car_radar = 3;    //�״����ʼ��Ϊ��̬״̬
	CAR_MODLE = 3;        //����⳵λ    
}





