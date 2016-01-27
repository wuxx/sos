#include <libc.h>
#include <int.h>
#include <memory_map.h>

#include "timer.h"

#define CORETM_HZ (1000000) /* 1M, may be not very precise */
#define MS2TICK(ms)     (ms*1000)
#define TICK2MS(tick)   (tick/1000)

void delay(s32 count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : 
            : [count]"r"(count) 
            : "cc");
}

void timer_irq_handler(u32 irq_nr)
{
    uart_printf("in %s %d\n", __func__, irq_nr);
    uart_printf("%d\n", __LINE__);
    writel(CORETMCLR, 0x0);
    uart_printf("%d\n", __LINE__);
}

s32 timer_init()
{
    writel(CORETMLOAD, MS2TICK(10000)); /* 10s */
                        /* 23-bit counter & irq enable & timer enable */
    writel(CORETMCTRL, 0x1 << 1 | 0x1 << 5 | 0x1 << 7);
    request_irq(IRQ_CORE_TIMER, timer_irq_handler);
    enable_irq(IRQ_CORE_TIMER);
}
