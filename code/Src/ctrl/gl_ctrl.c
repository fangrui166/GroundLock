#include "gl_ctrl.h"
#include "stm32f1xx_hal.h"
#include "gl_manager.h"
#include <contiki.h>
#include "hlog.h"
#include "distance_manager.h"
static Motor_state_t motor_state = MOTOR_STOP;
PROCESS(ctrl_process, "ctrl");
Motor_state_t Ctrl_GetMotorState(void)
{
    return motor_state;
}

int Ctrl_MotorOff(void)
{
    motor_state = MOTOR_STOP;
    HAL_GPIO_WritePin(CTRL_PORT, CTRL1_GPIO, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTRL_PORT, CTRL2_GPIO, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CTRL_PORT, CTRL3_GPIO, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTRL_PORT, CTRL4_GPIO, GPIO_PIN_SET);
    return 0;
}
int Ctrl_MotorUp(void)
{
    motor_state = MOTOR_MOVUP;
    HAL_GPIO_WritePin(CTRL_PORT, CTRL1_GPIO, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTRL_PORT, CTRL2_GPIO, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTRL_PORT, CTRL3_GPIO, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CTRL_PORT, CTRL4_GPIO, GPIO_PIN_SET);
    return 0;
}
int Ctrl_MotorDown(void)
{
    motor_state = MOTOR_MOVDOWN;
    HAL_GPIO_WritePin(CTRL_PORT, CTRL1_GPIO, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CTRL_PORT, CTRL2_GPIO, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CTRL_PORT, CTRL3_GPIO, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTRL_PORT, CTRL4_GPIO, GPIO_PIN_RESET);
    return 0;
}

Ctrl_status_t Ctrl_GetStatus(void)
{
    Ctrl_status_t status = CTRL_STATUS_UNKOWN;
    GPIO_PinState up = HAL_GPIO_ReadPin(SW_PORT, SWUP_GPIO);
    GPIO_PinState down = HAL_GPIO_ReadPin(SW_PORT, SWDOWN_GPIO);
    if(down == GPIO_PIN_RESET){
        status = CTRL_STATUS_MOVING;
    }
    else{
        if(up == GPIO_PIN_RESET){
            status = CTRL_STATUS_UP;
        }
        else{
            status = CTRL_STATUS_DOWN;
        }
    }
    return status;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if((GPIO_Pin == SWUP_GPIO)  || (GPIO_Pin == SWDOWN_GPIO)){
        process_poll(&ctrl_process);
    }
}
static int Ctrl_PinInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = SWUP_GPIO | SWDOWN_GPIO;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SW_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    GPIO_InitStruct.Pin = CTRL1_GPIO | CTRL2_GPIO | CTRL3_GPIO | CTRL4_GPIO;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CTRL_PORT, &GPIO_InitStruct);
    Ctrl_MotorOff();
    return 0;
}
int Ctrl_Init(void)
{
    Ctrl_PinInit();


    process_start(&ctrl_process,NULL);
    return 0;
}


PROCESS_THREAD(ctrl_process, ev, data)
{
    PROCESS_BEGIN();
    while(1)
    {
        PROCESS_WAIT_EVENT();
        if (PROCESS_EVENT_POLL == ev){
            Ctrl_status_t status = Ctrl_GetStatus();
            if((status == CTRL_STATUS_MOVING) && (motor_state == MOTOR_MOVDOWN)){
                logi("start moving down\n");
                Gl_SetLockState(LOCK_STATE_MOVING);
            }
            else if((status == CTRL_STATUS_MOVING) && (motor_state == MOTOR_MOVUP)){
                logi("start moving up\n");
                Gl_SetLockState(LOCK_STATE_MOVING);
            }
            else if((status == CTRL_STATUS_UP) && (motor_state == MOTOR_MOVUP)){
                logi("arrived top\n");
                Gl_CtrlLock(STOP);
                Dist_StopMesure();
                Gl_SetLockState(LOCK_STATE_LOCKED);
            }
            else if((status == CTRL_STATUS_DOWN) && (motor_state == MOTOR_MOVDOWN)){
                logi("arrived bottom\n");
                Gl_CtrlLock(STOP);
                Dist_StartMesure();
                if(Gl_GetActionErrStage() == ACTION_ERR_UP_ERR){
                    Gl_SetLockState(LOCK_STATE_MISSUPSUCCEDOWN);
                }
                else{
                    Gl_SetLockState(LOCK_STATE_UNLOCKED);
                }
            }
            else{
                logi("what's happend? Ctrl_status:%d, motor_state:%d\n", status, motor_state);
            }
        }
    }
    PROCESS_END();
}

