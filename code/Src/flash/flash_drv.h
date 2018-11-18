/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_DRV_H__
#define __FLASH_DRV_H__

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
#include <stdint.h>

typedef enum
{
    FLASH_IRQ_NO = 0x0,
    FLASH_IRQ_NORMAL = 0x1,
} FLASH_IRQ_STATE;
typedef enum
{
    FLASH_WP_BL = 0x0,
    FLASH_WP_SYS = 0x6,
} FLASH_WP_AREA;

typedef enum {
     PART_SYS,          /* 122K */
     PART_MISC_RW,      /* 2K   */
     PART_MISC_RO,      /* 2K   */
     PART_EXP,          /* 2K   */
     PART_ERROR,
 }FLASH_PART;

int flash_init(void);
uint32_t flash_erase(uint32_t offset, uint32_t length);
uint32_t flash_erase_pages(uint32_t addr, uint32_t num);
uint32_t flash_write(void * source, uint32_t offset, uint32_t length);
uint32_t flash_read(uint32_t addr, void *buff, unsigned size);

//uint32_t flash_ob_wp_disable(FLASH_WP_AREA wp_area);
//uint32_t flash_ob_wp_enable(FLASH_WP_AREA wp_area);
int flash_deinit(void);


#define flash_err(fmt, ...) \
	printf("[ERR] :" fmt, ##__VA_ARGS__)
#define flash_warning(fmt, ...) \
	printf("[WARN]:" fmt, ##__VA_ARGS__)
#define flash_info(fmt, ...) \
	printf("[INFO]:" fmt, ##__VA_ARGS__)
#define flash_debug(fmt, ...) \
	printf("[DBG] :" fmt, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ifndef __FLASH_DRV_H__ */
