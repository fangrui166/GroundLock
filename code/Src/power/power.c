#include "power.h"
#include "hlog.h"
#include "stm32f1xx_hal.h"




int PWRMGR_SYSTEM_POWER_RESET(void)
{
    __disable_irq();
    HAL_NVIC_SystemReset();
    return 0;
}
