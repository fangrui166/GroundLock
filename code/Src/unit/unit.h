#ifndef __UNIT_H__
#define __UNIT_H__
#include <stdint.h>

uint8_t CRC8(uint8_t *puchMsg, uint16_t usDataLen);
void buffer_dump(uint8_t *buf, int len);
void buffer_dump_decimal(uint8_t *buf, int len);
uint8_t buf_is_null(uint8_t * data, int len);
uint8_t buf_is_match(uint8_t * des, uint8_t * src, int len);
uint32_t get_le32(uint8_t *buf);
uint16_t get_le16(uint8_t *buf);

#endif
