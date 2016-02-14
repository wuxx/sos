#include <memory_map.h>
#include <libc.h>
#include "timer.h"
#include "log.h"

void systimer_irq_handler(u32 irq_nr)
{
    PRINT_EMG("in %s %d \n", __func__, irq_nr);
    u32 clear = 0;
    switch (irq_nr) {
        case (IRQ_SYS_TIMER0):
            set_bit(&clear, 0, 1);
            break;
        case (IRQ_SYS_TIMER1):
            set_bit(&clear, 1, 1);
            break;
        case (IRQ_SYS_TIMER2):
            set_bit(&clear, 2, 1);
            break;
        case (IRQ_SYS_TIMER3):
            set_bit(&clear, 3, 1);
            break;
        default:
            PRINT_EMG("unknown irq %d \n", irq_nr);
            lockup();
            break;
    }
    writel(SYSTMCS, clear);
}

void sys_timer_init()
{
    writel(SYSTMCS, 0xF);
}

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
