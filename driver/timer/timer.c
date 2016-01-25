#include <libc.h>

#include <memory_map.h>
#include "timer.h"

void delay(s32 count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : 
            : [count]"r"(count) 
            : "cc");
}


#define CORETM_HZ (1000000) /* 1M, may be not very precise */
s32 timer_init()
{
    writel(CORETMLOAD, 0xFFFFFFFF);
                        /* 23-bit counter & irq enable & timer enable */
    writel(CORETMCTRL, 0x1 << 1 | 0x1 << 5 | 0x1 << 7);
}
