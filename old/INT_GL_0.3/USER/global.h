#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "IO.h"
#include "string.h"
#include "disp.h"
#include "Interface.h"
#include "timer.h"

#define CAR_DISTANCE 80  //单位：厘米

extern unsigned char key;

extern unsigned char CAR_MODLE;

extern RDAR_STRUCT radar;

extern unsigned int SysTickCnt;
extern unsigned int SysWait_Times[10];
extern unsigned int Sys_Tick_Count;
extern unsigned int Sys_Tick_1s;
extern unsigned int Sys_Tick_500ms;

/*全局命令结构体*/
typedef struct
{
	u8 car_radar;       //雷达开机指令 1：开机
  u8 motor;           //电机控制指令1：上升 2:下降
	u8 voice;           //声音指令 1 - 7 ：七种声音 8：声音最大化
	u8 lock;            //卡扣指令   0：锁卡扣   1:打开卡扣
	u8 ctrl;            //地锁控制 1 -- 上升 2 -- 下降  
	u8 sts_copy;        //地锁状态复制控制 1 -- 不允许控制
}CMD_T;

/*全局状态结构体*/
typedef struct
{
  u8 init;           //初始化状态1：初始化中  2:初始化完成
	u8 car_radar;      //超声波判断有无车标志 1: 有车 0 无车 3:不做判断
	u8 voice;          //1 -- 收到指令  2 -- 指令执行完毕
	u8 work_sts;       //1 -- 运行  2 -- 升起  3 -- 下降  4 -- 上升遇阻  5 -- 下降遇阻
	u8 work_old;       //地锁系统状态历史值
	u8 run;            //运行的状态 1：上升      2: 下降
	u8 resi_l;         //遇阻内部状态
	u8 residown_l;     //下降遇阻内部状态
	u8 up_l;           //上升成功内部状态
	u8 upok_l;         //上升成功内部状态
	u8 down_l;         //上升成功内部状态
	u8 downok_l;       //上升成功内部状态
	u8 lock;           //1 -- 吸和   2 -- 断开
	u8 sense;          //1 -- 过流   0 -- 正常
	u32 stick_radar;   //雷达滴答值计时标志
}STS_T;
extern CMD_T cmd;
extern STS_T sts;
#endif



