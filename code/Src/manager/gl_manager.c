#include "gl_manager.h"
#include <sys/ctimer.h>
#include "gl_ctrl.h"
#include <string.h>
#include "adc.h"
#include <stdio.h>
#include "spk.h"


static struct ctimer current_detc_timer;
static struct ctimer moving_timeout_timer;

uint8_t gl_lock_state;
uint8_t current_detc_timer_stop = 1;
uint8_t moving_timeout_timer_stop = 1;

uint8_t action_err_stage = ACTION_ERR_UNKOWN;
uint8_t Gl_GetLockState(void)
{
    return gl_lock_state;
}

void Gl_SetLockState(uint8_t state)
{
    gl_lock_state = state;
}
uint8_t Gl_GetActionErrStage(void)
{
    return action_err_stage;
}
static void Gl_CurrentDetcCallback(void *ptr)
{
    Motor_state_t motor_state = Ctrl_GetMotorState();
    uint32_t current = ADC_GetCurrentAverage();
    if((current >= UP_MEET_RESISTANCE_CURRENT_MA) && (motor_state == MOTOR_MOVUP)){
        printf("up over current:%d\n", current);
        action_err_stage = ACTION_ERR_UP_ERR;
        Gl_CtrlLock(UNLOCK);
    }
    else if((current >= UP_BIG_DAMP_CURRENT_MA) && (motor_state == MOTOR_MOVUP)){
        printf("up bit damp current:%d\n", current);
    }
    else if((current >= DOWN_MEET_RESISTANCE_CURRENT_MA) && (motor_state == MOTOR_MOVDOWN)){
        printf("down over current:%d\n", current);
        Gl_CtrlLock(STOP);
        Gl_SetLockState(LOCK_STATE_FIALDOWN);
    }
    else if((current >= DOWN_BIG_DAMP_CURRENT_MA) && (motor_state == MOTOR_MOVDOWN)){
        printf("down bit damp current:%d\n", current);
    }

    if(!current_detc_timer_stop){
        ctimer_reset(&current_detc_timer);
    }
}
static void Gl_MovTimeoutCallback(void *ptr)
{
    if(!moving_timeout_timer_stop){
        SPK_Start(MOVTIMEOUT_BUZZER_TIME);
    }

}
static void Gl_CurrentDetcStart(void)
{
    if(current_detc_timer_stop){
        current_detc_timer_stop = 0;
        ctimer_set(&current_detc_timer, 200, Gl_CurrentDetcCallback, NULL);
    }
}
static void Gl_CurrentDetcStop(void)
{
    if(current_detc_timer_stop == 0){
        current_detc_timer_stop = 1;
        ctimer_stop(&current_detc_timer);
    }

}
static void Gl_MovTimeoutStart(void)
{
    if(moving_timeout_timer_stop){
        moving_timeout_timer_stop = 0;
        ctimer_set(&moving_timeout_timer, CLOCK_CONF_SECOND*5, Gl_MovTimeoutCallback, NULL);
    }
}
static void Gl_MovTimeoutStop(void)
{
    if(moving_timeout_timer_stop == 0){
        moving_timeout_timer_stop = 1;
        ctimer_stop(&moving_timeout_timer);
    }

}
int Gl_CtrlLock(uint8_t action)
{
    if(action == LOCK){
        Gl_CurrentDetcStart();
        Gl_MovTimeoutStart();
        Ctrl_MotorUp();
    }
    else if(action == UNLOCK){
        Gl_CurrentDetcStart();
        Gl_MovTimeoutStart();
        Ctrl_MotorDown();
    }
    else if(action == STOP){
        Gl_CurrentDetcStop();
        Gl_MovTimeoutStop();
        Ctrl_MotorOff();
    }
    return 0;
}

Lock_location Gl_GetLocation(void)
{
    Lock_location location;
    Ctrl_status_t status = Ctrl_GetStatus();
    if(status == CTRL_STATUS_UP){
        location = LOCK_TOP;
    }
    else if(status == CTRL_STATUS_DOWN){
        location = LOCK_BOTTOM;
    }
    else{
        location = LOCK_MIDDLE;
    }
    return location;
}

int Gl_LockOn(void)
{
    Lock_location location = Gl_GetLocation();
    printf("%s location:%d\n", __func__, location);
    switch(location){
        case LOCK_TOP:
            printf("do nothing\n");
            break;
        case LOCK_MIDDLE:
        case LOCK_BOTTOM:
            Gl_CtrlLock(LOCK);
            break;
    }
    return 0;
}

int Gl_LockOff(void)
{
    Lock_location location = Gl_GetLocation();
    printf("%s location:%d\n", __func__, location);
    switch(location){
        case LOCK_TOP:
        case LOCK_MIDDLE:
            Gl_CtrlLock(UNLOCK);
            break;
        case LOCK_BOTTOM:
            printf("do nothing\n");
            break;
    }
    return 0;
}
int Gl_ManagerInit(void)
{
    return 0;
}
