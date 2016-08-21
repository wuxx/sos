#include <memory_map.h>
#include <libc.h>
#include "log.h"
#include "systest.h"

extern void dumpb(void *buf, u32 size);

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
    u32 i, arg1, arg2;

    i    = atoi(argv[2]);
    arg1 = atoi(argv[3]);
    arg2 = atoi(argv[4]);

    SHOW_VAR(i);
    SHOW_VAR(arg1);
    SHOW_VAR(arg2);

    switch (i) {
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
        case (2):
            dump_log();
            break;
        case (3):
            dumpb((void *)arg1, arg2);
            break;
        default:
            return -1;
    }

    return ret;
}
