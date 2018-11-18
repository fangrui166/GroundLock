#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "flash_drv.h"
#include "flash_memory_define.h"
#include "string.h"


/* Private variables */

/* Private functions */

/*
 * flash init sequence without ThreadX components
 */
int  flash_init(void)
{
    //xlog(LOG_TAG, HTCLOG_INFO, "flash driver init start\n");

    /* enable flash write protect */
    /* Lock the Option Bytes */
    HAL_FLASH_OB_Lock();
    /* Lock the Flash Program Erase controller */
    HAL_FLASH_Lock();
    return 0;
}

int flash_deinit(void)
{
    /* enable flash write protect */
    /* Lock the Option Bytes */
    HAL_FLASH_OB_Unlock();
    /* Lock the Flash Program Erase controller */
    HAL_FLASH_Unlock();
    return 0;
}
// get the address in which sector
static FLASH_PART get_erase_sector(uint32_t addr,uint32_t len)
{
    if(addr>=REGION_FLASH_SYS&&(addr+len-1)<REGION_FLASH_EXP ){
        return PART_SYS;
    }
    else if(addr>=REGION_FLASH_EXP&&(addr+len-1)<REGION_FLASH_MISC_RW ){
        return PART_EXP;
    }
    else if(addr>=REGION_FLASH_MISC_RW&&(addr+len-1)<REGION_FLASH_MISC_RO ){
        return PART_MISC_RW;
    }
	else if(addr>=REGION_FLASH_MISC_RO&&(addr+len-1)<REGION_FLASH_END ){
        return PART_MISC_RO;
    }
	else
		return PART_ERROR;
}
uint32_t flash_read(uint32_t addr, void *buff, unsigned size)
{
    if(buff==NULL){
        flash_err(" erro due to buffer is null\n");
        return 0;
    }
    addr = addr - REGION_FLASH_START;
    memcpy(buff, (void*)(addr), size);
    return size;
}
/*
 * Erase flash
 */

uint32_t flash_erase_pages(uint32_t addr, uint32_t num){
    volatile HAL_StatusTypeDef FLASHStatus = HAL_OK;
    FLASH_EraseInitTypeDef EraseInfo;
	uint32_t EraseErrorPage = 0;
    if (!IS_FLASH_PROGRAM_ADDRESS(addr))
    {
        flash_err(" invalid start_addr 0x%X\r\n", addr);
        return 1;
    }
	// now erase can only erase one sector a time.
    EraseInfo.TypeErase =FLASH_TYPEERASE_PAGES;
	EraseInfo.PageAddress= addr;
    EraseInfo.NbPages = num;
    __disable_irq();
    /* Unlock flash */
    HAL_FLASH_Unlock();
    /* Clear ERR flags */
    __HAL_FLASH_CLEAR_FLAG(  FLASH_SR_WRPRTERR | FLASH_SR_PGERR);
	FLASHStatus = HAL_FLASHEx_Erase(&EraseInfo, &EraseErrorPage);

	if (FLASHStatus != HAL_OK)
    {
        /* Lock flash */
        HAL_FLASH_Lock();
        __enable_irq();

        flash_err("erase failed !! status=%d, address = 0x%08X\r\n", FLASHStatus, addr );

        return 1;
    }
	HAL_FLASH_Lock();
    __enable_irq();

    return 0;
}
uint32_t flash_erase(uint32_t addr, uint32_t length)
{
    volatile uint32_t NbrOfPage = 0x0;
    volatile HAL_StatusTypeDef FLASHStatus = HAL_OK;
    uint32_t end_addr = 0;
    FLASH_EraseInitTypeDef EraseInfo;
    uint32_t EraseErrorPage = 0;
    FLASH_PART curr_sec;
    //flash_err("[FLASH_DRV] flash_erase addr=0x%X, length=%u\r\n", addr, length);

    /* Check write range */
    if (length == 0)
    {
        flash_err(" invalid length 0x%08X\r\n", length);
        return 1;
    }

    if (!IS_FLASH_PROGRAM_ADDRESS(addr))
    {
        flash_err("[FLASH_DRV] invalid start_addr 0x%X\r\n", addr);
        return 1;
    }
    end_addr = addr + length;
    if (!IS_FLASH_PROGRAM_ADDRESS(end_addr - 1))
    {
        flash_err(" invalid end_addr 0x%X\r\n", end_addr);
        return 1;
    }
    curr_sec = get_erase_sector(addr,length);
    if( curr_sec == PART_ERROR){
        flash_err("invalid sector to erase start 0x%x, end 0x%x\r\n",addr, end_addr);
        return 1;
    }
    // now erase can only erase one sector a time.
    EraseInfo.TypeErase =FLASH_TYPEERASE_PAGES;
    __disable_irq();
    /* Unlock flash */
    HAL_FLASH_Unlock();
    /* Clear ERR flags */
    __HAL_FLASH_CLEAR_FLAG(  FLASH_SR_WRPRTERR | FLASH_SR_PGERR);
    switch(curr_sec){
    case PART_SYS:
        EraseInfo.PageAddress= REGION_FLASH_SYS;
        EraseInfo.NbPages = SYS_PAGES;
        break;
    case PART_MISC_RW:
        EraseInfo.PageAddress= REGION_FLASH_MISC_RW;
        EraseInfo.NbPages = MISC_RW_PAGES;
        break;
    case PART_MISC_RO:
        EraseInfo.PageAddress= REGION_FLASH_MISC_RO;
        EraseInfo.NbPages = MISC_MFG_PAGES;
        break;
    case PART_EXP:
        EraseInfo.PageAddress= REGION_FLASH_EXP;
        EraseInfo.NbPages = EXP_PAGES;
        break;
	default:
        flash_err(" invalid sector to erase start 0x%x, end 0x%x\r\n",addr, end_addr);
        return 1;
    }
    // only has bank1

	FLASHStatus = HAL_FLASHEx_Erase(&EraseInfo, &EraseErrorPage);

	if (FLASHStatus != HAL_OK)
    {
        /* Lock flash */
        HAL_FLASH_Lock();
        __enable_irq();

        flash_err("erase failed !! status=%d, address = 0x%08X\r\n", FLASHStatus, addr );

        return 1;
    }

    /* Lock flash */
    HAL_FLASH_Lock();
    __enable_irq();

    return 0;
}


/*
 * Write to flash without erase
 */
uint32_t flash_write(void * source, uint32_t addr, uint32_t length)
{
    uint8_t alignment_bytes = 0x0;
    HAL_StatusTypeDef FLASHStatus = HAL_OK;
    uint32_t end_addr = 0;
    uint32_t iterator = 0x0;
    uint64_t data = 0;
    uint8_t alignment=0;
    //flash_info("[FLASH_DRV] flash_write offset=0x%X, length=%u\r\n", addr, length);
    /* Check write range */
    if (!IS_FLASH_PROGRAM_ADDRESS(addr)){
        flash_err(" invalid addr %u\r\n", addr);
        return 1;
    }
    end_addr = addr + length;
    if (!IS_FLASH_PROGRAM_ADDRESS(end_addr)){
        flash_err("invalid end_addr %u\r\n", end_addr);
        return 1;
    }

   /* if( addr%8 == 0){
        alignment = FLASH_TYPEPROGRAM_DOUBLEWORD;
        alignment_bytes = 8;
	}else if( addr%4 == 0){
        alignment = FLASH_TYPEPROGRAM_WORD;
        alignment_bytes = 4;
    } else*/ if( addr%2 == 0){
        alignment = FLASH_TYPEPROGRAM_HALFWORD;
        alignment_bytes = 2;
    }else {
        flash_err("invalid addr %u for halfword\r\n", addr);
        return 1;
	}
    __disable_irq();
    /* Unlock flash */
    HAL_FLASH_Unlock();

    /* Clear ERR flags */
    __HAL_FLASH_CLEAR_FLAG(   FLASH_SR_WRPRTERR | FLASH_SR_PGERR  );
    /* Write loop */
    iterator = 0x0;


    while((iterator+alignment_bytes) <= length)
    {
		if(alignment == FLASH_TYPEPROGRAM_DOUBLEWORD){
			data = * ((uint64_t *)(source)+ (iterator/alignment_bytes));
				FLASHStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (addr + iterator), data);

		} else if( alignment == FLASH_TYPEPROGRAM_WORD){
				data = * ((uint32_t *)(source)+ (iterator/alignment_bytes));
				FLASHStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (addr + iterator), data);

		} else if( alignment == FLASH_TYPEPROGRAM_HALFWORD){

				*((uint8_t *)&data) = * ((uint8_t *)(source)+ (iterator));
                *(((uint8_t *)&data)+1) = * ((uint8_t *)(source)+ (iterator)+1);
				FLASHStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, (addr + iterator), data);

		}
		if (FLASHStatus != HAL_OK){
			break;
		}
			iterator += alignment_bytes;
    }
    // flash left data
	// warnning may overwrite 1 byte.
    alignment_bytes = 2;
    while(iterator<length){
        *((uint8_t *)&data) = * ((uint8_t *)(source)+ (iterator));
        *(((uint8_t *)&data)+1) = * ((uint8_t *)(source)+ (iterator)+1);
		FLASHStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, (addr + iterator), data);
		if (FLASHStatus != HAL_OK){
            flash_err(" failed !!  iterator = %u\r\n", iterator);
			break;
		}
		iterator += alignment_bytes;
    }
	if( iterator>length){

        flash_err("failed !!iterator = %u\r\n", iterator);
	}

    if (FLASHStatus != HAL_OK)
    {
        /* Lock flash */
        HAL_FLASH_Lock();

        __enable_irq();


        flash_err("failed !! status=%d, iterator = %u\r\n", FLASHStatus, iterator);

        return 1;
    }

    /* Lock flash */
    HAL_FLASH_Lock();
    __enable_irq();
    return 0;
}




#if 0
/**
  * @brief  Enable NOR flash OB WP
  * @param  None
  * @retval None
  */
uint32_t flash_ob_wp_enable(FLASH_WP_AREA wp_area)
{
    FLASH_OBProgramInitTypeDef OBS_1A ;
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t ret_val = 0;

    /* Init OB structure */
    __disable_irq();

    /* Unlock the Options Bytes */
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_SR_SOP);
    HAL_FLASH_OB_Unlock();

    if(wp_area== FLASH_WP_BL ){
        /* Get current OB */
        HAL_FLASHEx_OBGetConfig(&OBS_1A);
        OBS_1A.OptionType = OPTIONBYTE_WRP;
        OBS_1A.WRPState = OB_WRPSTATE_ENABLE;
        OBS_1A.WRPSector = OB_WRP_SECTOR_0|OB_WRP_SECTOR_1;
        status = HAL_FLASHEx_OBProgram(&OBS_1A);


    } else if(wp_area==FLASH_WP_SYS) {

        HAL_FLASHEx_OBGetConfig(&OBS_1A);
        OBS_1A.OptionType = OPTIONBYTE_WRP;
        OBS_1A.WRPState = OB_WRPSTATE_ENABLE;
        OBS_1A.WRPSector = OB_WRP_SECTOR_6|OB_WRP_SECTOR_7;
        status = HAL_FLASHEx_OBProgram(&OBS_1A);

        if(status != HAL_OK) { ret_val = 1; }

    } else {
        ret_val = 5;
        goto FLASH_OB_WP_EXIT;
    }



FLASH_OB_WP_EXIT:
    /* Lock the Options Bytes */
    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();

    __enable_irq();

    if (ret_val > 0) {
        //flash_err("[FLASH] ERROR: flash_ob_wp_enable ret=%u status=%u \r\n", ret_val, status);
    }
    else{
        //flash_err("[FLASH] OB WRP set success\r\n");
    }

    return ret_val;
}


/**
  * @brief  Disable NOR flash OB  WP
  * @param  None
  * @retval None
  */
uint32_t flash_ob_wp_disable(FLASH_WP_AREA wp_area)
{
    FLASH_OBProgramInitTypeDef OBS_1A = {0};
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t ret_val = 0;

    /* Init OB structure */

    __disable_irq();

    /* Unlock the Options Bytes */
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_SR_SOP);
    HAL_FLASH_OB_Unlock();

    if(wp_area== FLASH_WP_BL ){

        /* Get current OB */
        HAL_FLASHEx_OBGetConfig(&OBS_1A);
        OBS_1A.OptionType = OPTIONBYTE_WRP;
        OBS_1A.WRPState = OB_WRPSTATE_DISABLE;
        OBS_1A.WRPSector =OB_WRP_SECTOR_0|OB_WRP_SECTOR_1;
        status = HAL_FLASHEx_OBProgram(&OBS_1A);

        if(status != HAL_OK) { ret_val = 1; }

    } else if(wp_area==FLASH_WP_SYS) {

        HAL_FLASHEx_OBGetConfig(&OBS_1A);
        OBS_1A.OptionType = OPTIONBYTE_WRP;
        OBS_1A.WRPState = OB_WRPSTATE_DISABLE;
        OBS_1A.WRPSector = OB_WRP_SECTOR_6|OB_WRP_SECTOR_7;
        status = HAL_FLASHEx_OBProgram(&OBS_1A);

        if(status != HAL_OK) { ret_val = 1; }

    } else {
        ret_val = 5;
        goto FLASH_OB_WP_EXIT;
    }

FLASH_OB_WP_EXIT:
    /* Lock the Options Bytes */
    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();

    __enable_irq();

    if (ret_val > 0) {
        //flash_err("[FLASH] ERROR: flash_ob_wp_disable ret=%u status=%u \r\n", ret_val, status);
    }
    else{
        //flash_err("[FLASH] OB WRP Disable success\r\n");
    }

    return 0;
}
#endif

