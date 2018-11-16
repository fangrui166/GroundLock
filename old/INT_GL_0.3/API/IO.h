#ifndef __IO_H
#define __IO_H	 
#include "sys.h"

//output 输出引脚
#define PW1    PAout(6)
#define PW2    PAout(7)
#define CTR485 PAout(4)
#define LED_R  PCout(13)
#define LED_G  PCout(14)
#define LED_B  PCout(15)

//input 输入引脚
#define SW_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取上升的限位开关
#define SW_DOWN GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)//读取下降的限位开关
#define SENSE   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//读取下降的限位开关


typedef struct
{
  u8 cmd;            //超声波探测
  u8 Time[2];        //超时判断
	u8 sts[2];         //状态 1：查询中 2查询完成
  u8 ECHO_Sts[2];    //超声波状态
  u16 ECHO_Val[2];   //超声波状态
  u16 Destance[2];   //计算结果
}RDAR_STRUCT;
extern u8 led_cmd;  //0 - 熄灭 1-红色  2-绿色  3-蓝色
void LED_ON(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void LED_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Lock_ON(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Lock_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void IO_Init(void);
void IO_Main(void);		
void LED_play(void);

#endif

