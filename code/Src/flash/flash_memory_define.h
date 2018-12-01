#ifndef __FLASH_MEMORY_DEFINE_H__
#define __FLASH_MEMORY_DEFINE_H__

/* Internal Flash */
#ifdef STM32F103_128K
#define REGION_FLASH_START      0x08000000
#define REGION_FLASH_SYS        0x08000000	// 122K
#define REGION_FLASH_EXP        0x0801E800	// 2K
#define REGION_FLASH_MISC_RW    0x0801F000	// 2K
#define REGION_FLASH_MISC_RO    0x0801F800	// 2K
#define REGION_FLASH_END        0x08020000	// 128
#else
#define REGION_FLASH_START      0x08000000
#define REGION_FLASH_SYS        0x08000000	// 58K
#define REGION_FLASH_EXP        0x0800E800	// 2K
#define REGION_FLASH_MISC_RW    0x0800F000	// 2K
#define REGION_FLASH_MISC_RO    0x0800F800	// 2K
#define REGION_FLASH_END        0x08010000	// 64
#endif

#define SYS_PAGES				((REGION_FLASH_EXP-REGION_FLASH_SYS)/0x800)
#define EXP_PAGES				((REGION_FLASH_MISC_RW-REGION_FLASH_EXP)/0x800)
#define MISC_RW_PAGES			((REGION_FLASH_MISC_RO-REGION_FLASH_MISC_RW)/0x800)
#define MISC_MFG_PAGES			((REGION_FLASH_END-REGION_FLASH_MISC_RO)/0x800)

#endif
