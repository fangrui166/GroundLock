#ifndef __IO_H
#define __IO_H	 
#include "sys.h"

//output �������
#define ERROR  PAout(1) //GPIOA,GPIO_Pin_1
#define LOCK   GPIOB,GPIO_Pin_15
#define LMPD   GPIOB,GPIO_Pin_1
#define UPWR   GPIOB,GPIO_Pin_7
#define TRIG_1 GPIOB,GPIO_Pin_3
//#define TRIG_2 GPIOB,GPIO_Pin_5
#define PW1    PBout(13)
#define PW2    PBout(12)
#define PWM_EN PBout(8)      //PWMʹ��
#define CTR485 PAout(4)

//input ��������
#define SW_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)//��ȡ��������λ����
#define SW_DOWN GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//��ȡ�½�����λ����
#define ECHO_1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//1#�״�
#define NC_STS  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)//�ش��ź�
#define NC_ERR  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//�ش�����״��
#define SENSE   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//����
#define GEO     GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)//�ش�

//���ܷ�װ
#define VOICE_ENABLE LED_OFF(LMPD);          //����ʹ��
#define VOICE_DISABLE LED_ON(LMPD);          //������ֹ
#define RADAR_ENABLE Lock_ON(UPWR);          //���ʹ��
#define RADAR_DISABLE LED_OFF(UPWR);         //����ֹ



typedef struct
{
  u8 cmd;            //������̽��
  u8 Time[2];        //��ʱ�ж�
	u8 sts[2];         //״̬ 1����ѯ�� 2��ѯ���
  u8 ECHO_Sts[2];    //������״̬
  u16 ECHO_Val[2];   //������״̬
  u16 Destance[2];   //������
}RDAR_STRUCT;

void LED_ON(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void LED_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Lock_ON(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Lock_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void IO_Init(void);
void IO_Main(void);		


#endif

