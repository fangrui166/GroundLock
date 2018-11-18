#include "wdt.h"
#include <clock.h>
#include <sys/process.h>
#include <etimer.h>

IWDG_HandleTypeDef hiwdg;

/*WDT PROCESS*/
PROCESS(wdg_poll_handler, "wdg_poll_handler");
PROCESS_THREAD(wdg_poll_handler, ev, data)
{
    /* this code will execute for every event */
    PROCESS_BEGIN();
    while(1)
    {
        static struct etimer wdg_timer;
        etimer_set(&wdg_timer, CLOCK_SECOND*2);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&wdg_timer));
        wdt_reset();
    }
    PROCESS_END();
}
/*WDT PROCESS end*/

/* IWDG init function */
int wdt_init(void)
{
    HAL_StatusTypeDef ret = HAL_OK;
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32; // watchdog reset timeout 3.276s
    hiwdg.Init.Reload = 4095;
    HAL_IWDG_Init(&hiwdg);
    process_start(&wdg_poll_handler,NULL);
    printf("[WDT] ""watchdog enable\r\n");
    return (ret != HAL_OK);
}

void wdt_reset(void)
{
    if(hiwdg.Instance)
        HAL_IWDG_Refresh(&hiwdg);
}

void wdt_iwdg_Init_for_suspend(void){
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256; // watchdog reset timeout 26.2s
    hiwdg.Init.Reload = 4095;
    HAL_IWDG_Init(&hiwdg);
}

void wdt_iwdg_Init_for_normal(void){
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32; // watchdog reset timeout 3.276s
    hiwdg.Init.Reload = 4095;
    HAL_IWDG_Init(&hiwdg);
}

