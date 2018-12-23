#ifndef __SPK_H__
#define __SPK_H__

#include <stdint.h>

#define SPK_GPIO  GPIO_PIN_7

#define SPK_PORT   GPIOA

typedef enum{
    SPK_SWITCH_OFF,
    SPK_SWITCH_ON,
    SPK_SWITCH_READ,
}SPK_PARS_T;
int SPK_Init(void);
int SPK_Start(uint32_t timeout_ms);

#endif
