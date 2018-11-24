#include <stdio.h>
#include <string.h>
#include "trx485_if.h"
#include "stm32f1xx_hal.h"
#include "uart_485_drv.h"
#include "unit.h"

#define MAX_RESPONSE_BUF_LEN            50
static uint8_t response_buf[MAX_RESPONSE_BUF_LEN] = {0};
static uint8_t local_addr = 0;
Frame_t frame_tx ={
    .head = TX_CMD_RESPONSE_NORMAL,
    .addr = DEFAULT_LOCAL_ADDR,
    .data_len = 0,
    .cmd = 0,
    .data = response_buf,
    .crc = 0,
    .tail = TRX_CMD_TAIL,
};

int TRx485_CtrlPinInit(void)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    return 0;
}
int TRx485_Init(void)
{
    TRx485_CtrlPinInit();

    return 0;
}

int cmd_respond(uint8_t response_t,uint8_t commandd, uint8_t * append, uint8_t append_len)
{
    int ret;
    uint8_t crc_buf[MAX_RESPONSE_BUF_LEN+2];
    uint8_t *trx485_tx = TRx485Get_TxBuf();

    frame_tx.head = response_t;
    frame_tx.addr = local_addr;
    frame_tx.cmd = commandd;
    frame_tx.data_len = append_len+1;
    memcpy(frame_tx.data, append, append_len);

    crc_buf[0] = frame_tx.cmd;
    crc_buf[1] = frame_tx.data_len;
    memcpy(&crc_buf[2], append, append_len);
    frame_tx.crc = CRC8(crc_buf, append_len+2);

    frame_tx.tail = TRX_CMD_TAIL;

    trx485_tx[0] = frame_tx.head;
    trx485_tx[1] = frame_tx.addr;
    trx485_tx[2] = frame_tx.data_len;
    trx485_tx[3] = frame_tx.cmd;
    memcpy(&trx485_tx[4], frame_tx.data, frame_tx.data_len-1);
    trx485_tx[frame_tx.data_len+4] = frame_tx.crc;
    trx485_tx[frame_tx.data_len+5] = frame_tx.tail;

    CTR485_HIGHT();
    ret = TRx485Uart_Transmit(trx485_tx, frame_tx.data_len+6);
    CTR485_LOW();

    return ret;
}
int TRx485_LockOn(void)
{
    //todo lock on
    printf("%s\n",__func__);
    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_LOCK_ON, NULL, 0);
}
int TRx485_LockOff(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_LOCK_OFF, NULL, 0);
}
int TRx485_GetLockStatus(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_LOCK_STATUS, NULL, 0);
}
int TRx485_SetUltrasoundPeriod(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_ULTRASUOUND_PERIOD, NULL, 0);
}
int TRx485_GetUltrasoundPeriod(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_ULTRASUOUND_PERIOD, NULL, 0);
}
int TRx485_SetUltrasoundTimer(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_ULTRASUOUND_TIMER, NULL, 0);
}
int TRx485_GetUltrasoundTimer(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_ULTRASUOUND_TIMER, NULL, 0);
}
int TRx485_GetUltrasoundPars(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_ULTRASUOUND_PARS, NULL, 0);
}
int TRx485_Buzzer(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_BUZZER, NULL, 0);
}
int TRx485_GetVersion(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_VERSION, NULL, 0);
}
int TRx485_SetUltrasoundOnOff(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_ULTRASUOUND_ON_OFF, NULL, 0);
}
int TRx485_GetCarStatus(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_CAR_STATUS, NULL, 0);
}
int TRx485_SetAddr(void)
{
    //todo lock on


    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_ADDR, NULL, 0);
}
int TRx485_GetAddr(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_ADDR, NULL, 0);
}
int TRx485_SetBaudRate(void)
{
    //todo lock on

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_BAUDRATE, NULL, 0);
}
int TRx485_ParsePacket(uint8_t *data, uint8_t length)
{
    int ret;
    Frame_t frame_rx;
    frame_rx.head = data[0];
    frame_rx.addr = data[1];
    frame_rx.data_len = data[2];
    frame_rx.cmd = data[3];
    frame_rx.data = &data[4];
    frame_rx.crc = data[length-2];
    frame_rx.tail = data[length-1];

    if((frame_rx.head != RX_CMD_NORMAL) && (frame_rx.tail != TRX_CMD_TAIL)){
        printf("rx frame error, head:%#x, tail:%#x\n", frame_rx.head, frame_rx.tail);
        ret =  -CMD_FORMAT_ERROR;
        goto error;
    }
    if((length - 5) != frame_rx.data_len){
        printf("rx frame length unmatch, length:%d, data_len:%d\n", length, frame_rx.data_len);
        ret =  -CMD_LENGTH_ERROR;
        goto error;
    }
    if(frame_rx.crc != CRC8(&data[2], length-4)){
        printf("rx frame crc error\n");
        ret =  -CMD_CRC_ERROR;
        goto error;
    }

    switch(frame_rx.cmd){
        case RX_CMD_LOCK_ON:
            TRx485_LockOn();
        break;
        case RX_CMD_LOCK_OFF:
            TRx485_LockOff();
        break;
        case RX_CMD_GET_LOCK_STATUS:
            TRx485_GetLockStatus();
        break;
        case RX_CMD_SET_ULTRASUOUND_PERIOD:
            TRx485_SetUltrasoundPeriod();
        break;
        case RX_CMD_GET_ULTRASUOUND_PERIOD:
            TRx485_GetUltrasoundPeriod();
        break;
        case RX_CMD_SET_ULTRASUOUND_TIMER:
            TRx485_SetUltrasoundTimer();
        break;
        case RX_CMD_GET_ULTRASUOUND_TIMER:
            TRx485_GetUltrasoundTimer();
        break;
        case RX_CMD_GET_ULTRASUOUND_PARS:
            TRx485_GetUltrasoundPars();
        break;
        case RX_CMD_BUZZER:
            TRx485_Buzzer();
        break;
        case RX_CMD_GET_VERSION:
            TRx485_GetVersion();
        break;
        case RX_CMD_SET_ULTRASUOUND_ON_OFF:
            TRx485_SetUltrasoundOnOff();
        break;
        case RX_CMD_GET_CAR_STATUS:
            TRx485_GetCarStatus();
        break;
        case RX_CMD_SET_ADDR:
            TRx485_SetAddr();
        break;
        case RX_CMD_GET_ADDR:
            TRx485_GetAddr();
        break;
        case RX_CMD_SET_BAUDRATE:
            TRx485_SetBaudRate();
        break;
        default:
            goto error;
        break;
    }
    return 0;
error:
    frame_rx.data[0] = 0;
    cmd_respond(TX_CMD_RESPONSE_ERROR, frame_rx.cmd, frame_rx.data, 1);
    return ret;
}
