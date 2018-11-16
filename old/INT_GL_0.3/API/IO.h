#ifndef __IO_H
#define __IO_H	 
#include "sys.h"

//output �������
#define PW1    PAout(6)
#define PW2    PAout(7)
#define CTR485 PAout(4)
#define LED_R  PCout(13)
#define LED_G  PCout(14)
#define LED_B  PCout(15)

//input ��������
#define SW_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ��������λ����
#define SW_DOWN GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)//��ȡ�½�����λ����
#define SENSE   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//��ȡ�½�����λ����


typedef struct
{
  u8 cmd;            //������̽��
  u8 Time[2];        //��ʱ�ж�
	u8 sts[2];         //״̬ 1����ѯ�� 2��ѯ���
  u8 ECHO_Sts[2];    //������״̬
  u16 ECHO_Val[2];   //������״̬
  u16 Destance[2];   //������
}RDAR_STRUCT;
extern u8 led_cmd;  //0 - Ϩ�� 1-��ɫ  2-��ɫ  3-��ɫ
void LED_ON(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void LED_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Lock_ON(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Lock_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void IO_Init(void);
void IO_Main(void);		
void LED_play(void);

#endif

