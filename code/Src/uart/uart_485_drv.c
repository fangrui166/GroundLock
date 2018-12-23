#include "uart_485_drv.h"
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "stdarg.h"
#include "stm32f1xx_hal.h"
#include <contiki.h>
#include "shellTask.h"
#include "trx485_if.h"
#include "hlog.h"
#include "global.h"
#include "unit.h"

#define TRX485_UART_RX_BUFFER_SIZE           255 // you'd better set as 255 to make code simply
#define TRX485_UART_TX_BUFFER_SIZE           255
#define TRX485_UART_RX_BUFFER_CACHE          10
static uint8_t trx485_uart_rx_buf[TRX485_UART_RX_BUFFER_SIZE] = {0};
static uint8_t trx485_uart_tx_buf[TRX485_UART_TX_BUFFER_SIZE] = {0};

static UART_HandleTypeDef trx485_uart;
typedef struct{
    uint8_t start;
    uint8_t end;
}item_idx_range_t;
typedef struct {
    uint8_t * ringbuf;
    uint8_t rxbuf_wr_idx;
    uint8_t item_wr_idx;
    uint8_t item_rd_idx;
    item_idx_range_t item_idx_range[TRX485_UART_RX_BUFFER_CACHE];
}rx_ring_buf_t;
static rx_ring_buf_t trx485_rx = {
    .ringbuf = trx485_uart_rx_buf,
    .rxbuf_wr_idx = 0,
    .item_wr_idx = 0,
    .item_rd_idx = 0,
    .item_idx_range = {0},
    };


PROCESS(TRx485Uart_Handler, "TRx485Uart_Handler");

void TRx485Uart_FlushRxBuf(void)
{
	HAL_NVIC_DisableIRQ(TRX485UART_IRQn);
	trx485_rx.item_rd_idx = 0;
	trx485_rx.item_wr_idx = 0;
    trx485_rx.rxbuf_wr_idx = 0;
    trx485_rx.item_idx_range[0].start = 0;
	HAL_NVIC_EnableIRQ(TRX485UART_IRQn);
}

static void TRx485Uart_RxInit(void){
	memset(trx485_uart_rx_buf, 0, sizeof(trx485_uart_rx_buf));
	TRX485_UART_PORT->CR1 |= USART_CR1_RXNEIE | USART_CR1_IDLEIE;
	HAL_NVIC_SetPriority(TRX485UART_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TRX485UART_IRQn);
    TRx485Uart_FlushRxBuf();
}
void TRx485Uart_Init(void)
{
    trx485_uart.Instance = TRX485_UART_PORT;
    trx485_uart.Init.BaudRate = BaudRate;
    trx485_uart.Init.WordLength = UART_WORDLENGTH_8B;
    trx485_uart.Init.StopBits = UART_STOPBITS_1;
    trx485_uart.Init.Parity = UART_PARITY_NONE;
    trx485_uart.Init.Mode = UART_MODE_TX_RX;
    trx485_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    trx485_uart.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&trx485_uart) != HAL_OK)    {
        logi("uart init error\n");
    }
    TRx485Uart_RxInit();
    process_start(&TRx485Uart_Handler,NULL);

    logi("%s\n",__func__);
}
static void TRx485Uart_FrameDone(void)
{
    trx485_rx.item_idx_range[trx485_rx.item_wr_idx].end = trx485_rx.rxbuf_wr_idx-1;
    if(((trx485_rx.item_wr_idx+1)%TRX485_UART_RX_BUFFER_CACHE) != trx485_rx.item_rd_idx){
        trx485_rx.item_wr_idx = (trx485_rx.item_wr_idx+1)%TRX485_UART_RX_BUFFER_CACHE;
        trx485_rx.item_idx_range[trx485_rx.item_wr_idx].start = trx485_rx.rxbuf_wr_idx;
    }
    process_poll(&TRx485Uart_Handler);
}
void TRx485Uart_IRQHandler(void)
{
    UART_HandleTypeDef *huart = &trx485_uart;
    uint32_t isrflags = READ_REG(huart->Instance->SR);
    uint32_t cr1its     = READ_REG(huart->Instance->CR1);
    uint32_t cr3its     = READ_REG(huart->Instance->CR3);
    uint32_t errorflags = 0x00U;
    uint32_t dmarequest = 0x00U;

    errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));

    /* If no error occurs */
    if (errorflags == RESET){
      /* UART in mode Receiver ---------------------------------------------------*/
      if(((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET)){
        trx485_rx.ringbuf[trx485_rx.rxbuf_wr_idx++] = (uint8_t)(huart->Instance->DR);
        return;
      }
      else if((isrflags & USART_SR_IDLE) != RESET){
        dmarequest = (uint8_t)(huart->Instance->SR);
        dmarequest = (uint8_t)(huart->Instance->DR);
        TRx485Uart_FrameDone();
        return;
      }
    }
    /* If some errors occur */
    cr3its = READ_REG(huart->Instance->CR3);
    if((errorflags != RESET)&&
        (((cr3its & USART_CR3_EIE) != RESET)||
         ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET))
       ){
        /* UART parity error interrupt occurred ----------------------------------*/
        if(((isrflags & USART_SR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
        {
          huart->ErrorCode |= HAL_UART_ERROR_PE;
        }

        /* UART noise error interrupt occurred -----------------------------------*/
        if(((isrflags & USART_SR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
        {
          huart->ErrorCode |= HAL_UART_ERROR_NE;
        }

        /* UART frame error interrupt occurred -----------------------------------*/
        if(((isrflags & USART_SR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
        {
          huart->ErrorCode |= HAL_UART_ERROR_FE;
        }

        /* UART Over-Run interrupt occurred --------------------------------------*/
        if(((isrflags & USART_SR_ORE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
        {
          huart->ErrorCode |= HAL_UART_ERROR_ORE;
        }

        /* Call UART Error Call back function if need be --------------------------*/
        if(huart->ErrorCode != HAL_UART_ERROR_NONE)
        {
          /* UART in mode Receiver -----------------------------------------------*/
          if(((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
          {
              trx485_rx.ringbuf[trx485_rx.rxbuf_wr_idx++] = (uint8_t)(huart->Instance->DR);
          }

          /* If Overrun error occurs, or if any error occurs in DMA mode reception,
             consider error as blocking */
          dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
          if(((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET) || dmarequest)
          {
            /* Blocking error : transfer is aborted
               Set the UART state ready to be able to start again the process,
               Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
                /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
                CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
                CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

                /* At end of Rx process, restore huart->RxState to Ready */
                huart->RxState = HAL_UART_STATE_READY;

            /* Call user error callback */
            HAL_UART_ErrorCallback(huart);
          }
          else
          {
            /* Non Blocking error : transfer could go on.
               Error is notified to user through user error callback */
            HAL_UART_ErrorCallback(huart);
            huart->ErrorCode = HAL_UART_ERROR_NONE;
          }
        }

      } /* End if some error occurs */



}

void TRx485Uart_PollBuf(void)
{
    process_poll(&TRx485Uart_Handler);
}

/*
static int TRx485Uart_RxBufItemCount(void)
{
    return ((trx485_rx.item_wr_idx - trx485_rx.item_rd_idx)%TRX485_UART_RX_BUFFER_CACHE);
}
*/
static int TRx485Uart_RxBufLen(uint8_t index){
	return ((trx485_rx.item_idx_range[index].end - trx485_rx.item_idx_range[index].start)%TRX485_UART_RX_BUFFER_SIZE)+1;
}

static void TRx485Uart_RxBufRead(uint8_t * data, uint8_t len){
	for(uint8_t i = 0; i < len; i++){
		data[i] = trx485_rx.ringbuf[trx485_rx.item_idx_range[trx485_rx.item_rd_idx].start++];
	}
    trx485_rx.item_rd_idx = (trx485_rx.item_rd_idx+1)%TRX485_UART_RX_BUFFER_CACHE;
}
int TRx485Uart_Printf(char *fmt, ...)
{
    int retval;
    va_list vp;
    char logmsg_buf[TRX485_UART_RX_BUFFER_SIZE]={0};

    va_start(vp, fmt);
    retval = vsnprintf(logmsg_buf, TRX485_UART_RX_BUFFER_SIZE, fmt, vp);
    va_end(vp);

    HAL_UART_Transmit(&trx485_uart, (uint8_t*)logmsg_buf, retval, 50);

    return retval;
}
int TRx485Uart_Transmit(uint8_t * buf, uint16_t len)
{
    int ret;
    ret = HAL_UART_Transmit(&trx485_uart, buf, len, 50);
    return ret;
}

uint8_t * TRx485Get_TxBuf(void)
{
    return trx485_uart_tx_buf;
}

PROCESS_THREAD(TRx485Uart_Handler, ev, data)
{
    /* this code will execute for every event */
    PROCESS_BEGIN();
    while(1)
    {
        PROCESS_YIELD();
        if(ev == PROCESS_EVENT_POLL )
        {
            do{
                uint8_t data[TRX485_UART_RX_BUFFER_SIZE] = {0};
                uint8_t len = TRx485Uart_RxBufLen(trx485_rx.item_rd_idx);
                TRx485Uart_RxBufRead(data,len);
                if(len ) {
                    buffer_dump(data, len);
                    //logi("%s",data);
                    TRx485_ParsePacket(data,len);
                }
            }while(trx485_rx.item_rd_idx != trx485_rx.item_wr_idx);
        }
    }
    PROCESS_END();
}


