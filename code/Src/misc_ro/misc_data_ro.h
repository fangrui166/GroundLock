#ifndef __MISC_DATA_H
#define __MISC_DATA_H
#ifdef __cplusplus
extern "C" {
#endif

#include <contiki.h>
#include <stdbool.h>
#include "integer.h"

#define u8    uint8_t
#define u16   uint16_t
#define u32   uint32_t

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

typedef enum
{
	SEMI_FINISH 	= 0x18,
	SEMI_UNFINISH	= 0x58,
}SEMI_STATUS;
#define PROP_SERIALNUMBER_LENTH                    (16)
#define PROP_HWVERSION_LENTH                       (16)

typedef enum{
    UP_RESISTANCE,
    UP_GIG_DAMP,
    DOWN_RESISTANCE,
    DOWN_GIG_DAMP,
}current_limited_t;
typedef struct
{
    char serialNumber[PROP_SERIALNUMBER_LENTH];
    char HwVersion[PROP_HWVERSION_LENTH];
    uint32_t BaudRate;
    uint16_t up_meet_resistance_current;
    uint16_t up_big_damp_current;
    uint16_t down_meet_resistance_current;
    uint16_t down_big_damp_current;
}
MISC_DataTypeDefRO;



int loadMiscDataRORO(void);
int saveMiscData(void);

int getSerialNumber(char *value);
int setSerialNumber(const char *value, int length);
int getHwVersion(char *value);
int setHwVersion(const char *value, int length);
int getBaudRate(uint32_t *value);
int setBaudRate(uint32_t     value);
int getLimitedCurrent(uint16_t *value, current_limited_t type);
int setLimitedCurrent(uint16_t     value,  current_limited_t type);


#ifdef __cplusplus
}
#endif

#endif /* __MiSC_DATA_H */

