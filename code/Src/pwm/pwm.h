#ifndef __PWM_H__
#define __PWM_H__

#include <contiki.h>

#define CSB_IN1      GPIO_PIN_3
#define CSB_IN2      GPIO_PIN_4


#define CSB_PORT     GPIOB


int PWM_Init(void);
int PWM_ChannelAStart(uint32_t pwm_count);
int PWM_ChannelBStart(uint32_t pwm_count);
void PWM_CSBIN1_IRQHandler(void);
void PWM_CSBIN2_IRQHandler(void);

#endif
