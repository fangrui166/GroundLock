#include "global.h"
#include "usart.h"

#define LOCK_LIMIT 5   //���������



/*�������*/
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
/*1S��ʱ*/
void Time1s(void)
{
  if(Sys_Tick_Count > 1000)
	{
		Sys_Tick_Count = 0;
		Sys_Tick_1s = 1;
	}
  
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
		}
    //�㲥����ָ��Ӧ��
		if(uart[2].buf[0] == 0xAA && uart[2].buf[1] == 0x02 && 
		   uart[2].buf[2] == 0x41 && uart[2].buf[3] == 0xEF)  //AA 02 41 EF
		{
      sts.voice = 1;
		}
    //�����������Ӧ��
		if(uart[2].buf[0] == 0xAA && uart[2].buf[1] == 0x02 && 
		   uart[2].buf[2] == 0x80 && uart[2].buf[3] == 0xEF)  //AA 02 80 EF 
		{
      sts.voice = 2;
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
      Time_en = 0;     //��ʱ��־����
    }
  }
}

/*������������*/
void Up_Deal()
{
  /*����*/
  if((sts.work_old != sts.work_sts) && (sts.work_sts == 1) && (sts.run == 1))
	  sts.up_l = 1;         //��ʼ����

  /*��������ж�*/
  if(key == 1 && sts.up_l)
  {
    cmd.motor = 0;        //�ص��
		
    sts.work_sts = 2;     //ϵͳ״̬�л�Ϊ����
    sts.up_l = 0;         //�ڲ�״̬��ԭ
  }
  /*���������ж�*/
  if(sts.sense && sts.up_l)
  {
    cmd.motor = 0;        //�ص��
		/*���ȴ�ͳһ������*/
		{
			PW1 = 1;
			PW2 = 1;
		}
    sts.work_sts = 4;     //ϵͳ״̬�л�Ϊ��������
		sts.work_old = 0xff;  //��֤���账��״̬���н�ȥ
    sts.up_l = 0;         //�ڲ�״̬��ԭ
  }
	/*�޳����������*/
	if(sts.up_l == 1)
	{
    cmd.car_radar = 0; //�״�ػ�
    cmd.motor = 1;     //�������
	}
}
/*�������账��*/
void ResisUp_Deal()
{
  static u8 Count = 0;    //��ʱ����
  static u8 Time_en = 0;  //��ʱ����ʹ��
	static uint32_t sys_stick = 0; 
  /*����*/
  if((sts.work_old != sts.work_sts) && (sts.work_sts == 4))
    sts.resi_l = 1;    //��ʼ����
  /*�½����� -- ���ﲻ�ÿ��ǵ�����û����ԭλû��*/
  if(sts.resi_l == 1)
  {
    cmd.motor = 2;     //�½����
		sys_stick = SysTickCnt;
		sts.resi_l = 2;
  }
  if((sts.resi_l == 2) && (key == 2 || sts.sense) && ((SysTickCnt - sys_stick) > 500))
  {
    sts.resi_l = 4;    //״̬����
  }
  /*�趨��ʱʱ��*/
  if(sts.resi_l == 4)
  {
    cmd.motor = 0;     //�ص����
    Count = 10;        //�趨��ʱʱ��
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
/*�����½�����*/
void Down_Deal()
{
  /*����*/
  if((sts.work_old != sts.work_sts) && (sts.work_sts == 1) && (sts.run == 2))
    sts.down_l = 1;         //��ʼ����

  /*�½������ж�*/
  if(key == 2 && sts.down_l)
  {
    cmd.motor = 0;        //�ص��
    sts.work_sts = 3;     //ϵͳ״̬�л�Ϊ�½�
    sts.down_l = 0;       //�ڲ�״̬��ԭ
  }
  /*�½������ж�*/
  if(sts.sense && sts.down_l)
  {
    cmd.motor = 0;        //�ص��
		/*���ȴ�ѭ���ص����*/
		{
			PW1 = 1;
			PW2 = 1;
		}
    sts.work_sts = 5;     //ϵͳ״̬�л�Ϊ�½�����
	  sts.work_old = 0xff;  //��֤���账��״̬�ܽ�ȥ
    sts.down_l = 0;       //�ڲ�״̬��ԭ
  }

  if(sts.down_l == 1)
    cmd.motor = 2;     //�½����

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

  /*�ȴ���ʱʱ�䵽���½�����*/
  if(sts.residown_l == 1)
  {
    cmd.motor = 0;     //�ص����
    Count = 10;         //�趨��ʱʱ��
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



/*�����������*/
void Cmd_Analy()
{
	/*��������*/
	if(cmd.ctrl == 1)
	{
    if((sts.work_sts == 1 && sts.run == 1) || sts.work_sts == 4)
    ;
    else
    {
      /*ͣ�����*/
      {
        PW1 = 1;
        PW2= 1;
      }
			/*�½������״̬����*/
			sts.residown_l = 0;
      sts.work_sts = 1;
      sts.run = 1;
			sts.work_old = 0xff; //��֤�϶���ִ�е�
    }
	}
	/*�½�����*/
	if(cmd.ctrl == 2)
	{
    if((sts.work_sts == 1 && sts.run == 2) || sts.work_sts == 5)
    ;
    else
    {
      /*ͣ�����*/
      {
        PW1 = 1;
        PW2= 1;
      }
			/*���������״̬����*/
			sts.resi_l = 0;
      sts.work_sts = 1;
      sts.run = 2;
			sts.work_old = 0xff; //��֤�϶���ִ�е�
    }
	}
	cmd.ctrl = 0;
}





/*��������������*/
void GL_main(void)
{  
	Interface();          //�ӿں���
	Cmd_Analy();          //�������
	GL_Forced();          //ǿ��̧���´�����
	Up_Deal();            //������������
	ResisUp_Deal();       //�����������账��
	Down_Deal();          //�����½�����
	ResisDown_Deal();     //�½����账��
	if(!cmd.sts_copy)     //copy����
		sts.work_old = sts.work_sts;
	else
		cmd.sts_copy = 0;
}

/*ȫ�ֳ�ʼ��*/
void init_main(void)
{
	sts.work_sts = 1;     //ģʽ�л�Ϊ���� -- ����
	sts.run = 1;   
}





