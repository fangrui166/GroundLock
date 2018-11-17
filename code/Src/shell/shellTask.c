#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <contiki.h>
#include "stm32f1xx_hal.h"
#include "shellTask.h"

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

int cmd_help(int argc, char * argv[], _command_source source);
const MONITOR_COMMAND commandTable[] =
{
    {"task",    get_task_state},
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
        (0x0D == CMDCacheBuf[CMD_Cache_Head].buf[rec_count])){
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

