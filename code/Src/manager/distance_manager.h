#ifndef __DISTANCE_MANAGER_H__
#define __DISTANCE_MANAGER_H__
#include <contiki.h>

#define DISTANCE_FOR_CAR_DETECTED       300 // cm

typedef struct {
    uint32_t distance_a;
    uint32_t distance_b;
    uint8_t result_msk;
}dist_mag_t;

typedef enum{
    ULT_SWITCH_OFF,
    ULT_SWITCH_ON,
    ULT_SWITCH_READ,
}Ultrasound_switch_t;

int Dist_StopMesure(void);
int Dist_StartMesure(void);
void Dist_MesureDone(uint32_t distance, uint8_t channel);
uint32_t Dist_GetCSBNum(void);
void Dist_SetCSBTIME(uint8_t time_s);
uint8_t Dist_GetMesureState(void);
uint8_t Dist_GetCSBTIME(void);
void Dist_SetCSB_NUM_MAX(uint8_t time_s);
uint8_t Dist_GetCSB_NUM_MAX(void);
uint8_t Dist_GetCSB_NUM(void);
int Dist_Init(void);
#endif
