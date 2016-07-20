#include <memory_map.h>
#include <libc.h>
#include "timer.h"
#include "log.h"
#include "watchdog.h"
#include "systest.h"

s32 test_wdt_all(u32 argc, char **argv)
{
    u64 sc;
    u32 clo, chi;
    s32 ret = 0;
    u32 i, arg1;
    i    = atoi(argv[2]);
    arg1 = atoi(argv[3]);

    PRINT_EMG("arg1: %d\n", arg1);

    switch (i) {
        case (0):
            dump_mem(WATCHDOG_BASE, 12);
            break;
        case (1):
            watchdog_ctrl(arg1);
            break;
        default:
            return -1;
    }

    return ret;
}
