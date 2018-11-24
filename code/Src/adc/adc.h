#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>


void ADC_Init(void);
void ADC_StartDMA(void);
void ADC_StopDMA(void);
int ADC_GetRawData(uint16_t * adc_raw_data);
uint32_t ADC_GetCurrentAverage(void);
#endif
