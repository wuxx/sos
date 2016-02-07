#include <memory_map.h>
#include <libc.h>
#include "timer.h"
#include "log.h"

extern u32 os_tick;

s32 test_os_all(u32 argc, char **argv)
{
    u64 sc;
    u32 clo, chi;
    s32 ret = 0;
    u32 i, arg1;
    i    = atoi(argv[2]);
    arg1 = atoi(argv[3]);

    PRINT_EMG("arg1: %d\n", arg1);

    switch(i) {
        case (0):
            PRINT_EMG("os_tick: 0x%x\n", os_tick);
            break;
        default:
            return -1;
    }

    return ret;
}
