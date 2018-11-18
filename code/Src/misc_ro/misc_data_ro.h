#ifndef __MISC_DATA_H
#define __MISC_DATA_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
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

typedef struct
{
    char serialNumber[PROP_SERIALNUMBER_LENTH];
    char HwVersion[PROP_HWVERSION_LENTH];
    uint32_t BaudRate;
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


#ifdef __cplusplus
}
#endif

#endif /* __MiSC_DATA_H */

