#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
extern u8 YY_CMD[10][6];
                                  //声音设定最大
#define VOICE_SET UartSend(USART3,5,YY_CMD[0]);
                                  //播放1#声音
#define VOICE_NO1 UartSend(USART3,6,YY_CMD[1]);
                                  //播放2#声音
#define VOICE_NO2 UartSend(USART3,6,YY_CMD[2]); 
#define VOICE_NO3 UartSend(USART3,6,YY_CMD[3]);
#define VOICE_NO4 UartSend(USART3,6,YY_CMD[4]); 
#define VOICE_NO5 UartSend(USART3,6,YY_CMD[5]); 
#define VOICE_NO6 UartSend(USART3,6,YY_CMD[6]);
#define VOICE_NO7 UartSend(USART3,6,YY_CMD[7]);
#define VOICE_NO8 UartSend(USART3,6,YY_CMD[8]);
#define VOICE_NO9 UartSend(USART3,6,YY_CMD[9]);

typedef struct
{
  u16 length;           //接收的数据长度
  u8  Sts;              //接收状态  0:无数据  1:有数据
  u8  buf[USART_REC_LEN];           //数据域
}UART_Frame;

extern UART_Frame uart[3];

void uart_init(u32 bound);

void UartSend(USART_TypeDef* USARTx,u16 Length,u8* Data);

void USART3_init(u32 bound);

extern UART_Frame uart[3];

#endif


