#include <memory_map.h>
#include <libc.h>
#include "timer.h"

s32 test_timer_all(u32 argc, char **argv)
{
    s32 ret = 0;
    u32 i;
    i = atoi(argv[2]);
    switch(i) {
        case (0):
            dump_mem(CORETIMER_BASE, 9);
            break;
        case (1):
            ret = timer_init();
            break;
        default:
            return -1;
    }

    return ret;
}
