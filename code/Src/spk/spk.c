#include "spk.h"
#include "stm32f1xx_hal.h"
#include <contiki.h>
#include <sys/ctimer.h>
#include "global.h"


static struct ctimer spk_timer;

static void SPK_TimeoutCallback(void *ptr)
{
    HAL_GPIO_WritePin(SPK_PORT, SPK_GPIO, GPIO_PIN_RESET);
}

int SPK_Start(uint32_t timeout_ms)
{
    if(spk_switch == SPK_SWITCH_OFF) return 0;
    HAL_GPIO_WritePin(SPK_PORT, SPK_GPIO, GPIO_PIN_SET);
    ctimer_set(&spk_timer, timeout_ms, SPK_TimeoutCallback, NULL);
    return 0;
}
static int SPK_GpioInit(void)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = SPK_GPIO;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SPK_PORT, &GPIO_InitStruct);

    return 0;
}
int SPK_Init(void)
{
    SPK_GpioInit();
    return 0;
}
