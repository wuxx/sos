#include <memory_map.h>
#include <libc.h>
#include "timer.h"
#include "log.h"

s32 test_timer_all(u32 argc, char **argv)
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
            dump_mem(CORETIMER_BASE, 9);
            break;
        case (1):
            dump_mem(SYSTIMER_BASE, 7);
            break;
        case (2):
            ret = timer_init();
            break;
        case (3):
            sc = get_syscounter();
            chi = (u32)(sc >> 32);
            clo = (u32)(sc);
            PRINT_EMG("[%x] [%x] \n", chi, clo);
            break;
        case (100): /* udelay */
            PRINT_STAMP();
            udelay(arg1);
            PRINT_STAMP();
            break;
        case (101):
            PRINT_STAMP();
            mdelay(arg1);
            PRINT_STAMP();
            break;
        default:
            return -1;
    }

    return ret;
}
