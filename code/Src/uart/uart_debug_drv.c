#include "uart_debug_drv.h"
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "stdarg.h"
#include "stm32f1xx_hal.h"
#include <contiki.h>
#include "shellTask.h"
#include <sys/process.h>


#define DEBUG_UART_RX_BUFFER_SIZE           255 // you'd better set as 255 to make code simply
#define DEBUG_UART_RX_BUFFER_CACHE          10
static uint8_t debug_uart_rx_buf[DEBUG_UART_RX_BUFFER_SIZE] = {0};
static UART_HandleTypeDef debug_uart;
typedef struct{
    uint8_t start;
    uint8_t end;
}item_idx_range_t;
typedef struct {
    uint8_t * ringbuf;
    uint8_t rxbuf_wr_idx;
    uint8_t item_wr_idx;
    uint8_t item_rd_idx;
    item_idx_range_t item_idx_range[DEBUG_UART_RX_BUFFER_CACHE];
}rx_ring_buf_t;
static rx_ring_buf_t debug_rx = {
    .ringbuf = debug_uart_rx_buf,
    .rxbuf_wr_idx = 0,
    .item_wr_idx = 0,
    .item_rd_idx = 0,
    .item_idx_range = {0},
    };


PROCESS(DebugUart_Handler, "DebugUart_Handler");

void DebugUart_FlushRxBuf(void)
{
	HAL_NVIC_DisableIRQ(DEBUGUART_IRQn);
	debug_rx.item_rd_idx = 0;
	debug_rx.item_wr_idx = 0;
    debug_rx.rxbuf_wr_idx = 0;
    debug_rx.item_idx_range[0].start = 0;
	HAL_NVIC_EnableIRQ(DEBUGUART_IRQn);
}

static void DebugUart_RxInit(void){
	memset(debug_uart_rx_buf, 0, sizeof(debug_uart_rx_buf));
	DEBUG_UART_PORT->CR1 |= USART_CR1_RXNEIE | USART_CR1_IDLEIE;
	HAL_NVIC_SetPriority(DEBUGUART_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DEBUGUART_IRQn);
    DebugUart_FlushRxBuf();
}
void DebugUart_Init(void)
{
    debug_uart.Instance = DEBUG_UART_PORT;
    debug_uart.Init.BaudRate = 115200;
    debug_uart.Init.WordLength = UART_WORDLENGTH_8B;
    debug_uart.Init.StopBits = UART_STOPBITS_1;
    debug_uart.Init.Parity = UART_PARITY_NONE;
    debug_uart.Init.Mode = UART_MODE_TX_RX;
    debug_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    debug_uart.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&debug_uart) != HAL_OK)    {
        printf("uart init error\n");
    }
    DebugUart_RxInit();
    process_start(&DebugUart_Handler,NULL);

    printf("%s\n",__func__);
}
static void DebugUart_FrameDone(void)
{
    debug_rx.item_idx_range[debug_rx.item_wr_idx].end = debug_rx.rxbuf_wr_idx-1;
    if(((debug_rx.item_wr_idx+1)%DEBUG_UART_RX_BUFFER_CACHE) != debug_rx.item_rd_idx){
        debug_rx.item_wr_idx = (debug_rx.item_wr_idx+1)%DEBUG_UART_RX_BUFFER_CACHE;
        debug_rx.item_idx_range[debug_rx.item_wr_idx].start = debug_rx.rxbuf_wr_idx;
    }

    process_poll(&DebugUart_Handler);
}
void DebugUart_IRQHandler(void)
{
    UART_HandleTypeDef *huart = &debug_uart;
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
        debug_rx.ringbuf[debug_rx.rxbuf_wr_idx++] = (uint8_t)(huart->Instance->DR);
        return;
      }
      if((isrflags & USART_SR_IDLE) != RESET){
        dmarequest = (uint8_t)(huart->Instance->SR);
        dmarequest = (uint8_t)(huart->Instance->DR);
        DebugUart_FrameDone();
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
              debug_rx.ringbuf[debug_rx.rxbuf_wr_idx++] = (uint8_t)(huart->Instance->DR);
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

void DebugUart_PollBuf(void)
{
    process_poll(&DebugUart_Handler);
}

/*
static int DebugUart_RxBufItemCount(void)
{
    return ((debug_rx.item_wr_idx - debug_rx.item_rd_idx)%DEBUG_UART_RX_BUFFER_CACHE);
}
*/
static int DebugUart_RxBufLen(uint8_t index){
	return ((debug_rx.item_idx_range[index].end - debug_rx.item_idx_range[index].start)%DEBUG_UART_RX_BUFFER_SIZE)+1;
}

static void DebugUart_RxBufRead(uint8_t * data, uint8_t len){
	for(uint8_t i = 0; i < len; i++){
		data[i] = debug_rx.ringbuf[debug_rx.item_idx_range[debug_rx.item_rd_idx].start++];
	}
    debug_rx.item_rd_idx = (debug_rx.item_rd_idx+1)%DEBUG_UART_RX_BUFFER_CACHE;
}
int DebugUart_Printf(char *fmt, ...)
{
    int retval;
    va_list vp;
    char logmsg_buf[DEBUG_UART_RX_BUFFER_SIZE]={0};

    va_start(vp, fmt);
    retval = vsnprintf(logmsg_buf, DEBUG_UART_RX_BUFFER_SIZE, fmt, vp);
    va_end(vp);

    HAL_UART_Transmit(&debug_uart, (uint8_t*)logmsg_buf, retval, 50);

    return retval;
}
int DebugUart_Transmit(uint8_t * buf, uint16_t len)
{
    return (int)HAL_UART_Transmit(&debug_uart, buf, len, 50);
}


PROCESS_THREAD(DebugUart_Handler, ev, data)
{
    /* this code will execute for every event */
    PROCESS_BEGIN();
    while(1)
    {
        PROCESS_YIELD();
        if(ev == PROCESS_EVENT_POLL )
        {
            do{
                uint8_t data[DEBUG_UART_RX_BUFFER_SIZE] = {0};
                uint8_t len = DebugUart_RxBufLen(debug_rx.item_rd_idx);
                DebugUart_RxBufRead(data,len);
                if(len == 0) {
                    if(debug_rx.item_rd_idx != debug_rx.item_wr_idx)
                        continue;
                    else
                        break;
                }
                Shell_rec_buf((char*)data,len);
                printf("%s",data);
            }while(debug_rx.item_rd_idx != debug_rx.item_wr_idx);
        }
    }
    PROCESS_END();
}


