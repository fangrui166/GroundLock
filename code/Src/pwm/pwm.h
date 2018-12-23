#ifndef __PWM_H__
#define __PWM_H__

#include <contiki.h>

#define PWM_TRIGGER1    GPIO_PIN_8
#define PWM_TRIGGER2    GPIO_PIN_11

#define CSB_IN1      GPIO_PIN_3
#define CSB_IN2      GPIO_PIN_4

#define CSB_PORT     GPIOB
#define PWM_PORT     GPIOA

#define PWM_MAX_TIMEOUT             1000  //ms
#define PWM_TIMEOUT_DISTANCE        700  //cm

#define  DWT_CR      *(volatile u32 *)0xE0001000
#define  DWT_CYCCNT  *(volatile u32 *)0xE0001004
#define  DEM_CR      *(volatile u32 *)0xE000EDFC
#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)

typedef enum{
    PWM_Channel_A,
    PWM_Channel_B,
}PWM_Channel;

#define PWM_ChannelA_msk        (1 << PWM_Channel_A)
#define PWM_ChannelB_msk        (1 << PWM_Channel_B)

int PWM_Init(void);
int PWM_ChannelAStart(uint32_t pwm_count);
int PWM_ChannelBStart(uint32_t pwm_count);
void PWM_CSBIN1_IRQHandler(void);
void PWM_CSBIN2_IRQHandler(void);

#endif
