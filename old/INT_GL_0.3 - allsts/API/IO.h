#ifndef __IO_H
#define __IO_H	 
#include "sys.h"

//output 输出引脚
#define ERROR  PAout(1) //GPIOA,GPIO_Pin_1
#define LOCK   GPIOB,GPIO_Pin_15
#define LMPD   GPIOB,GPIO_Pin_1
#define UPWR   GPIOB,GPIO_Pin_7
#define TRIG_1 GPIOB,GPIO_Pin_3
//#define TRIG_2 GPIOB,GPIO_Pin_5
#define PW1    PBout(13)
#define PW2    PBout(12)
#define PWM_EN PBout(8)      //PWM使能
#define CTR485 PAout(4)

//input 输入引脚
#define SW_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)//读取上升的限位开关
#define SW_DOWN GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//读取下降的限位开关
#define ECHO_1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//1#雷达
#define NC_STS  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)//地磁信号
#define NC_ERR  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//地磁运行状况
#define SENSE   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//过流
#define GEO     GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)//地磁

//功能封装
#define VOICE_ENABLE LED_OFF(LMPD);          //声音使能
#define VOICE_DISABLE LED_ON(LMPD);          //声音禁止
#define RADAR_ENABLE Lock_ON(UPWR);          //测距使能
#define RADAR_DISABLE LED_OFF(UPWR);         //测距禁止



typedef struct
{
  u8 cmd;            //超声波探测
  u8 Time[2];        //超时判断
	u8 sts[2];         //状态 1：查询中 2查询完成
  u8 ECHO_Sts[2];    //超声波状态
  u16 ECHO_Val[2];   //超声波状态
  u16 Destance[2];   //计算结果
}RDAR_STRUCT;

void LED_ON(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void LED_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Lock_ON(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Lock_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void IO_Init(void);
void IO_Main(void);		


#endif

