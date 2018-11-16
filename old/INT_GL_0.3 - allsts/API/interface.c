#include "IO.h"
#include "global.h"
#include "delay.h"
#include "usart.h"

/*定义地锁专用buf*/
u8 to_lock_rbuf[10];


/* Table of CRC values for Corder byte */
static u8 CRC8_TAB[256] =
{
0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5,
0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6,
0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f,
0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c,
0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1,
0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a,
0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
} ;
u8 CRC8(u8 *puchMsg, u16 usDataLen)
{
  u8   uchCRC8 = 0x00;                         /* byte of CRC8 initialized */
  u8   uIndex;                                 /* will index into CRC lookup table */

  while(usDataLen--)                                /* pass through message buffer */
  {
    uIndex  = uchCRC8 ^ *puchMsg++;                 /* calculate the CRC */
    uchCRC8 = CRC8_TAB[uIndex];
  }
  return  uchCRC8;
}
void To_Lock_send (USART_TypeDef* u, To_Lock_Frame *f)
{
  u8   *bp = &f->head;
  u16  len = f->length;
  u8   crc;
  
  /* 计算CRC,校验范围:长度+命令+数据 */
  crc = CRC8 (&bp[2], (f->data_len + 1));

  bp[len++] = crc;
  bp[len++] = 0xAA;

  UartSend(u,len,bp);
}
void To_Lock_ON (void)
{
  To_Lock_Frame  *frm;
  
  frm = (To_Lock_Frame  *)to_lock_rbuf;

  frm->head     = 0x5A;
  frm->addr     = 0x00;
  frm->data_len = 0x01;
  frm->cmd      = TO_LOCK_CMD_ON; 

  frm->length   = 4;

  To_Lock_send (USART2, frm);
}
void To_Lock_OFF (void)
{
  To_Lock_Frame  *frm;
  
  frm = (To_Lock_Frame  *)to_lock_rbuf;

  frm->head     = 0x5A;
  frm->addr     = 0x00;
  frm->data_len = 0x01;
  frm->cmd      = TO_LOCK_CMD_OFF; 

  frm->length   = 4;

  To_Lock_send (USART2, frm);
}

void Get_To_Lock_Sts (void)
{
  To_Lock_Frame  *frm;
  
  frm = (To_Lock_Frame  *)to_lock_rbuf;

  frm->head     = 0x5A;
  frm->addr     = 0x00;
  frm->data_len = 0x02;
  frm->cmd      = TO_LOCK_CMD_READ_STS; 
  switch(sts.work_sts)
  {
    /*运行,初始化也算是运行*/
		case 0:
    case 1:
      frm->buf[0] = 3;
    break;
    /*升起*/
    case 2:
      frm->buf[0] = 1;
    break;
    /*下降*/
    case 3:
      frm->buf[0] = 2;
    break;
    /*上升遇阻*/
    case 4:
      frm->buf[0] = 4;
    break;
    /*下降遇阻*/
    case 5:
      frm->buf[0] = 5;
    break;
  }
	//故障表示 -- 故障只是在限位板故障的时候
	if(sts.err.limit_board_1b)
		frm->buf[0] = 6;
  frm->length   = 5;

  To_Lock_send (USART2, frm);
}
void Get_To_Car_Sts (void)
{
  To_Lock_Frame  *frm;
  
  frm = (To_Lock_Frame  *)to_lock_rbuf;

  frm->head     = 0x5A;
  frm->addr     = 0x00;
  frm->data_len = 0x02;
  frm->cmd      = TO_CAR_CMD_READ_STS; 
  switch(sts.car_radar)
  {
		case 0: //无车
      frm->buf[0] = 2;
    break;
    case 1: //有车
      frm->buf[0] = 1;
    break;
   
  }
	//车位未知
	if((sts.err.geo_1b && sts.err.radar_1b) || (sts.car_radar == 3))
		frm->buf[0] = 3;
  frm->length   = 5;

  To_Lock_send (USART2, frm);
}
void Get_To_Err_Sts (void)
{
  To_Lock_Frame  *frm;
  
  frm = (To_Lock_Frame  *)to_lock_rbuf;

  frm->head     = 0x5A;
  frm->addr     = 0x00;
  frm->data_len = 0x02;
  frm->cmd      = TO_CAR_CMD_READ_STS;
  frm->buf[0] = 0;
	if(sts.err.geo_1b)
    frm->buf[0] |= 0x04;
	if(sts.err.limit_board_1b)
    frm->buf[0] |= 0x01;
	if(sts.err.radar_1b)
    frm->buf[0] |= 0x08;
	if(sts.err.voice_1b)
    frm->buf[0] |= 0x02;
  frm->length   = 5;

  To_Lock_send (USART2, frm);
}
/*接口函数*/
//u8 test12 = 0;
void Interface()
{
  /*查询是否接收到新信息*/
  u8 crc;
	To_Lock_Frame  *frm;
	u8* bp;
	
//	if(test12 == 1)
//		To_Lock_ON();
//	else if(test12 == 2)
//		To_Lock_OFF();
//	else if(test12 == 3)
//		Get_To_Lock_Sts();
//	test12 = 0;	
	
	
  if(uart[1].Sts == 1)
  {
    uart[1].Sts = 0;
    
    memcpy(to_lock_rbuf + 1,uart[1].buf,9);
    frm = (To_Lock_Frame  *)to_lock_rbuf;
    bp = (u8*)&frm->head;
    
    if(frm->head == 0x55)
    {
      crc  = bp[uart[1].length - 2];
      if(crc == CRC8 (&bp[2], uart[1].length - 4))
      {
        switch(frm->cmd)
        {
          case TO_LOCK_CMD_ON:
            To_Lock_ON();
            cmd.ctrl = 1;
            break;
          case TO_LOCK_CMD_OFF:
            To_Lock_OFF();
            cmd.ctrl = 2;
            break;
          case TO_LOCK_CMD_READ_STS:
            Get_To_Lock_Sts();
            break;
          case TO_CAR_CMD_READ_STS:
            Get_To_Car_Sts();
            break;
          case TO_ERR_CMD_READ_STS:
            Get_To_Err_Sts();
            break;
					
          default:break;
        }
      }
    }
    uart[1].length = 0;
  }
}









