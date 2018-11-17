#ifndef __UART_DEBUG_DRV_H__
#define __UART_DEBUG_DRV_H__

#include "stdint.h"

#define DEBUG_UART_PORT         USART2
#define DEBUGUART_IRQn          USART2_IRQn


void DebugUart_Init(void);
void DebugUart_IRQHandler(void);
int DebugUart_Printf(char *fmt, ...);
int DebugUart_Transmit(uint8_t * buf, uint16_t len);
#endif
