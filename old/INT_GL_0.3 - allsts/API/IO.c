#include "IO.h"
#include "global.h"
#include "delay.h"
RDAR_STRUCT radar;
/*************************************************************
  Function:    
  Description: ���� �����½��źŶ�ȡ���ų�ʼ��
  Calls:       
  Called By:   
  Input:       
  Output:      
  Return:      
  Others:      ��ʼ�� PA5��PA6
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
  Description: ���� �����½��źŶ�ȡ���Ŷ�ȡ
  Calls:       
  Called By:   
  Input:       
  Output:      
  Return:      
  Others:      ��ʼ�� PA5��PA6  һ��ֻ�ܶ�ȡһ������
*************************************************************/
u8 KEY_Scan(u8 mode)
{	 
	static uint8_t key_check = 0;
  /*������� -- ȥ��*/	
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
  /*�شų�λ���*/
  //���޳����г�
  if(NC_ERR && CAR_MODLE == 2) //�ߵ�ƽ�������쳣
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
    //���г����޳�
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
	//�ӵ�ż��ת�������������
  else if(!NC_ERR && CAR_MODLE == 2)
  {
    CAR_MODLE = 1;
  }
	
	
	//�ӳ��������ת������ż��
	if(NC_ERR && CAR_MODLE == 1)
	{
		CAR_MODLE = 2;
	}
	//�شŹ���
	if(!NC_ERR)
		sts.err.geo_1b = 1;
	else if(NC_ERR)
		sts.err.geo_1b = 0;
	
	//��λ���ؼ��
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
	
	//��λ������ж�
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
  Description: ������ų�ʼ��
  Calls:       
  Called By:   
  Input:       
  Output:      
  Return:      
  Others:      ��ʼ�� PA1
*************************************************************/
void OutIo_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//��ʼ��PA1 -- error led PA4 -- 485
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4;				     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					     
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
	
	//��ʼ��PB15 -- �����  PB1 -- ���� PB7 -- �״� 
	//------PB3  -- TRIG1   PB5 -- TRIG2
	//------PB12 -- PW2     PB13 -- PW1
	//------PB8
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_1 | GPIO_Pin_3  | GPIO_Pin_7 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_8;				      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(GPIOB, &GPIO_InitStructure);					     			          
	GPIO_ResetBits(GPIOB,GPIO_Pin_15 | GPIO_Pin_7 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_8 | GPIO_Pin_3);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
}



/*************************************************************
  Function:    IO_Init
  Description: ����������ų�ʼ��
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
  Description: ���
  Calls:       
  Called By:   
  Input:       
  Output:      
  Return:      
  Others:      
*************************************************************/
void Radar_Deal(void)
{
  static u8 cmd_old = 0;       //��ʷֵ�洢
	static u8 count_car = 0;     //���޳�����
	static u8 car_store;         //��ʷֵ�洢
	static u8 car_modle_old;     //��λ�����ʷֵ
	u32 Temp_Destance = 0;
	/*�����*/
	if(radar.cmd == 1 && ((SysTickCnt - sts.stick_radar) >= 100))
	{
	  radar.cmd = 0;
		Lock_ON(TRIG_1);
    sts.stick_radar = SysTickCnt;     
		memset(radar.sts,1,2);         //�״�״̬��λ
		memset(radar.Time,5,2);        //�״ﳬʱ��λ
	  //TIM4_Cap_Init(0XFFFF,72-1);	 //��1Mhz��Ƶ�ʼ��� 
	  TIM3_Cap_Init(0XFFFF,72-1);	   //��1Mhz��Ƶ�ʼ��� 
		TIM_Cmd(TIM3,ENABLE ); 	       //ʹ�ܶ�ʱ��3
		//TIM_Cmd(TIM4,ENABLE ); 	  //ʹ�ܶ�ʱ��4
		SysDelayHMSM(0,0,0,5);
    Lock_OFF(TRIG_1);
    //Lock_OFF(TRIG_2);
	}
	/*ͨ��1�״�����*/
  if(radar.ECHO_Sts[0] & 0X80)
	{
	  TIM_Cmd(TIM3,DISABLE);
		Temp_Destance = ((radar.ECHO_Sts[0] & 0X3F) * 65536 ) + radar.ECHO_Val[0];
		//̽��������600cm ���� С��20cm
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
	/*ͨ�����״�����*/
  /*if(radar.ECHO_Sts[1] & 0X80)
	{
		Temp_Destance = ((radar.ECHO_Sts[1] & 0X3F) * 65536 ) + radar.ECHO_Val[1];
		//̽��������600cm ���� С��20cm
		if((Temp_Destance >= 35000) || (Temp_Destance < 1100))
		{
			radar.Destance[1] = 0;
		}
		else
			radar.Destance[1] = Temp_Destance * 340 / 10000 / 2;
		radar.ECHO_Sts[1] = 0;
		radar.ECHO_Val[1] = 0;
		radar.sts[1] = 2;
	  TIM_Cmd(TIM4,DISABLE ); 	//ʹ�ܶ�ʱ��3
	}*/
	/*���޳��ӿ����*/
	if(radar.sts[0] == 2)// && radar.sts[1] == 2)
	{
	  radar.sts[0] = 0;
		//memset(radar.sts,0,2);
		if(!radar.Destance[0])// && !radar.Destance[1])
			car_store |= (1 << count_car);     //��ʷֵ�洢
		/*else if(!radar.Destance[1])
    {
			if(radar.Destance[0] <= CAR_DISTANCE && radar.Destance[0])
				car_store |= (1 << count_car);   //��ʷֵ�洢
				
			else if(radar.Destance[0] > CAR_DISTANCE && radar.Destance[0])
				car_store &= (0 << count_car);   //��ʷֵ�洢
		}
		else if(!radar.Destance[0])
    {
			if(radar.Destance[1] <= CAR_DISTANCE && radar.Destance[1])
			  car_store |= (1 << count_car);   //��ʷֵ�洢
			else if(radar.Destance[1] > CAR_DISTANCE && radar.Destance[1])
				car_store &= (0 << count_car);   //��ʷֵ�洢
		}*/
		else
		{
			if(radar.Destance[0] <= CAR_DISTANCE)// || radar.Destance[1] <= CAR_DISTANCE)
				car_store |= (1 << count_car);   //��ʷֵ�洢
			else
				car_store &= (0 << count_car);   //��ʷֵ�洢
		}
		/*������*/
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
	/*������������� -- �龰���شŻ��������������������⳵λ*/
	if((car_modle_old != CAR_MODLE) && (CAR_MODLE == 1))
	{
	  radar.cmd = 1;
	}
	car_modle_old = CAR_MODLE;

	/*���޳���ⴥ�� -- ���5Sִ��һ��
	if(Sys_Tick_1s && cmd.car_radar && CAR_MODLE == 1)
	{
		check_count ++;
		if(check_count >= 3)
		{
			check_count = 0;
			radar.cmd = 1;
		}
	}*/
	
//	/*������ⴥ��*/
//	if(cmd_old != cmd.car_radar && cmd.car_radar && CAR_MODLE == 1)
//	{
//		radar.cmd = 1;
//		count_car = 0; //�洢��������
//    
//	}
//	if(!cmd.car_radar)
//	{
//		radar.cmd = 0;
//	}
//	cmd_old = cmd.car_radar;
	
	//2017 - 11 - 12  ��   ��ʹ���״���
	//2018 - 01 - 15  ��   ���ӽ�ֹ�жϳ�λ��־
	if(CAR_MODLE == 3)
	  sts.car_radar = 0;
}
/*************************************************************
  Function:    IO_Init
  Description: ����������ų�ʼ��
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
  Description: �����������
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





