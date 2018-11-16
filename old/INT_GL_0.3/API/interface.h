#ifndef __INTERFACE_H
#define __INTERFACE_H	 
#include "sys.h"

#define TO_LOCK_CMD_OFF       0x01
#define TO_LOCK_CMD_ON        0x02
#define TO_LOCK_CMD_READ_STS  0x06

typedef struct
{
  u8 length;   //整体长度
  u8 head;     //数头:0x55/0x5A/0x5B
  u8 addr;     //地址
  u8 data_len; //长度
  u8 cmd;      //控制命令  
  u8 buf[1];   //数据内容
} To_Lock_Frame;

void Interface(void);
#endif





