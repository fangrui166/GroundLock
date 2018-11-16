#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"



void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_Cap_Init(u16 arr,u16 psc);
void TIM4_Cap_Init(u16 arr,u16 psc);

unsigned int ECO1_API(void);
unsigned int ECO2_API(void);
 
#endif



