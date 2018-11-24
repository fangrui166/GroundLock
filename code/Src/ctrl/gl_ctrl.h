#ifndef __GL_CTRL_H__
#define __GL_CTRL_H__

#define CTRL1_GPIO  GPIO_PIN_10
#define CTRL2_GPIO  GPIO_PIN_2
#define CTRL3_GPIO  GPIO_PIN_1
#define CTRL4_GPIO  GPIO_PIN_0

#define SWUP_GPIO    GPIO_PIN_14
#define SWDOWN_GPIO  GPIO_PIN_15


#define CTRL_PORT   GPIOB
#define SW_PORT     GPIOB

typedef enum{
    CTRL_STATUS_UP,             // stoped and 90 degree
    CTRL_STATUS_DOWN,           // stoped and 0 degree
    CTRL_STATUS_MOVING,         // moving
    CTRL_STATUS_UNKOWN,
}Ctrl_status_t;

typedef enum{
    MOTOR_MOVUP,
    MOTOR_MOVDOWN,
    MOTOR_STOP,
}Motor_state_t;
Motor_state_t Ctrl_GetMotorState(void);
int Ctrl_MotorOff(void);
int Ctrl_MotorUp(void);
int Ctrl_MotorDown(void);
Ctrl_status_t Ctrl_GetStatus(void);
int Ctrl_Init(void);

#endif
