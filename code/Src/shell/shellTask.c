#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <contiki.h>
#include "stm32f1xx_hal.h"
#include "shellTask.h"
#include "trx485_if.h"
#include "gl_manager.h"
#include "adc.h"
#include "spk.h"
#include "hlog.h"
#include "misc_data_ro.h"
#include "misc_data_rw.h"
#include "power.h"
#include "pwm.h"
#include "misc_data_ro.h"
#include "distance_manager.h"

PROCESS(shell_process, "shell");

typedef struct{
    uint32_t lenght;
    char buf[CMD_BUF_LEN];
}CMDRxBuf;
static CMDRxBuf CMDCacheBuf[CMD_CACHE_LEN] = {0};
static uint8_t CMD_Cache_Head, CMD_Cache_Tail;

const unsigned char whiteSpace[] = {' ', '\t', '=', '\r', '\n'};
static process_event_t ev_serial_ch;

__weak int get_task_state(int argc, char *argv[], _command_source source)
{
    int i;

    for (i = 0; i < argc; i++)
    {
        printf("argv[%d]:%s\r\n",i,(char const*)argv[i]);
    }

    return 0;
}

int uart485_test(int argc, char *argv[], _command_source source)
{
    TRx485_LockOn();
    return 0;
}
int spk_test(int argc, char *argv[], _command_source source)
{
    if(argc == 2){
        int time = atoi(argv[1]);
        SPK_Start(time);
    }
    return 0;
}
int lock_test(int argc, char *argv[], _command_source source)
{
    if(argc == 2){
        if(!strncmp(argv[1], "on", 2)){
            Gl_LockOn();
        }
        else if(!strncmp(argv[1], "off", 3)){
            Gl_LockOff();
        }
        else if(!strncmp(argv[1], "get", 3)){
            Lock_location location = Gl_GetLocation();
            logi("location:%d.\n",location);
        }
    }
    return 0;
}
int misc_test(int argc, char *argv[], _command_source source)
{
    int ret = 0;
    if(argc == 3){
        if(!strncmp(argv[1], "get", 3)){
            if(!strncasecmp(argv[2], "BaudRate", 8)){
                uint32_t baudrate;
                ret = getBaudRate(&baudrate);
                logi("BaudRate:%d\n", baudrate);
            }
            else if(!strncasecmp(argv[2], "addr", 4)){
                uint8_t local_addr;
                ret = getLocalAddr(&local_addr);
                logi("local_addr:%d\n", local_addr);
            }
            else if(!strncasecmp(argv[2], "CSB_TIME", 8)){
                uint32_t csb_time;
                ret = getCSBTIME(&csb_time);
                logi("csb_time:%d\n", csb_time);
            }
            else if(!strncasecmp(argv[2], "CSB_NUM", 7)){
                uint32_t csb_num;
                ret = getCSB_NUM_MAX(&csb_num);
                logi("csb_num:%d\n", csb_num);
            }
        }
    }
    else if(argc == 4){
        if(!strncmp(argv[1], "set", 3)){
            if(!strncasecmp(argv[2], "BaudRate", 8)){
                uint32_t baudrate = atoi(argv[3]);
                logi("setBaudRate:%d\n", baudrate);
                ret = setBaudRate(baudrate);
            }
            else if(!strncasecmp(argv[2], "addr", 4)){
                uint8_t local_addr = atoi(argv[3]);
                logi("setLocalAddr:%d\n", local_addr);
                ret = setLocalAddr(local_addr);
            }
            else if(!strncasecmp(argv[2], "CSB_TIME", 8)){
                uint32_t csb_time = atoi(argv[3]);
                logi("setCSBTIME:%d\n", csb_time);
                ret = setCSBTIME(csb_time);
            }
            else if(!strncasecmp(argv[2], "CSB_NUM", 7)){
                uint32_t csb_num = atoi(argv[3]);
                logi("setCSB_NUM_MAX:%d\n", csb_num);
                ret = setCSB_NUM_MAX(csb_num);
            }
        }
    }
    return ret;
}

int power_test(int argc, char *argv[], _command_source source)
{
    if(argc == 2){
        if(!strncmp(argv[1], "reboot", 6)){
            PWRMGR_SYSTEM_POWER_RESET();
        }
    }
    return 0;
}
int pwm_test(int argc, char *argv[], _command_source source)
{
    if(argc == 3){
        uint32_t  pwm_count = atoi(argv[2]);
        if(!strncmp(argv[1], "a", 1)){
            PWM_ChannelAStart(pwm_count);
        }
        else if(!strncmp(argv[1], "b", 1)){
            PWM_ChannelBStart(pwm_count);
        }
    }
    else if(argc == 2){
        if(!strncmp(argv[1], "start", 5)){
            Dist_StartMesure();
        }
        else if(!strncmp(argv[1], "stop", 4)){
            Dist_StopMesure();
        }
    }
    return 0;
}
int current_test(int argc, char *argv[], _command_source source)
{
    if(argc == 4){
        if(!strncmp(argv[1], "set", 3)){
            int  value = atoi(argv[3]);
            current_limited_t type;
            if(!strncmp(argv[2], "upr", 3)){
                type = UP_RESISTANCE;
            }
            else if(!strncmp(argv[2], "upd", 3)){
                type = UP_GIG_DAMP;
            }
            else if(!strncmp(argv[2], "downr", 5)){
                type = DOWN_RESISTANCE;
            }
            else if(!strncmp(argv[2], "downd", 5)){
                type = DOWN_GIG_DAMP;
            }
            Gl_UpdateLimitedCurrent((uint16_t)value, type);
        }
    }
    else if(argc == 3){
        if(!strncmp(argv[1], "get", 3)){
            current_limited_t type;
            if(!strncmp(argv[2], "upr", 3)){
                type = UP_RESISTANCE;
            }
            else if(!strncmp(argv[2], "upd", 3)){
                type = UP_GIG_DAMP;
            }
            else if(!strncmp(argv[2], "downr", 5)){
                type = DOWN_RESISTANCE;
            }
            else if(!strncmp(argv[2], "downd", 5)){
                type = DOWN_GIG_DAMP;
            }
            uint16_t stroe_value;
            uint16_t use_value;
            getLimitedCurrent(&stroe_value, type);
            Gl_GetLimitedCurrent(&use_value, type);
            logi("limited stroe:%d, use:%d\n", stroe_value, use_value);
        }
    }

    return 0;
}
int cmd_help(int argc, char * argv[], _command_source source);
const MONITOR_COMMAND commandTable[] =
{
    {"task",    get_task_state},
    {"485",     uart485_test},
    {"spk",     spk_test},
    {"lock",    lock_test},
    {"misc",    misc_test},
    {"pm",      power_test},
    {"pwm",     pwm_test},
    {"cur",     current_test},
    {"?",       cmd_help}, //This must be the last command
};
const unsigned long ulNumberOfCommands = (sizeof(commandTable) / sizeof(commandTable[0]));

int cmd_help(int argc, char * argv[], _command_source source)
{
    uint8_t i;

    printf("\tAll MFG Command list\r\n");

    for (i=0; i<(ulNumberOfCommands-1); i++){
        printf("\t%s\r\n", commandTable[i].command);
    }
    printf("\t________ ^_^ ________\r\n");
    return 0;
}

int isaspace(unsigned char c)
{
    int     i;

    for (i = 0; i < sizeof(whiteSpace); i++)
    {
        if (c == whiteSpace[i])
        {
            return 1;
        }
    }
    return 0;
}

int ParseCommandAndData(unsigned char *pLineBuf, int *argn,
                                    unsigned char *argv[], int MaxArgs)
{
    int             n;
    int             i;
    unsigned char   quoteChar;

    n = 0;
    while (n < MaxArgs)
    {
        while (isaspace(*pLineBuf))
        {
            pLineBuf++;
        }

        if (*pLineBuf == '"' || *pLineBuf == '\'')
        {
            quoteChar = *pLineBuf;
            *pLineBuf = (unsigned char)1;
            argv[n++] = pLineBuf++;
            while (*pLineBuf && (*pLineBuf != quoteChar))
            {
                pLineBuf++;
            }
            if (*pLineBuf)
            {
                *pLineBuf = 0;
                pLineBuf++;
            }
            else
            {
                n = 0;                     // Error, no matching quote char
                break;
            }
        }
        else if (*pLineBuf)
        {
            argv[n++] = pLineBuf;
            //
            // Go to the next whiteSpace
            //
            while (*pLineBuf && !isaspace(*pLineBuf))
            {
                pLineBuf++;
            }
            if (*pLineBuf)
            {
                *pLineBuf = 0;
                pLineBuf++;
            }
            else break;
        }
        else break;
    }

    if ((n >= 1) && *argv[0] == '?' && *(argv[0] + 1))
    {
        n++;
        if (n <= MaxArgs)
        {
            for (i = 1; i < n; i++)
            {
                argv[i] = argv[i - 1];
            }
            (argv[1])++;
            argv[0] = (unsigned char*)"?";
        }
    }
    if (n > MaxArgs)
    {
        printf("Too many arguments\n");
        n = 0;
    }
    *argn = n;
    return n;
}

/**
*use SecureCRT serial transceivers tools,in the character stream,may contain
*a character that is not needed , such as backspace,move around.Before use the
*command line tool to parse the character,we must delete the invalid character.
* supported character:
*   move up:1B 5B 41
*   move down:1B 5B 42
*   move right:1B 5B 43
*   move left:1B 5B 44
*   carriage retuen and line feed:0D 0A
*   Backspace:08
*/
static uint32_t filter_useless_string(char *dest,const char *src)
{
    uint32_t dest_count=0;
    uint32_t src_count=0;
    while(src[src_count]!=0x0D && src[src_count+1]!=0x0A){
        //if(isprint(src[src_count])){
        if(src[src_count] >= 0x20 && src[src_count] <= 0x7E){
            dest[dest_count++]=src[src_count++];
        }
        else{
            switch(src[src_count]){
                case    0x08:                          //Backspace
                {
                    if(dest_count>0){
                        dest_count --;
                    }
                    src_count ++;
                }
                break;
                case    0x1B:
                {
                    if(src[src_count+1]==0x5B){
                        if(src[src_count+2]==0x41 || src[src_count+2]==0x42){
                            src_count +=3;              //move up and down
                        }
                        else if(src[src_count+2]==0x43){
                            dest_count++;               //move right
                            src_count+=3;
                        }
                        else if(src[src_count+2]==0x44){
                            if(dest_count >0)           //move left
                            {
                                dest_count --;
                            }
                            src_count +=3;
                        }
                        else{
                            src_count +=3;
                        }
                    }
                    else{
                        src_count ++;
                    }
                }
                break;
                default:
                {
                    src_count++;
                }
                break;
            }
        }
    }

    dest[dest_count++]=src[src_count++];
    dest[dest_count++]=src[src_count++];

    return dest_count;
}

int DoCommand(int argn,unsigned char *argv[], _command_source source)
{
    unsigned int uiCount;

    //
    // The first argument should be the command
    //
    for (uiCount = 0; uiCount < ulNumberOfCommands; uiCount++){
        char resut_cmp = (strcmp((char const*)argv[0], (char const*)commandTable[uiCount].command)== 0);
        if ( resut_cmp ){
            return(*(commandTable[uiCount].pFunc))(argn, (char **)argv, source);
        }
    }
     printf("Command error !!!\n");

    return -1;
}

void Shell_parse_cmd(CMDRxBuf *cmd_buf)
{
    int argn = 0;
    CMDRxBuf cmd = {0};
    unsigned char *argv[MAX_ARGS] = {0};
    //printf("rec :%s\n", cmd_buf->buf);
    cmd.lenght = filter_useless_string(cmd.buf,cmd_buf->buf);
    if(cmd.lenght != 0){
        if(0 != ParseCommandAndData((unsigned char *)cmd.buf, &argn, argv,
                                    (sizeof(argv)/sizeof(argv[0])))){
            DoCommand(argn, argv, CMD_SOURCE_UART);
        }
        else{
            printf("Shell Command Task %s \r\n", "[No Processor for Command]");

        }
    }
    else{
        printf("[No Command]\n");
    }
}
void ShellRecvProcess(void)
{
    if(CMD_Cache_Tail != CMD_Cache_Head){
        CMDRxBuf CMDBuffer = {0};
        memcpy(&CMDBuffer,&CMDCacheBuf[CMD_Cache_Tail],
                    CMDCacheBuf[CMD_Cache_Tail].lenght+4);
        Shell_parse_cmd(&CMDBuffer);
        if(++CMD_Cache_Tail >= CMD_CACHE_LEN){
            CMD_Cache_Tail = 0;
        }
    }
}

void Shell_rec_buf(char *data, uint8_t length)
{
    static uint32_t rec_count=0;
    if(length >= CMD_BUF_LEN) return;
    if(rec_count+length >= CMD_BUF_LEN) rec_count = 0;
    memcpy(&CMDCacheBuf[CMD_Cache_Head].buf[rec_count], data, length);
    rec_count += length;
    if(/*(0x0D == CMDCacheBuf[CMD_Cache_Head].buf[rec_count-1]) &&*/ //support mtty in mfg mode
        (0x0D == CMDCacheBuf[CMD_Cache_Head].buf[rec_count-2])){
        CMDCacheBuf[CMD_Cache_Head].lenght = rec_count;
        rec_count = 0;
        if(++CMD_Cache_Head >= CMD_CACHE_LEN){
            CMD_Cache_Head = 0;
        }
    }
    else{
        if(rec_count >= CMD_BUF_LEN){
            rec_count = 0;
        }
    }
    process_post(&shell_process, ev_serial_ch, NULL);
}

void Shell_init(void)
{
    CMD_Cache_Head = 0;
    CMD_Cache_Tail = 0;
    ev_serial_ch = process_alloc_event();
    process_start(&shell_process,NULL);
}

PROCESS_THREAD(shell_process, ev, data)
{
  PROCESS_BEGIN();
  while(1)
 {
   PROCESS_WAIT_EVENT();
   if (ev_serial_ch == ev)
       ShellRecvProcess();
 }
  PROCESS_END();
}

