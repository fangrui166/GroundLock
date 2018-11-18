#ifndef __MISC_DATA_RW_H
#define __MISC_DATA_RW_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#define misc_emerg(fmt, ...) \
    printf("[MISC][EMR] :" fmt, ##__VA_ARGS__)
#define misc_err(fmt, ...) \
	printf("[MISC][ERR] :" fmt, ##__VA_ARGS__)
#define misc_warning(fmt, ...) \
	printf("[MISC][WARN]:" fmt, ##__VA_ARGS__)
#define misc_info(fmt, ...) \
	printf("[MISC][INFO]:" fmt, ##__VA_ARGS__)
#define misc_debug(fmt, ...) \
	printf("[MISC][DBG] :" fmt, ##__VA_ARGS__)




typedef enum{
	FRIST_BOOT_MODE	= 0xFFFFFFFF,
	NORM_MODE	= 0xAEAEAEAE,
	ERROR_MODE	= 0xEDEDEDED,
} BOOT_MODE;


#define DFU_STR "DFU"
#define NORMAL_STR "NORM"
#define REBOOT_STR "reboot"
#define WDT_RESET "watchdog reset"
#define RF_DOWNLOAD  "rf_download"



/*magnetic sensor struct define end*/

typedef struct
{
    BOOT_MODE Boot_flag;

} MISC_DataTypeDefRW;



BOOT_MODE get_bootmode();
int set_bootmode(BOOT_MODE mode);












#ifdef __cplusplus
}
#endif

#endif /* __MiSC_DATA_H */

