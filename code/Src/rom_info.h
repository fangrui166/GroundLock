#ifndef __ROM_INFO_H__
#define __ROM_INFO_H__
#define FW_VERSION_STR							"0.1.0.0"	/**< version number */

extern const char  ROM_info[16];
extern const char  ROM_buildtime[22];
extern const char  ROM_version[16];
void rom_info(void);

#endif
