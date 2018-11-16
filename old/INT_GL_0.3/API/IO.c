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
	//A0 A1 A5
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5;;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
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
  uint8_t key_check = 0;
  /*������� -- ȥ��*/	
	if(SENSE)
	{
		SysDelayHMSM(0,0,0,500);
		if(SENSE)
		{
			SysDelayHMSM(0,0,0,500);
			if(SENSE)
				sts.sense = 1;
		}
	}
	else
		sts.sense = 0;
  /*��λ���ؼ��*/
	if(mode && (!SW_UP || !SW_DOWN))
	{
		//SysDelayHMSM(0,0,0,10);
		if(!SW_UP)
			key_check = 1;
		else if(!SW_DOWN)
			key_check = 2;
	}	    
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
	//��ʼ��PA6 a7 a4
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_4;				     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					     
	GPIO_SetBits(GPIOA,GPIO_Pin_6 | GPIO_Pin_7);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	
	//��ʼ��PC13 C14 C15
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;				     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					     
	GPIO_SetBits(GPIOC,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
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
u8 led_cmd = 0;  //0 - Ϩ�� 1-��ɫ  2-��ɫ  3-��ɫ
/*LEDָʾ*/
void LED_play()
{
	switch(led_cmd)
	{
		case 0:
			LED_R = 1;
			LED_G = 1;
			LED_B = 1;
			break;
		case 1:
			LED_R = 0;
			LED_G = 1;
			LED_B = 1;
			break;
		case 2:
			LED_R = 1;
			LED_G = 0;
			LED_B = 1;
			break;
		case 3:
			LED_R = 1;
			LED_G = 1;
			LED_B = 0;
			break;
	}
}



