#ifndef __INTERFACE_H
#define __INTERFACE_H	 
#include "sys.h"

#define TO_LOCK_CMD_OFF       0x01
#define TO_LOCK_CMD_ON        0x02
#define TO_LOCK_CMD_READ_STS  0x06

typedef struct
{
  u8 length;   //���峤��
  u8 head;     //��ͷ:0x55/0x5A/0x5B
  u8 addr;     //��ַ
  u8 data_len; //����
  u8 cmd;      //��������  
  u8 buf[1];   //��������
} To_Lock_Frame;

void Interface(void);
#endif





