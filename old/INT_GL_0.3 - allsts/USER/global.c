
#include"global.h"

/*ȫ������ṹ��*/
CMD_T cmd;
STS_T sts;


unsigned char key = 0;	          //���������½��ź� 1:����  2: �½�

//���峵λ���ģʽ
unsigned char CAR_MODLE = 0;      //1 ���������    2 �شż��   3 ����⳵λ

/*�δ�ʱ��ר��*/
unsigned int SysTickCnt = 0;      //�δ�ֵ
unsigned int SysWait_Times[10];   //�ȴ�ֵ
// --map
// 0 -- chu
unsigned int Sys_Tick_Count = 0;  //�δ�ʱ���ڲ����� -- 1sר��
unsigned int Sys_Tick_1s = 0;     //�δ�ʱ��������1S�ж���





