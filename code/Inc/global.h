#ifndef __GLOBAL_H__
#define __GLOBAL_H__


#include <contiki.h>

extern uint8_t local_addr;
extern uint32_t BaudRate;
extern uint16_t up_meet_resistance_current;
extern uint16_t up_big_damp_current ;
extern uint16_t down_meet_resistance_current;
extern uint16_t down_big_damp_current;

#define DEFAULT_LOCAL_ADDR                      0x00
#define DEFAULT_BAUDRATE                        9600
#define UP_MEET_RESISTANCE_CURRENT_MA           1500
#define UP_BIG_DAMP_CURRENT_MA                  1200
#define DOWN_MEET_RESISTANCE_CURRENT_MA         1200
#define DOWN_BIG_DAMP_CURRENT_MA                1000

#define INIT_MISC_VARIABLE(var, value, default_value, max)              \
                            do{                                         \
                                if((value) == (max)){                   \
                                    (var) = (default_value);            \
                                }                                       \
                                else {                                  \
                                    (var) = (value);                    \
                                }                                       \
                            }while(0)


void global_variable_init(void);

#endif
