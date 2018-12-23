#ifndef __UART_TRX485_DRV_H__
#define __UART_TRX485_DRV_H__

#include "stdint.h"

#define TRX485_UART_PORT         USART1
#define TRX485UART_IRQn          USART1_IRQn

typedef enum
{
    BAUDRATE_9600,
    BAUDRATE_4800,
    BAUDRATE_2400,
    BAUDRATE_1200,
    BAUDRATE_600,
}BAUDRATE_t;
void TRx485Uart_Init(void);
void TRx485Uart_IRQHandler(void);
int TRx485Uart_Printf(char *fmt, ...);
int TRx485Uart_Transmit(uint8_t * buf, uint16_t len);
uint8_t * TRx485Get_TxBuf(void);
#endif
