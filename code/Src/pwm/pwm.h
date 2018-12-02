#ifndef __PWM_H__
#define __PWM_H__

#define CSB_IN1      GPIO_PIN_3
#define CSB_IN2      GPIO_PIN_4


#define CSB_PORT     GPIOB


int PWM_Init(void);
int PWM_ChannelAStart(void);
int PWM_ChannelBStart(void);
void PWM_CSBIN1_IRQHandler(void);
void PWM_CSBIN2_IRQHandler(void);

#endif
