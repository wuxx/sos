#include <libc.h>
#include <int.h>
#include <memory_map.h>
#include <system_config.h>

#include "log.h"
#include "timer.h"

/* increment syscounter */
u64 get_syscounter()
{
    u64 sc;
    u32 clo, chi;
    clo = readl(SYSTMCLO);
    chi = readl(SYSTMCHI);
    sc  = (chi << 32) | clo;
    return sc;
}

void udelay(u32 us)
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

void mdelay(u32 ms)
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

void delay(s32 count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : 
            : [count]"r"(count) 
            : "cc");
}

void timer_irq_handler(u32 irq_nr)
{
    PRINT_DEBUG("in %s %d\n", __func__, irq_nr);
    writel(CORETMCLR, 0x0);
}

s32 timer_init()
{
    /* core timer */
    writel(CORETMLOAD, MS2TICK(1000/OS_HZ));
                        /* 23-bit counter & irq enable & timer enable */
    writel(CORETMCTRL, 0x1 << 1 | 0x1 << 5 | 0x1 << 7);
    request_irq(IRQ_CORE_TIMER, timer_irq_handler);
    enable_irq(IRQ_CORE_TIMER);
}

