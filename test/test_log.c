#include <memory_map.h>
#include <libc.h>
#include "log.h"
#include "systest.h"

char *loglevel_desc[] = {
        "LOG_EMG",
        "LOG_ERR",
        "LOG_WARN",
        "LOG_INFO",
        "LOG_DEBUG",
        "LOG_MAX",
};

s32 test_log_all(u32 argc, char **argv)
{
    s32 ret = 0;
    u32 i, arg1;
    i    = atoi(argv[2]);
    arg1 = atoi(argv[3]);
    switch(i) {
        case (0):
            arg1 = arg1 > LOG_MAX ? LOG_MAX : arg1;
            PRINT_EMG("set loglevel [%s]\n", loglevel_desc[arg1]);
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
