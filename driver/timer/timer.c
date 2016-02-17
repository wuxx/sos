#include <libc.h>
#include <int.h>
#include <memory_map.h>
#include <system_config.h>

#include "log.h"
#include "timer.h"

/* increment syscounter */
PUBLIC u64 get_syscounter()
{
    u64 sc;
    u32 clo, chi;
    clo = readl(SYSTMCLO);
    chi = readl(SYSTMCHI);
    sc  = (chi << 32) | clo;
    return sc;
}

PUBLIC void udelay(u32 us)
{
    u64 sc_start, sc_end;
    u32 ticks;

    us = us > 1000 ? 1000 : us; /* max of 1ms */
    ticks = US2TICK(us);

    sc_start = get_syscounter();
    sc_end   = get_syscounter();

    while ((sc_end - sc_start) < ticks) {
        sc_end = get_syscounter();
    }

}

PUBLIC void mdelay(u32 ms)
{
    u64 sc_start, sc_end;
    u32 ticks;

    ms = ms > 100000 ? 100000 : ms; /* max of 100s */
    ticks = MS2TICK(ms);

    sc_start = get_syscounter();
    sc_end   = get_syscounter();

    while ((sc_end - sc_start) < ticks) {
        sc_end = get_syscounter();
    }

}

PUBLIC void clk_delay(u32 count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : 
            : [count]"r"(count) 
            : "cc");
}

PUBLIC s32 timer_init()
{
    return 0;
}
