#include "gl_manager.h"
#include <sys/ctimer.h>
#include "gl_ctrl.h"
#include <string.h>
#include "adc.h"
#include <stdio.h>
#include "spk.h"
#include "hlog.h"

static struct ctimer current_detc_timer;
static struct ctimer moving_timeout_timer;

uint8_t gl_lock_state;
uint8_t current_detc_timer_stop = 1;
uint8_t moving_timeout_timer_stop = 1;

static uint16_t up_meet_resistance_current;
static uint16_t up_big_damp_current ;
static uint16_t down_meet_resistance_current;
static uint16_t down_big_damp_current;

uint8_t action_err_stage = ACTION_ERR_UNKOWN;
const char *location_str[] = {
    [LOCK_TOP]    = "top",
    [LOCK_MIDDLE] = "middle",
    [LOCK_BOTTOM] = "bottom",
};

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
    if((current >= up_meet_resistance_current) && (motor_state == MOTOR_MOVUP)){
        logi("up over current:%d\n", current);
        action_err_stage = ACTION_ERR_UP_ERR;
        Gl_CtrlLock(UNLOCK);
    }
    else if((current >= up_big_damp_current) && (motor_state == MOTOR_MOVUP)){
        logi("up bit damp current:%d\n", current);
    }
    else if((current >= down_meet_resistance_current) && (motor_state == MOTOR_MOVDOWN)){
        logi("down over current:%d\n", current);
        Gl_CtrlLock(STOP);
        Gl_SetLockState(LOCK_STATE_FIALDOWN);
    }
    else if((current >= down_big_damp_current) && (motor_state == MOTOR_MOVDOWN)){
        logi("down bit damp current:%d\n", current);
    }

    if(!current_detc_timer_stop){
        ctimer_restart(&current_detc_timer);
    }
}
static void Gl_MovTimeoutCallback(void *ptr)
{
    if(!moving_timeout_timer_stop){
        logw("lock moving timeout\n");
        SPK_Start(MOVTIMEOUT_BUZZER_TIME);
        Gl_CtrlLock(STOP);
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
        ADC_StartDMA();
        Gl_CurrentDetcStart();
        Gl_MovTimeoutStart();
        Ctrl_MotorUp();
    }
    else if(action == UNLOCK){
        ADC_StartDMA();
        Gl_CurrentDetcStart();
        Gl_MovTimeoutStart();
        Ctrl_MotorDown();
    }
    else if(action == STOP){
        ADC_StopDMA();
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
    logi("%s :%s\n", __func__, location_str[location]);
    return location;
}

int Gl_LockOn(void)
{
    Lock_location location = Gl_GetLocation();
    switch(location){
        case LOCK_TOP:
            logi("do nothing\n");
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
    switch(location){
        case LOCK_TOP:
        case LOCK_MIDDLE:
            Gl_CtrlLock(UNLOCK);
            break;
        case LOCK_BOTTOM:
            logi("do nothing\n");
            break;
    }
    return 0;
}
int Gl_UpdateLimitedCurrent(uint16_t value, current_limited_t type)
{
    switch(type){
        case UP_RESISTANCE:
            up_meet_resistance_current = value;
            break;
        case UP_GIG_DAMP:
            up_big_damp_current = value;
            break;
        case DOWN_RESISTANCE:
            down_meet_resistance_current = value;
            break;
        case DOWN_GIG_DAMP:
            down_big_damp_current = value;
            break;
    }
    return setLimitedCurrent(value, type);

}
int Gl_GetLimitedCurrent(uint16_t *value, current_limited_t type)
{
    switch(type){
        case UP_RESISTANCE:
             *value = up_meet_resistance_current;
            break;
        case UP_GIG_DAMP:
            *value = up_big_damp_current;
            break;
        case DOWN_RESISTANCE:
            *value = down_meet_resistance_current;
            break;
        case DOWN_GIG_DAMP:
            *value = down_big_damp_current;
            break;
    }
    return 0;
}
int Gl_ManagerInit(void)
{
    int ret;
    ret = getLimitedCurrent(&up_meet_resistance_current, UP_RESISTANCE);
    if((up_meet_resistance_current == 0xFFFF)||
        (up_meet_resistance_current == 0) ||
        (ret != 0)){
        up_meet_resistance_current = UP_MEET_RESISTANCE_CURRENT_MA;
    }
    ret = getLimitedCurrent(&up_big_damp_current, UP_GIG_DAMP);
    if((up_big_damp_current == 0xFFFF)||
        (up_big_damp_current == 0) ||
        (ret != 0)){
        up_big_damp_current = UP_BIG_DAMP_CURRENT_MA;
    }
    ret = getLimitedCurrent(&down_meet_resistance_current, DOWN_RESISTANCE);
    if((down_meet_resistance_current == 0xFFFF)||
        (down_meet_resistance_current == 0) ||
        (ret != 0)){
        down_meet_resistance_current = DOWN_MEET_RESISTANCE_CURRENT_MA;
    }
    ret = getLimitedCurrent(&down_big_damp_current, DOWN_GIG_DAMP);
    if((down_big_damp_current == 0xFFFF)||
        (down_big_damp_current == 0) ||
        (ret != 0)){
        down_big_damp_current = DOWN_BIG_DAMP_CURRENT_MA;
    }
    return 0;
}
