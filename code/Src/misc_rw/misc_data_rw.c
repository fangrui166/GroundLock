#include "stm32f1xx.h"
#include "misc_data_rw.h"
#include "flash_drv.h"
#include "flash_memory_define.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
MISC_DataTypeDefRW MISC_Data_RW;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

static int loadMiscDataRW(void)
{
	uint32_t ret;
	//misc_info("loadMiscData\n");
	ret = flash_read(REGION_FLASH_MISC_RW, (void *)&MISC_Data_RW, sizeof(MISC_DataTypeDefRW));
	if (ret == sizeof(MISC_DataTypeDefRW)) {
		return 0;
	} else {
		misc_err("load misc data failed\n");
		return -1;
	}
}

static int saveMiscDataRW(void)
{
	uint32_t ret;
	//misc_info("saveMiscData\n");
	ret = flash_erase(REGION_FLASH_MISC_RW,sizeof(MISC_DataTypeDefRW));
	if (ret) {
		misc_err("[misc_data] flash erase failed\n");
		return -1;
	}
	ret = flash_write(&MISC_Data_RW,REGION_FLASH_MISC_RW,
	sizeof(MISC_DataTypeDefRW));
	if (ret) {
		misc_err("flash write failed\n");
		return -1;
	}
	return 0;
}


BOOT_MODE get_bootmode()
{
	if (!loadMiscDataRW()) {
		return MISC_Data_RW.Boot_flag;
	} else {
		misc_err("get_bootmode failed\n");
		return ERROR_MODE;
	}
}

int set_bootmode(BOOT_MODE mode)
{
	if (!loadMiscDataRW()) {
		MISC_Data_RW.Boot_flag = mode;
	} else {
		misc_err("set_bootmode failed\n");
		return -1;
	}
	if (!saveMiscDataRW()) {
		misc_info("set_bootmode success\n");
		return 0;
	} else {
		misc_err("set_bootmode failed\n");
		return -1;
	}
}
