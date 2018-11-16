#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
extern u8 YY_CMD[10][6];
                                  //�����趨���
#define VOICE_SET UartSend(USART3,5,YY_CMD[0]);
                                  //����1#����
#define VOICE_NO1 UartSend(USART3,6,YY_CMD[1]);
                                  //����2#����
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
  u16 length;           //���յ����ݳ���
  u8  Sts;              //����״̬  0:������  1:������
  u8  buf[USART_REC_LEN];           //������
}UART_Frame;

extern UART_Frame uart[3];

void uart_init(u32 bound);

void UartSend(USART_TypeDef* USARTx,u16 Length,u8* Data);

void USART3_init(u32 bound);

extern UART_Frame uart[3];

#endif


