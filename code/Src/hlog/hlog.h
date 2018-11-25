#ifndef __HLOG_H
#define __HLOG_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>

/**@brief HLOG level IDs used to indicate the level of the log. */
typedef enum {
     HLOG_LVL_ERR,
     HLOG_LVL_WARNING,
     HLOG_LVL_INFO,
     HLOG_LVL_DEBUG,
     HLOG_LVL_VERBOSE,
     HLOG_LVL_MAX
} hlog_level_t;

#ifdef DEBUG_BUILD
#define HLOG_ENABLED 		1
#define HLOG_LVL_DEFAULT 	HLOG_LVL_VERBOSE			/**hlog defalut print level**/
#else
#define HLOG_ENABLED 		1
#define HLOG_LVL_DEFAULT 	HLOG_LVL_INFO			/**hlog defalut print level**/
#endif

#define CONFIG_LOG_PART_ADDR       REGION_FLASH_EXP
#define CONFIG_LOG_PART_SIZE       (2*1024)
#define CONFIG_LOG_HEAD_ADDR       CONFIG_LOG_PART_ADDR
#define CONFIG_LOG_HEAD_SIZE       12
#define CONFIG_LOG_SAVE_ADDR       (CONFIG_LOG_PART_ADDR+CONFIG_LOG_HEAD_SIZE)

typedef enum{
    PENDING_BY_EXP = 0XABABABAB,
    PENDING_BY_WDG = 0XBABABABA,
}log_pending_t;

typedef struct {
	uint32_t log_length;
    uint32_t  log_exit_flag;
    log_pending_t log_pending_type;
}flash_log_head_t;


/**hlog_dump_logbuf - dump log from log buffer to uart*/
int hlog_dump_logbuf(void);
/**hlog_save_logbuf - save exception log from log buffer to flash*/
int hlog_save_logbuf(void);
/**hlog_dump_flashlog - dump exception log from flash to uart*/
int hlog_dump_flashlog(void);

/**@brief hlog_printf - Format and print a string
 *
 * @param[in]	level							Log level
 * @param[in]	fmt								The format string to use
 * @param[in]	...								Arguments for the format string
 *
 * @retval		the number of characters 		If the print successfully.
 * @retval		<0						 		If the print fail.
 */
int hlog_printf(int level,char *fmt, ...);
/**@brief hlog_expinfo_printf - Format and print a string for exception log
 *
 * @param[in]	fmt								The format string to use
 * @param[in]	...								Arguments for the format string
 *
 * @retval		the number of characters 		If the print successfully.
 * @retval		<0						 		If the print fail.
 */

int hlog_expinfo_printf(char *fmt, ...);
/**hlog_init - hlog initial function*/
int hlog_init(void);


void hlog_excep_dump_enable(void);
void hlog_excep_dump_disable(void);

/*hlog redefination*/
#define loge(fmt, ...) \
    hlog_printf(HLOG_LVL_ERR, fmt, ##__VA_ARGS__)
#define logw(fmt, ...) \
    hlog_printf(HLOG_LVL_WARNING, fmt, ##__VA_ARGS__)
#define logi(fmt, ...) \
    hlog_printf(HLOG_LVL_INFO, fmt, ##__VA_ARGS__)
#define logd(fmt, ...) \
    hlog_printf(HLOG_LVL_DEBUG, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // __HLOG_H

