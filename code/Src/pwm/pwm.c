#include "pwm.h"
#include "hlog.h"
#include "stm32f1xx_hal.h"

TIM_HandleTypeDef htim1;
static uint32_t channel_count_a, channel_count_b;
static uint8_t channel_a_start, channel_b_start;
static volatile uint32_t max_pwm_count_a = 9;
static volatile uint32_t max_pwm_count_b = 9;
int PWM_ChannelAStart(uint32_t pwm_count)
{
    if(channel_a_start) return 0;
    channel_a_start = 1;
    logi("%s\n", __func__);
    channel_count_a = 0;
    max_pwm_count_a = pwm_count;
    HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1);
    return 0;
}
int PWM_ChannelBStart(uint32_t pwm_count)
{
    if(channel_b_start) return 0;
    channel_b_start = 1;
    logi("%s\n", __func__);
    channel_count_b = 0;
    max_pwm_count_b = pwm_count;
    HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_4);
    return 0;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    //logi("%s\n", __func__);
    if(htim->Instance == htim1.Instance){
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
            if(++channel_count_a >= max_pwm_count_a){
                HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_1);
                channel_count_a = 0;
                channel_a_start = 0;
                logi("pwm a stop\n");
            }
        }
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4){
            if(++channel_count_b >= max_pwm_count_b){
                HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_4);
                channel_count_b = 0;
                channel_b_start = 0;
                logi("pwm b stop\n");
            }
        }
    }
}
void PWM_CSBIN1_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(CSB_IN1) != RESET){
        __HAL_GPIO_EXTI_CLEAR_IT(CSB_IN1);
        if(HAL_GPIO_ReadPin(CSB_PORT, CSB_IN1)){
            //logi("+\n");
        }
        else{
            //logi("-\n");
        }
    }

}
void PWM_CSBIN2_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(CSB_IN2) != RESET){
        __HAL_GPIO_EXTI_CLEAR_IT(CSB_IN2);
        if(HAL_GPIO_ReadPin(CSB_PORT, CSB_IN2)){
            //logi(",\n");
        }
        else{
            //logi(".\n");
        }
    }
}
static int PWM_TimInit(void)
{
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 36;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 49;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
    {
      loge(__FILE__, __LINE__);
    }

    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
    {
      loge(__FILE__, __LINE__);
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
      loge(__FILE__, __LINE__);
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 25;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
      loge(__FILE__, __LINE__);
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
    {
      loge(__FILE__, __LINE__);
    }

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
    {
      loge(__FILE__, __LINE__);
    }

    extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);

    HAL_TIM_MspPostInit(&htim1);

    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 2, 7);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
    return 0;
}
int PWM_GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = CSB_IN1 | CSB_IN2;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CSB_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    return 0;
}
int PWM_Init(void)
{
    PWM_TimInit();
    PWM_GpioInit();
    return 0;
}
