//
// Created by nparikh on 8/18/2017.
//

#ifndef WDT_H
#define WDT_H

#include "stm32f1xx_hal.h"

int wdt_init(void);

void wdt_reset(void);
void wdt_iwdg_Init_for_normal(void);

void wdt_iwdg_Init_for_suspend(void);


#endif //WDT_H
