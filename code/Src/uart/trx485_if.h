#ifndef __TRX485_IF_H__
#define __TRX485_IF_H__
#include <stdint.h>

#define RX_CMD_NORMAL               0x55
#define TX_CMD_RESPONSE_NORMAL      0x5A
#define TX_CMD_RESPONSE_ERROR       0x5B
#define TRX_CMD_TAIL                0xAA

#define DEFAULT_LOCAL_ADDR          0x00

#define RX_CMD_LOCK_ON                      0x01
#define RX_CMD_LOCK_OFF                     0x02
#define RX_CMD_GET_LOCK_STATUS              0x06
#define RX_CMD_SET_ULTRASUOUND_PERIOD       0x07
#define RX_CMD_GET_ULTRASUOUND_PERIOD       0x08
#define RX_CMD_SET_ULTRASUOUND_TIMER        0x09
#define RX_CMD_GET_ULTRASUOUND_TIMER        0x0A
#define RX_CMD_GET_ULTRASUOUND_PARS         0x14
#define RX_CMD_BUZZER                       0x15
#define RX_CMD_GET_VERSION                  0x1A
#define RX_CMD_SET_ULTRASUOUND_ON_OFF       0x1B
#define RX_CMD_GET_CAR_STATUS                   0X00 // ???
#define RX_CMD_SET_ADDR                     0x1C
#define RX_CMD_GET_ADDR                     0x1D
#define RX_CMD_SET_BAUDRATE                 0x1E

#define CTR485_HIGHT()         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET)
#define CTR485_LOW()           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET)
typedef struct{
    uint8_t head;
    uint8_t addr;
    uint8_t data_len;
    uint8_t cmd;
    uint8_t *data;
    uint8_t crc;
    uint8_t tail;
}Frame_t;

typedef enum{
    CMD_NO_ERROR,
    CMD_FORMAT_ERROR,
    CMD_LENGTH_ERROR,
    CMD_CRC_ERROR,
}cmd_error_t;

int TRx485_Init(void);
int TRx485_ParsePacket(uint8_t *data, uint8_t length);
#endif
