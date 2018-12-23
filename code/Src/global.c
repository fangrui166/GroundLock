#include "global.h"
#include "misc_data_ro.h"
#include "hlog.h"
#include "spk.h"


uint8_t local_addr = 0;
uint32_t BaudRate;
uint16_t up_meet_resistance_current;
uint16_t up_big_damp_current ;
uint16_t down_meet_resistance_current;
uint16_t down_big_damp_current;
uint32_t CSB_TIME;
uint32_t CSB_NUM_MAX;
uint8_t spk_switch;


void global_variable_init(void)
{
    int ret;
    MISC_DataTypeDefRO *MiscDataRO = NULL;
    ret = getMiscDataRO(&MiscDataRO);
    if(ret == -1){
        local_addr = DEFAULT_LOCAL_ADDR;
        BaudRate = DEFAULT_BAUDRATE;
        up_meet_resistance_current = UP_MEET_RESISTANCE_CURRENT_MA;
        up_big_damp_current = UP_BIG_DAMP_CURRENT_MA;
        down_meet_resistance_current = DOWN_MEET_RESISTANCE_CURRENT_MA;
        down_big_damp_current = DOWN_BIG_DAMP_CURRENT_MA;
        CSB_TIME = CSB_TIME_DEFAULT;
        CSB_NUM_MAX = CSB_NUM_MAX_DEFAULT;
        spk_switch = SPK_SWITCH_ON;
    }
    else{
        INIT_MISC_VARIABLE(local_addr, MiscDataRO->local_addr, DEFAULT_LOCAL_ADDR, 0xFF);
        INIT_MISC_VARIABLE(BaudRate, MiscDataRO->BaudRate, DEFAULT_BAUDRATE, 0xFFFFFFFF);
        INIT_MISC_VARIABLE(up_meet_resistance_current, MiscDataRO->up_meet_resistance_current, UP_MEET_RESISTANCE_CURRENT_MA, 0xFFFF);
        INIT_MISC_VARIABLE(up_big_damp_current, MiscDataRO->up_big_damp_current, UP_BIG_DAMP_CURRENT_MA, 0xFFFF);
        INIT_MISC_VARIABLE(down_meet_resistance_current, MiscDataRO->down_meet_resistance_current, DOWN_MEET_RESISTANCE_CURRENT_MA, 0xFFFF);
        INIT_MISC_VARIABLE(down_big_damp_current, MiscDataRO->down_big_damp_current, DOWN_BIG_DAMP_CURRENT_MA, 0xFFFF);
        INIT_MISC_VARIABLE(CSB_TIME, MiscDataRO->CSB_TIME, CSB_TIME_DEFAULT, 0xFFFFFFFF);
        INIT_MISC_VARIABLE(CSB_NUM_MAX, MiscDataRO->CSB_NUM_MAX, CSB_NUM_MAX_DEFAULT, 0xFFFFFFFF);
        INIT_MISC_VARIABLE(spk_switch, MiscDataRO->spk_switch, SPK_SWITCH_ON, 0xFF);
    }

}
