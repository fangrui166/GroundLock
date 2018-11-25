#include "rom_info.h"
#include "hlog.h"
#include <stddef.h>
#include <stdint.h>



extern const unsigned short ielftool_checksum;

__root const unsigned char  checksum_start    @ "checksum_start_mark" = 0;
__root const unsigned char  checksum_end[4]   @ "checksum_end_mark" ={01,0,0,0xEE};
__root const char  ROM_info[16]      @ "ROM_info" = "yiwaGL";
__root const char  ROM_buildtime[22] @ "ROM_buildtime" = __DATE__"-"__TIME__;
__root const char  ROM_version[16]   @ "ROM_version" = FW_VERSION_STR;
// The last byte is the actual "checksum area end mark"
#pragma section = "ROM_CONTENT"
const size_t ROM_CONTENT_size @ "ROM_length_used" = __section_size("ROM_CONTENT");


void rom_info(void)
{
    logi("ROM size=%d \r\n",ROM_CONTENT_size);
    logi("checksum=%#x \r\n",ielftool_checksum);
}


