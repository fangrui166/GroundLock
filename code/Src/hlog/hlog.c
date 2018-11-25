#include "hlog.h"
#include "flash_drv.h"
#include "flash_memory_define.h"
#include "stdarg.h"
#include "wdt.h"
#include <stdio.h>
#include "stdlib.h"
#include <contiki.h>
#include <string.h>
#include "uart_debug_drv.h"


#define HLOG_LVL_LVL0 						HLOG_LVL_ERR
#define CONFIG_HLOG_HZ						(CLOCK_CONF_SECOND)
#define hlog_get_tick()						(HAL_GetTick())
#define CONFIG_LOGMSG_BUF_SIZE				128
#define HLOG_META_FMT "[%u.%u]<%s> "
static char *hlog_level_names[HLOG_LVL_MAX] = {"E", "W", "I", "D" , "V"};



/*log buffer start*/
#define FLASH_ACCESS_STACK_SIZE             64
#define CONFIG_LOG_BUF_SIZE                 (2*1024)
static volatile int cursor_offset;
static char log_buf[CONFIG_LOG_BUF_SIZE];

/*hlog_exception log save start*/
int dbg_uart_write(char *buff, unsigned size);

typedef enum
{
  HLOG_EXCEP_ENABLE     = 0x00U,
  HLOG_EXCEP_DISABLE    = 0x01U
} HLOG_ExcepDef;
static HLOG_ExcepDef hlog_excep_dump_ctrl = HLOG_EXCEP_DISABLE;
static uint32_t hlog_excep_dump_len = 0;

static int logbuf_write(char *buff, int size)
{
    int tmp_size = 0;
    if(size > (CONFIG_LOG_BUF_SIZE-cursor_offset)){
        tmp_size = (CONFIG_LOG_BUF_SIZE-cursor_offset);
        memcpy(log_buf+cursor_offset, buff, tmp_size);
        if(hlog_excep_dump_len < CONFIG_LOG_PART_SIZE - CONFIG_LOG_HEAD_SIZE){
            if(hlog_excep_dump_len + CONFIG_LOG_BUF_SIZE < CONFIG_LOG_PART_SIZE - CONFIG_LOG_HEAD_SIZE){
                flash_write(log_buf,CONFIG_LOG_SAVE_ADDR+hlog_excep_dump_len,CONFIG_LOG_BUF_SIZE);
                hlog_excep_dump_len += CONFIG_LOG_BUF_SIZE;
            }
            else{
                flash_write(log_buf,CONFIG_LOG_SAVE_ADDR+hlog_excep_dump_len,CONFIG_LOG_PART_SIZE - CONFIG_LOG_HEAD_SIZE - hlog_excep_dump_len);
                hlog_excep_dump_len = CONFIG_LOG_PART_SIZE - CONFIG_LOG_HEAD_SIZE;
            }
        }
        memcpy(log_buf, buff+tmp_size, size-tmp_size);
        cursor_offset = size-tmp_size;
    }
    else{
        memcpy(log_buf+cursor_offset,buff, size);
        cursor_offset += size;
    }
    return cursor_offset;
}

static int logbuf_flush(void)
{
    flash_log_head_t log_head;
    if(hlog_excep_dump_len + cursor_offset <= CONFIG_LOG_PART_SIZE - CONFIG_LOG_HEAD_SIZE){
        if(cursor_offset%2 == 0){
            flash_write(log_buf,CONFIG_LOG_SAVE_ADDR+hlog_excep_dump_len,cursor_offset);
        }
        else{
            /*flash write must half word align,and write one more byte*/
            flash_write(log_buf,CONFIG_LOG_SAVE_ADDR+hlog_excep_dump_len,cursor_offset + 1);
        }
        hlog_excep_dump_len += cursor_offset;
    }

    log_head.log_exit_flag = 1;
    log_head.log_length = hlog_excep_dump_len;
    log_head.log_pending_type = PENDING_BY_EXP;
    flash_write(&log_head, CONFIG_LOG_HEAD_ADDR,sizeof(flash_log_head_t));
    return 0;
}

void hlog_excep_dump_enable(void){
    flash_erase(CONFIG_LOG_PART_ADDR,CONFIG_LOG_PART_SIZE);
    hlog_excep_dump_ctrl = HLOG_EXCEP_ENABLE;
}

void hlog_excep_dump_disable(void){
    logbuf_flush();
    hlog_excep_dump_ctrl = HLOG_EXCEP_DISABLE;
}

static HLOG_ExcepDef __IS_HLOG_EXCEP_ENABLE (void){
    return hlog_excep_dump_ctrl;
}
/*hlog_exception log save end*/
int hlog_dump_flashlog(void)
{
    int ret;
    int32_t size;
    uint32_t num,offs=0;
    /*read 64 byte from flash once*/
    char buff[FLASH_ACCESS_STACK_SIZE];
    flash_log_head_t log_head;
    wdt_reset();
    ret = flash_read(CONFIG_LOG_HEAD_ADDR, &log_head, sizeof(flash_log_head_t));
    if(ret != sizeof(flash_log_head_t)){
        loge("hlog flash dump head read fail\r\n");
        return -1;
    }
    if((log_head.log_exit_flag != 1) || (log_head.log_length > CONFIG_LOG_PART_SIZE - CONFIG_LOG_HEAD_SIZE) || (log_head.log_pending_type != PENDING_BY_EXP)){
        loge("have no exception log in flash\r\n");
        return -1;
    }
    size = log_head.log_length;
    while(size > 0){
        num = ((size < FLASH_ACCESS_STACK_SIZE)?size:FLASH_ACCESS_STACK_SIZE);
        ret = flash_read(CONFIG_LOG_SAVE_ADDR + offs, buff, num);
        if(ret != num){
            loge("hlog flash dump data read fail\r\n");
            return -1;
        }
        dbg_uart_write(buff, num);
        offs += num;
        size -= num;
    }
    return 0;
}
char *hlog_level_number2name(int level_number)
{
	if (level_number < HLOG_LVL_LVL0 || level_number >= HLOG_LVL_MAX)
		return NULL ;
	return hlog_level_names[level_number];
}

int dbg_uart_write(char *buff, unsigned size)
{
	return DebugUart_Transmit((uint8_t *)buff, (uint16_t)size);
}

int hlog_printf(int level, char *fmt, ...)
{
	int len = 0;
	va_list vap;
	int timestamp_len = 0;
	uint32_t tick;
	char logmsg_buf[CONFIG_LOGMSG_BUF_SIZE];

	if (level < HLOG_LVL_LVL0 || level > HLOG_LVL_MAX)
		return -1;
	if ((HLOG_ENABLED == 0) || (level > HLOG_LVL_DEFAULT))
		return 0;

	tick = hlog_get_tick();
	len += sprintf(logmsg_buf + len, HLOG_META_FMT, tick/CONFIG_HLOG_HZ, tick % CONFIG_HLOG_HZ, hlog_level_number2name(level));
	timestamp_len = len;

	va_start(vap,fmt);
    len += vsnprintf(logmsg_buf + len, CONFIG_LOGMSG_BUF_SIZE, fmt, vap);
	va_end(vap);

	/*if (len < CONFIG_LOGMSG_BUF_SIZE) {
		len++;
	} else {
		len = CONFIG_LOGMSG_BUF_SIZE;
		logmsg_buf[len -1] = '\0';
	}*/
	if(len >= CONFIG_LOGMSG_BUF_SIZE){
        len = CONFIG_LOGMSG_BUF_SIZE;
	}

    /*if(__IS_HLOG_BUF_LOCK() != HLOG_BUF_LOCKED){
        logbuf_write(logmsg_buf,len);
    }*/
	dbg_uart_write(logmsg_buf, len);

    return len - timestamp_len;
}

int hlog_expinfo_printf(char *fmt, ...)
{
    int retval;
    va_list vp;
    char logmsg_buf[CONFIG_LOGMSG_BUF_SIZE];

    if (HLOG_ENABLED == 0)
		return 0;
    va_start(vp, fmt);
    retval = vsnprintf(logmsg_buf, CONFIG_LOGMSG_BUF_SIZE, fmt, vp);
    va_end(vp);

    if(retval >= 127){
        retval = 127;
    }
    if(__IS_HLOG_EXCEP_ENABLE() == HLOG_EXCEP_ENABLE){
        logbuf_write(logmsg_buf,retval);
    }
    dbg_uart_write(logmsg_buf, retval);

    return retval;
}

int hlog_init(void)
{
    cursor_offset = 0;
    memset(log_buf, 0, CONFIG_LOG_BUF_SIZE);
    return 0;
}


