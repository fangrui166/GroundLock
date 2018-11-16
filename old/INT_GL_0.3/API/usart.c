#include "sys.h"
#include "usart.h"	  
#include "delay.h"	  
#include "global.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
u8 YY_CMD[10][6] = {{0x7e,0x03,0x31,0x1e,0xef},   //AA 02 31 EF 
									  {0x7e,0x04,0x41,0x00,0x01,0xef},//AA 02 41 EF    AA 02 80 EF 
									  {0x7e,0x04,0x41,0x00,0x02,0xef},//AA 02 41 EF    AA 02 80 EF 
									  {0x7e,0x04,0x41,0x00,0x03,0xef},//AA 02 41 EF    AA 02 80 EF 
									  {0x7e,0x04,0x41,0x00,0x04,0xef},
									  {0x7e,0x04,0x41,0x00,0x05,0xef},
									  {0x7e,0x04,0x41,0x00,0x06,0xef},
									  {0x7e,0x04,0x41,0x00,0x07,0xef},
									  {0x7e,0x04,0x41,0x00,0x08,0xef},
									  {0x7e,0x04,0x41,0x00,0x09,0xef}
                   };
void uart_init(u32 bound)
{
	//UART 1
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//����������� 
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9 
   
  //USART1_RX	  GPIOA.10��ʼ�� 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//�������� 
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10   

  //Usart1 NVIC ���� 
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//��ռ���ȼ�3 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ�� 
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
  
   //USART ��ʼ������  

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����  
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ  
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ  
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ  
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������ 
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ 

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1 
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1  
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�򿪿����ж�
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж� 
  USART_ITConfig(USART1, USART_IT_PE, ENABLE);  //����ż�����ж�
  USART_ITConfig(USART1, USART_IT_ERR, ENABLE); //�򿪴����ж� 
	//UART 2
  //GPIO�˿�����

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//����������� 
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA2 
   
  //USART1_RX	  GPIOA.3��ʼ�� 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//�������� 
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA3   

  //Usart2 NVIC ���� 
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//��ռ���ȼ�3 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ�� 
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
  
   //USART ��ʼ������  

	USART_InitStructure.USART_BaudRate = 9600;//���ڲ�����  
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ  
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ  
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ  
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������ 
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ 

  USART_Init(USART2, &USART_InitStructure); //��ʼ������1 
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1  
  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//�򿪿����ж�
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж� 
  USART_ITConfig(USART2, USART_IT_PE, ENABLE);  //����ż�����ж�
  USART_ITConfig(USART2, USART_IT_ERR, ENABLE); //�򿪴����ж� 
	//UART 3
  //GPIO�˿�����
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	
	//USART3_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//����������� 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //USART3_RX	  GPIOB.11��ʼ�� 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//�������� 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //Usart3 NVIC ���� 
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ�� 
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
   //USART ��ʼ������  
	USART_InitStructure.USART_BaudRate = 9600;//���ڲ�����  
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ  
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ  
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ  
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������ 
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ 

  USART_Init(USART3, &USART_InitStructure); //��ʼ������3 
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���1  
  USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//�򿪿����ж�
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж� 
  USART_ITConfig(USART3, USART_IT_PE, ENABLE);  //����ż�����ж�
  USART_ITConfig(USART3, USART_IT_ERR, ENABLE); //�򿪴����ж� 
}

UART_Frame uart[3];


void USART1_IRQHandler(void)                	
{
	uint8_t clear = clear;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		uart[0].buf[uart[0].length ++] = USART1->DR; 
  } 
	else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		clear = USART1->SR;
    clear = USART1->DR;
		uart[0].Sts = 1;
	}
	else if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET)  
	{
		USART_ClearITPendingBit(USART1, USART_IT_ORE);
  } 
	else if(USART_GetITStatus(USART1, USART_IT_NE) != RESET)  
	{
		USART_ClearITPendingBit(USART1, USART_IT_NE);
  } 
	else if(USART_GetITStatus(USART1, USART_IT_FE) != RESET)  
	{
		USART_ClearITPendingBit(USART1, USART_IT_FE);
  } 
	else if(USART_GetITStatus(USART1, USART_IT_PE) != RESET)  
	{
		USART_ClearITPendingBit(USART1, USART_IT_PE);
  } 
}
void USART2_IRQHandler(void)                	
{
	uint8_t clear = clear;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  
	{
		uart[1].buf[uart[1].length ++] = USART2->DR; 
  } 
	else if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		clear = USART2->SR;
    clear = USART2->DR;
    
		uart[1].Sts = 1;
	}
	else if(USART_GetITStatus(USART2, USART_IT_ORE) != RESET)  
	{
		USART_ClearITPendingBit(USART2, USART_IT_ORE);
  } 
	else if(USART_GetITStatus(USART2, USART_IT_NE) != RESET)  
	{
		USART_ClearITPendingBit(USART2, USART_IT_NE);
  } 
	else if(USART_GetITStatus(USART2, USART_IT_FE) != RESET)  
	{
		USART_ClearITPendingBit(USART2, USART_IT_FE);
  } 
	else if(USART_GetITStatus(USART2, USART_IT_PE) != RESET)  
	{
		USART_ClearITPendingBit(USART2, USART_IT_PE);
  } 
}

void USART3_IRQHandler(void)                	
{
	uint8_t clear = clear;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{
		uart[2].buf[uart[2].length ++] = USART3->DR; 
  } 
	else if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		//������ж�
		clear = USART3->SR;
    clear = USART3->DR;
		uart[2].Sts = 1;
	}
	else if(USART_GetITStatus(USART3, USART_IT_ORE) != RESET)  
	{
		USART_ClearITPendingBit(USART3, USART_IT_ORE);
  } 
	else if(USART_GetITStatus(USART3, USART_IT_NE) != RESET)  
	{
		USART_ClearITPendingBit(USART3, USART_IT_NE);
  } 
	else if(USART_GetITStatus(USART3, USART_IT_FE) != RESET)  
	{
		USART_ClearITPendingBit(USART3, USART_IT_FE);
  } 
	else if(USART_GetITStatus(USART3, USART_IT_PE) != RESET)  
	{
		USART_ClearITPendingBit(USART3, USART_IT_PE);
  } 
}

void UartSend(USART_TypeDef* USARTx,u16 Length,u8* Data)
{
	u16 i = 0;
	if(Length == 0)
		return;
	if(USARTx == USART2)
	{
		CTR485 = 1;
	}
	for(i = 0;i < Length;i ++)
	{
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
		USART_SendData(USARTx,Data[i]);	
		//delay_ms(1);
	}
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	if(USARTx == USART2)
	{
		CTR485 = 0;	
	}
}


void UartReceive(USART_TypeDef* USARTx,u16* Buff)
{
	while(USART_ReceiveData(USARTx))
	{
		
	}
}

