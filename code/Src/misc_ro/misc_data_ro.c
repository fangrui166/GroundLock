#include <string.h>
#include <stdlib.h>
#include "stm32f1xx.h"
#include "misc_data_ro.h"
#include "flash_drv.h"
#include "flash_memory_define.h"
//#include "usbd_app.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
MISC_DataTypeDefRO MISC_Data_RO = {0};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

int loadMiscDataRO(void)
{
	uint32_t ret;
	misc_info("loadMiscData\n");
	ret = flash_read(REGION_FLASH_MISC_RO, (void *)&MISC_Data_RO, sizeof(MISC_DataTypeDefRO));
	if (ret == sizeof(MISC_DataTypeDefRO)) {
		return 0;
	} else {
		misc_err("load misc data failed\n");
		return -1;
	}
}

int saveMiscDataRO(void)
{
	uint32_t ret;
	misc_info("saveMiscData\n");
	ret = flash_erase(REGION_FLASH_MISC_RO,sizeof(MISC_DataTypeDefRO));
	if (ret) {
		misc_err("[misc_data] flash erase failed\n");
		return -1;
	}
	ret = flash_write(&MISC_Data_RO,REGION_FLASH_MISC_RO, sizeof(MISC_DataTypeDefRO));
	if (ret) {
		misc_err("flash write failed\n");
		return -1;
	}
	return 0;
}

static BOOL misc_is_ascii_char(char char_key)
{
	if (char_key >= 0x20 && char_key <= 0x7E)
		return TRUE;

	return FALSE;
}

int getSerialNumber(char *value)
{
	if (!loadMiscDataRO()) {
		if (!misc_is_ascii_char(MISC_Data_RO.serialNumber[0])){
			return -1;
		}
		strcpy(value,(const char *)&MISC_Data_RO.serialNumber);
		return 0;
	} else {
		misc_err("getSerialNumber failed\n");
		return -1;
	}
}

int setSerialNumber(const char *value, int length)
{
	int __length = sizeof(MISC_Data_RO.serialNumber) - 1;
	if (length > __length) {
		length = __length;
		return -2;
	}
	if (!loadMiscDataRO()) {
		memset((char *)&(MISC_Data_RO.serialNumber), 0x0,
				sizeof(MISC_Data_RO.serialNumber));
		strncpy((char *)&(MISC_Data_RO.serialNumber), value, length);
	} else {
		misc_err(" failed\n");
		return -1;
	}
	if (!saveMiscDataRO()) {
		misc_info(" success\n");
		return 0;
	} else {
		misc_err(" failed\n");
		return -1;
	}
}

int getHwVersion(char *value)
{
	if (!loadMiscDataRO()) {
		if (!misc_is_ascii_char(MISC_Data_RO.HwVersion[0])){
			return -1;
		}
		strcpy(value,(const char *)&MISC_Data_RO.HwVersion);
		return 0;
	} else {
		misc_err("getSerialNumber failed\n");
		return -1;
	}
}

int setHwVersion(const char *value, int length)
{
	int __length = sizeof(MISC_Data_RO.HwVersion) - 1;
	if (length > __length) {
		length = __length;
		return -2;
	}
	if (!loadMiscDataRO()) {
		memset((char *)&(MISC_Data_RO.HwVersion), 0x0,
				sizeof(MISC_Data_RO.HwVersion));
		strncpy((char *)&(MISC_Data_RO.HwVersion), value, length);
	} else {
		misc_err(" failed\n");
		return -1;
	}
	if (!saveMiscDataRO()) {
		misc_info(" success\n");
		return 0;
	} else {
		misc_err(" failed\n");
		return -1;
	}
}

int getBaudRate(uint32_t *value)
{
	if (!loadMiscDataRO()) {
		*value = MISC_Data_RO.BaudRate;
		return 0;
	} else {
		misc_err(" failed\n");
		return -1;
	}
}

int setBaudRate(uint32_t     value)
{
	if (!loadMiscDataRO()) {
		MISC_Data_RO.BaudRate = value;
	} else {
		misc_err(" failed\n");
		return -1;
	}
	if (!saveMiscDataRO()) {
		misc_info("setBaudRate success\n");
		return 0;
	} else {
		misc_err(" failed\n");
		return -1;
	}
}
int getLimitedCurrent(uint16_t *value, current_limited_t type)
{
	if (!loadMiscDataRO()) {
        switch(type){
            case UP_RESISTANCE:
                *value = MISC_Data_RO.up_meet_resistance_current;
                break;
            case UP_GIG_DAMP:
                *value = MISC_Data_RO.up_big_damp_current;
                break;
            case DOWN_RESISTANCE:
                *value = MISC_Data_RO.down_meet_resistance_current;
                break;
            case DOWN_GIG_DAMP:
                *value = MISC_Data_RO.down_big_damp_current;
                break;
        }
		return 0;
	} else {
		misc_err(" failed\n");
		return -1;
	}
}
int setLimitedCurrent(uint16_t     value,  current_limited_t type)
{
	if (!loadMiscDataRO()) {
        switch(type){
            case UP_RESISTANCE:
                MISC_Data_RO.up_meet_resistance_current = value;
                break;
            case UP_GIG_DAMP:
                MISC_Data_RO.up_big_damp_current = value;
                break;
            case DOWN_RESISTANCE:
                MISC_Data_RO.down_meet_resistance_current = value;
                break;
            case DOWN_GIG_DAMP:
                MISC_Data_RO.down_big_damp_current = value;
                break;
        }
	} else {
		misc_err(" failed\n");
		return -1;
	}
	if (!saveMiscDataRO()) {
		misc_info("setLimitedCurrent success\n");
		return 0;
	} else {
		misc_err(" failed\n");
		return -1;
	}
}
