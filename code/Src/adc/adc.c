#include "adc.h"
#include "stm32f1xx_hal.h"
#include <contiki.h>
#include "ring_buffer.h"
#include "hlog.h"
#define ADC_BUFFER_CURRENT_MAX_LEN  5
#define ADC_AVERAGE_NUM             10
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

static RingBuffer_t adc_current_buffer;
uint32_t adc_convert_value;

uint32_t adc_current_average;

uint32_t ADC_GetCurrentAverage(void)
{
    uint8_t  adc_sample_num = 0;
    uint16_t adc_volatage_mv,adc_raw_data,adc_raw_average;
    uint32_t adc_raw_sum = 0,adc_raw_max = 0,adc_raw_min = 0xFFFFFFFF;
    uint32_t adc_current_mA;
    for(int i=0;i<ADC_AVERAGE_NUM*10;i++){
        if(!ADC_GetRawData(&adc_raw_data)){
            adc_raw_sum += adc_raw_data;
            adc_sample_num ++;
            if(adc_raw_data > adc_raw_min){
                adc_raw_max = adc_raw_data;
            }
            if(adc_raw_data < adc_raw_min){
                adc_raw_min = adc_raw_data;
            }
        }
        else{
            //delay wait adc to sample data
            HAL_Delay(1);
        }
        if(adc_sample_num == ADC_AVERAGE_NUM){
            break;
        }
    }
    if(adc_sample_num >= 5){
        //sample great than 5 is valid
        adc_sample_num = adc_sample_num - adc_raw_max - adc_raw_min;
        adc_raw_average = adc_sample_num/(adc_sample_num-2);
        adc_volatage_mv = ((adc_raw_average << 8) * 891) >> 16;
        logi("adc_volatage_mv:%d\n", adc_volatage_mv);
        adc_current_mA = ((adc_volatage_mv*2)); // I = U/R  , R = 0.5Ω
        adc_current_average = adc_current_mA;
    }
    else{
        // use last value if sample data not enough
        adc_current_mA = adc_current_average;
    }
    return adc_current_mA;

}
int ADC_GetRawData(uint16_t * adc_raw_data)
{
	uint32_t adc_value = 0;
	if(ring_buffer_empty(&adc_current_buffer)){
		logi("hall buffer empty\r\n");
		return -1;
	}
	adc_value = *((uint32_t *)ring_buffer_dequeue(&adc_current_buffer));
	*adc_raw_data = (uint16_t)adc_value;
	return 0;
}

void ADC_StartDMA(void)
{
    HAL_ADC_Start_DMA(&hadc1,(uint32_t *)&adc_convert_value,1);
}

void ADC_StopDMA(void)
{
	HAL_ADC_Stop_DMA(&hadc1);
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    logi("adc:%d\n", adc_convert_value);
    ring_buffer_enqueue(&adc_current_buffer, (uint8_t *) &adc_convert_value);
}

static void ADC_BufferInit(void)
{
    ring_buffer_init(&adc_current_buffer, sizeof(uint32_t), ADC_BUFFER_CURRENT_MAX_LEN);
}

/* ADC1 init function */
void ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig;

    /**Common config
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  HAL_ADC_Init(&hadc1);

    /**Configure Regular Channel
    */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  /*ADC self calibration*/
  if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK){
      logi("ADC selt calibration Error\r\n");
  }

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  ADC_BufferInit();
  ADC_StartDMA();

}



