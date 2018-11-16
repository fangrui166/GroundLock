#ifndef _DISP_H_
#define _DISP_H_



#include "global.h"
void Motor_Driver(void);
void SysDelayHMSM (uint8_t hours,uint8_t minutes,uint8_t seconds,uint16_t ms);
uint8_t SysWaitHMSM (uint8_t timescnt,uint8_t hours,uint8_t minutes,uint8_t seconds,uint16_t ms);
void init_main(void);
void GL_main(void);
void Time1s(void);


#endif



