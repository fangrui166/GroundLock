#include "distance_manager.h"
#include <contiki.h>
#include "global.h"
#include "pwm.h"
#include "hlog.h"
#include "gl_manager.h"
#include "stm32f1xx_hal.h"


static process_event_t ev_distance_done;
static struct ctimer dist_csb_timer;
static dist_mag_t data = {0};
static uint32_t CSB_NUM = 0;
static uint8_t stop_mesure = 1;
static uint8_t no_car_detected = 0;

PROCESS(distance_process, "distance");
static void Dist_CsbTimercb(void *ptr)
{
    if(stop_mesure) return;
    data.result_msk = 0;
    data.distance_a = 0;
    data.distance_b = 0;
    PWM_ChannelAStart(NULL);
}
uint8_t Dist_GetMesureState(void)
{
    return !stop_mesure;
}
int Dist_StopMesure(void)
{
    stop_mesure = 1;
    ctimer_stop(&dist_csb_timer);
    return 0;
}
int Dist_StartMesure(void)
{
    stop_mesure = 0;
    ctimer_set(&dist_csb_timer, CSB_TIME, Dist_CsbTimercb, NULL);
    return 0;
}
void Dist_MesureDone(uint32_t distance, uint8_t channel)
{
    if(stop_mesure) return;
    if(channel == PWM_Channel_A){
        data.distance_a = distance;
        data.result_msk |= PWM_ChannelA_msk;
    }
    else if(channel == PWM_Channel_B){
        data.distance_b = distance;
        data.result_msk |= PWM_ChannelB_msk;
    }
    process_post(&distance_process, ev_distance_done, &data);
}
uint32_t Dist_GetCSBNum(void)
{
    return CSB_NUM;
}
void Dist_SetCSBTIME(uint8_t time_s)
{
    CSB_TIME = time_s*1000;
    setCSBTIME(CSB_TIME);
}
uint8_t Dist_GetCSBTIME(void)
{
    return (CSB_TIME/1000);
}
void Dist_SetCSB_NUM_MAX(uint8_t time_s)
{
    CSB_NUM_MAX = time_s*1000;
    setCSBTIME(CSB_NUM_MAX);
}
uint8_t Dist_GetCSB_NUM_MAX(void)
{
    return (CSB_NUM_MAX/1000);
}
uint8_t Dist_GetCSB_NUM(void)
{
    return (CSB_NUM/1000);
}

int Dist_Init(void)
{
    ev_distance_done = process_alloc_event();
    process_start(&distance_process,NULL);
    return 0;
}
static void Dist_ProcessDistanceEv(void* data)
{
    if(data == NULL) return;
    dist_mag_t * data_p = (dist_mag_t*) data;
    if(data_p->result_msk == PWM_ChannelA_msk){
        PWM_ChannelBStart(NULL);
    }
    else if(data_p->result_msk == PWM_ChannelB_msk){
        PWM_ChannelAStart(NULL);
    }
    else if(data_p->result_msk == (PWM_ChannelA_msk | PWM_ChannelB_msk)){
        if((data_p->distance_a <= DISTANCE_FOR_CAR_DETECTED) ||
            (data_p->distance_b <= DISTANCE_FOR_CAR_DETECTED)){
            // car detected
            no_car_detected = 0;
            Dist_StartMesure();
        }
        else{
            // No car
            static uint32_t csb_current_num;
            if(no_car_detected == 0){
                no_car_detected = 1;
                csb_current_num = HAL_GetTick();
            }
            CSB_NUM = HAL_GetTick() - csb_current_num;
            if(CSB_NUM >= CSB_NUM_MAX){
                logi("LOCK_STATE_NOCARUNLOCKED\n");
                Gl_SetLockState(LOCK_STATE_NOCARUNLOCKED);
            }
            Dist_CsbTimercb(NULL);
        }
    }
}
PROCESS_THREAD(distance_process, ev, data)
{
    PROCESS_BEGIN();
    while(1)
    {
        PROCESS_WAIT_EVENT();
        if (PROCESS_EVENT_POLL == ev){
        }
        else if(ev == ev_distance_done){
            Dist_ProcessDistanceEv(data);
        }
    }
    PROCESS_END();
}
