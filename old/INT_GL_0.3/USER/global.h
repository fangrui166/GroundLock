#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "IO.h"
#include "string.h"
#include "disp.h"
#include "Interface.h"
#include "timer.h"

#define CAR_DISTANCE 80  //��λ������

extern unsigned char key;

extern unsigned char CAR_MODLE;

extern RDAR_STRUCT radar;

extern unsigned int SysTickCnt;
extern unsigned int SysWait_Times[10];
extern unsigned int Sys_Tick_Count;
extern unsigned int Sys_Tick_1s;
extern unsigned int Sys_Tick_500ms;

/*ȫ������ṹ��*/
typedef struct
{
	u8 car_radar;       //�״￪��ָ�� 1������
  u8 motor;           //�������ָ��1������ 2:�½�
	u8 voice;           //����ָ�� 1 - 7 ���������� 8���������
	u8 lock;            //����ָ��   0��������   1:�򿪿���
	u8 ctrl;            //�������� 1 -- ���� 2 -- �½�  
	u8 sts_copy;        //����״̬���ƿ��� 1 -- ���������
}CMD_T;

/*ȫ��״̬�ṹ��*/
typedef struct
{
  u8 init;           //��ʼ��״̬1����ʼ����  2:��ʼ�����
	u8 car_radar;      //�������ж����޳���־ 1: �г� 0 �޳� 3:�����ж�
	u8 voice;          //1 -- �յ�ָ��  2 -- ָ��ִ�����
	u8 work_sts;       //1 -- ����  2 -- ����  3 -- �½�  4 -- ��������  5 -- �½�����
	u8 work_old;       //����ϵͳ״̬��ʷֵ
	u8 run;            //���е�״̬ 1������      2: �½�
	u8 resi_l;         //�����ڲ�״̬
	u8 residown_l;     //�½������ڲ�״̬
	u8 up_l;           //�����ɹ��ڲ�״̬
	u8 upok_l;         //�����ɹ��ڲ�״̬
	u8 down_l;         //�����ɹ��ڲ�״̬
	u8 downok_l;       //�����ɹ��ڲ�״̬
	u8 lock;           //1 -- ����   2 -- �Ͽ�
	u8 sense;          //1 -- ����   0 -- ����
	u32 stick_radar;   //�״�δ�ֵ��ʱ��־
}STS_T;
extern CMD_T cmd;
extern STS_T sts;
#endif



