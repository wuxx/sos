#include <memory_map.h>
#include <libc.h>
#include "log.h"

s32 test_log_all(u32 argc, char **argv)
{
    s32 ret = 0;
    u32 i, arg1;
    i    = atoi(argv[2]);
    arg1 = atoi(argv[3]);
    switch(i) {
        case (0):
            ret = set_log_level(arg1);
            break;
        case (1):
            log(LOG_EMG,   "%d: %s\n", __LINE__, "hello, world!");
            log(LOG_ERR, "%d: %s\n", __LINE__, "hello, world!");
            log(LOG_WARN,  "%d: %s\n", __LINE__, "hello, world!");
            log(LOG_INFO,  "%d: %s\n", __LINE__, "hello, world!");
            log(LOG_DEBUG, "%d: %s\n", __LINE__, "hello, world!");
            break;
        default:
            return -1;
    }

    return ret;
}
