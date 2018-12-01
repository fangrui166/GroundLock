#ifndef __GL_MANAGER_H__
#define __GL_MANAGER_H__

#include <stdint.h>
#define LOCK_STATE_LOCKED               0x00
#define LOCK_STATE_UNLOCKED             0x01
#define LOCK_STATE_MOVING               0x88
#define LOCK_STATE_MISSUPSUCCEDOWN      0x03
#define LOCK_STATE_FIALDOWN             0x02
#define LOCK_STATE_NOCARUNLOCKED        0x10


#define UP_MEET_RESISTANCE_CURRENT_MA         1500
#define UP_BIG_DAMP_CURRENT_MA                1200
#define DOWN_MEET_RESISTANCE_CURRENT_MA       1200
#define DOWN_BIG_DAMP_CURRENT_MA              1000

#define MOVTIMEOUT_BUZZER_TIME                1500 // 1.5s
typedef enum{
    LOCK,
    UNLOCK,
    STOP,
}LOCK_ACTION;
typedef enum{
    LOCK_TOP,
    LOCK_MIDDLE,
    LOCK_BOTTOM,
}Lock_location;

typedef enum{
    ACTION_ERR_UNKOWN,
    ACTION_ERR_UP_ERR,
    ACTION_ERR_DOWN_ERR,
}Action_err_stage_t;


uint8_t Gl_GetLockState(void);
void Gl_SetLockState(uint8_t state);
uint8_t Gl_GetActionErrStage(void);
int Gl_CtrlLock(uint8_t action);
Lock_location Gl_GetLocation(void);
int Gl_LockOn(void);
int Gl_LockOff(void);
int Gl_ManagerInit(void);

#endif
