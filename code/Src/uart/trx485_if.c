#include <stdio.h>
#include <string.h>
#include "trx485_if.h"
#include "stm32f1xx_hal.h"
#include "uart_485_drv.h"
#include "unit.h"
#include "gl_manager.h"
#include "global.h"
#include "spk.h"
#include "rom_info.h"
#include "distance_manager.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "hlog.h"

#define MAX_RESPONSE_BUF_LEN            50
static uint8_t response_buf[MAX_RESPONSE_BUF_LEN] = {0};
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
    Gl_LockOn();
    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_LOCK_ON, NULL, 0);
}
int TRx485_LockOff(void)
{
    //todo lock off
    printf("%s\n",__func__);
    Gl_LockOff();
    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_LOCK_OFF, NULL, 0);
}
int TRx485_GetLockStatus(void)
{
    //todo lock on
    uint8_t status = Gl_GetLockState();
    printf("%s:%#x\n",__func__, status);

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_LOCK_STATUS, &status, sizeof(status));
}
int TRx485_SetUltrasoundPeriod(uint8_t period_s)
{
    //todo lock on
    printf("%s %d\n",__func__, period_s);
    Dist_SetCSBTIME(period_s);

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_ULTRASUOUND_PERIOD, NULL, 0);
}
int TRx485_GetUltrasoundPeriod(void)
{
    //todo lock on
    printf("%s\n",__func__);
    uint8_t period_s = Dist_GetCSBTIME();

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_ULTRASUOUND_PERIOD, &period_s, sizeof(uint8_t));
}
int TRx485_SetUltrasoundTimer(uint8_t filter_time_s)
{
    //todo lock on
    printf("%s %d\n",__func__, filter_time_s);
    Dist_SetCSB_NUM_MAX(filter_time_s);

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_ULTRASUOUND_TIMER, NULL, 0);
}
int TRx485_GetUltrasoundTimer(void)
{
    //todo lock on
    printf("%s\n",__func__);
    uint8_t filter_time = Dist_GetCSB_NUM_MAX();

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_ULTRASUOUND_TIMER, &filter_time, sizeof(uint8_t));
}
int TRx485_GetUltrasoundPars(void)
{
    //todo lock on
    uint8_t pars[2] = {0};
    printf("%s\n",__func__);
    pars[0] = Dist_GetCSBTIME();
    pars[1] = Dist_GetCSB_NUM();
    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_ULTRASUOUND_PARS, pars, sizeof(pars));
}
int TRx485_Buzzer(uint8_t pars)
{
    //todo lock on
    uint8_t data[2] = {0};
    printf("%s\n",__func__);
    switch(pars){
        case SPK_SWITCH_ON:
        case SPK_SWITCH_OFF:
            spk_switch = pars;
            return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_BUZZER, &pars, 1);
        case SPK_SWITCH_READ:
            data[0] = pars;
            data[1] = spk_switch;
            return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_BUZZER, data, 2);
    }
    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_BUZZER, NULL, 0);

}
int TRx485_GetVersion(void)
{
    //todo lock on
    uint8_t version[2] = {0};
    printf("%s\n",__func__);
    version[0] = atoi(&ROM_version[6]);
    version[1] = atoi(HW_VERSION_STR);

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_VERSION, version, 2);
}
int TRx485_SetUltrasoundOnOff(uint8_t pars)
{
    //todo lock on
    uint8_t data[2] = {0};
    printf("%s\n",__func__);
    switch(pars){
        case ULT_SWITCH_ON:
            Dist_StartMesure();
            return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_ULTRASUOUND_ON_OFF, &pars, 1);
        case ULT_SWITCH_OFF:
            Dist_StopMesure();
            return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_ULTRASUOUND_ON_OFF, &pars, 1);
        case ULT_SWITCH_READ:
            data[0] = pars;
            data[1] = Dist_GetMesureState();
            return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_ULTRASUOUND_ON_OFF, data, 2);
    }
    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_ULTRASUOUND_ON_OFF, NULL, 0);
}
int TRx485_GetCarStatus(void)
{
    //todo lock on
    printf("%s\n",__func__);

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_CAR_STATUS, NULL, 0);
}
int TRx485_SetAddr(uint8_t addr)
{
    //todo lock on
    uint8_t status = 0;
    printf("%s\n",__func__);
    local_addr = addr;
    setLocalAddr(local_addr);

    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_ADDR, &status, 1);
}
int TRx485_GetAddr(void)
{
    //todo lock on
    printf("%s\n",__func__);
    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_GET_ADDR, &local_addr, sizeof(local_addr));
}
int TRx485_SetBaudRate(uint8_t BAUDRATE)
{
    //todo lock on
    uint8_t status = 0;
    printf("%s\n",__func__);
    switch(BAUDRATE){
        case BAUDRATE_9600:
            setBaudRate(9600);
            break;
        case BAUDRATE_4800:
            setBaudRate(4800);
            break;
        case BAUDRATE_2400:
            setBaudRate(2400);
            break;
        case BAUDRATE_1200:
            setBaudRate(1200);
            break;
        case BAUDRATE_600:
            setBaudRate(600);
            break;
    }
    return cmd_respond(TX_CMD_RESPONSE_NORMAL, RX_CMD_SET_BAUDRATE, &status, 1);
}
int TRx485_ParsePacket(uint8_t *data, uint8_t length)
{
    int ret;
    Frame_t frame_rx;
    if(length < 6){
        loge("packet length error\n");
        return -1;
    }
    frame_rx.head = data[0];
    frame_rx.addr = data[1];
    frame_rx.data_len = data[2];
    frame_rx.cmd = data[3];
    frame_rx.data = &data[4];
    frame_rx.crc = data[length-2];
    frame_rx.tail = data[length-1];

    if(frame_rx.addr != local_addr){
        logi("addr not match, local_addr:%#x, packe_addr:%#x\n", local_addr, frame_rx.addr);
        return -1;
    }
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
            if(frame_rx.data_len >= 2)
                TRx485_SetUltrasoundPeriod(frame_rx.data[0]);
        break;
        case RX_CMD_GET_ULTRASUOUND_PERIOD:
            TRx485_GetUltrasoundPeriod();
        break;
        case RX_CMD_SET_ULTRASUOUND_TIMER:
            if(frame_rx.data_len >= 2)
                TRx485_SetUltrasoundTimer(frame_rx.data[0]);
        break;
        case RX_CMD_GET_ULTRASUOUND_TIMER:
            TRx485_GetUltrasoundTimer();
        break;
        case RX_CMD_GET_ULTRASUOUND_PARS:
            TRx485_GetUltrasoundPars();
        break;
        case RX_CMD_BUZZER:
            if(frame_rx.data_len >= 2)
                TRx485_Buzzer(frame_rx.data[0]);
        break;
        case RX_CMD_GET_VERSION:
            TRx485_GetVersion();
        break;
        case RX_CMD_SET_ULTRASUOUND_ON_OFF:
            if(frame_rx.data_len >= 2)
                TRx485_SetUltrasoundOnOff(frame_rx.data[0]);
        break;
        case RX_CMD_GET_CAR_STATUS:
            TRx485_GetCarStatus();
        break;
        case RX_CMD_SET_ADDR:
            if(frame_rx.data_len >= 2)
                TRx485_SetAddr(frame_rx.data[0]);
        break;
        case RX_CMD_GET_ADDR:
            if(frame_rx.data_len >= 2)
                TRx485_GetAddr();
        break;
        case RX_CMD_SET_BAUDRATE:
            if(frame_rx.data_len >= 2)
                TRx485_SetBaudRate(frame_rx.data[0]);
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
