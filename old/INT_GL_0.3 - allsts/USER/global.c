
#include"global.h"

/*全局命令结构体*/
CMD_T cmd;
STS_T sts;


unsigned char key = 0;	          //地锁上升下降信号 1:升起  2: 下降

//定义车位检测模式
unsigned char CAR_MODLE = 0;      //1 超声波检测    2 地磁检测   3 不检测车位

/*滴答定时器专区*/
unsigned int SysTickCnt = 0;      //滴答值
unsigned int SysWait_Times[10];   //等待值
// --map
// 0 -- chu
unsigned int Sys_Tick_Count = 0;  //滴答定时器内部计数 -- 1s专用
unsigned int Sys_Tick_1s = 0;     //滴答定时器产生的1S中断上





