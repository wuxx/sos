#include <libc.h>
#include <memory_map.h>
#include "log.h"
#include "watchdog.h"

PRIVATE u32 default_timeout = 10;   /* 1000 tick */

PUBLIC u32 watchdog_init()
{
    return 0;
}

PUBLIC s32 watchdog_ctrl(u32 cmd, ...)
{
    va_list args;
    va_start(args, cmd);
    u32 cur_rstc;
    u32 timeout = 0;

    switch (cmd) { 
        case (WDT_START):
            cur_rstc = readl(WATCHDOG_RSTC);
            writel(WATCHDOG_RSTC, WDT_PASSWORD | (cur_rstc & 0xFFFFFFCF) | 0x00000020);
            break;
        case (WDT_STOP):
            writel(WATCHDOG_RSTC, WDT_PASSWORD | 0x00000102);
            break;
        case (WDT_FEED):
            writel(WATCHDOG_WDOG, WDT_PASSWORD | ((default_timeout) & 0x000FFFFF));
            cur_rstc = readl(WATCHDOG_RSTC);
            writel(WATCHDOG_RSTC, WDT_PASSWORD | (cur_rstc & 0xFFFFFFCF) | 0x00000020);
            break;
        case (WDT_SET_TIMEOUT):
            timeout = va_arg(args, u32);
            va_end(args);
            writel(WATCHDOG_WDOG, WDT_PASSWORD | ((timeout) & 0x000FFFFF));
            break;
        default:
            PRINT_ERR("unknown watchdog cmd %x \n", cmd);
            return -1;
            break;
    }

    return 0;
}
