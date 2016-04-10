#include <libc.h>
#include <memory_map.h>
#include "log.h"
#include "cpu.h"
#include "systest.h"

static inline void swi(u32 i)
{
#if 1
    asm volatile (
            "swi 0x1\n\t"
            :
            :
            :
            );
#else
    u32 word;
    func_0 _swi;
    word = 0xef << 24 | i & 0xFFFFFFFF ;
    _swi = (func_0)word;
    _swi();
#endif
}

s32 test_cpu_all(u32 argc, char **argv)
{
    s32 ret = 0;
    u32 i, arg1;
    func_0 func;

    i = atoi(argv[2]);
    arg1 = atoi(argv[3]);

    PRINT_EMG("arg1: %x\n", arg1);

    switch(i) {
        case (0):
            dump_mem(IRQ_BASE, 10);
            break;
        case (1):
            PRINT_EMG("pc:   0x%x\n", __get_pc());
            PRINT_EMG("lr:   0x%x\n", __get_lr());
            PRINT_EMG("sp:   0x%x\n", __get_sp());
            PRINT_EMG("cpsr: 0x%x\n", __get_cpsr());
            break;
        case (2):
            __set_pc(arg1);
            break;
        case (3):
            __set_sp(arg1);
            break;
        case (4):
            __set_cpsr(arg1);
            break;
        case (100): /* illegal instruction */
            arg1 = 0x12345678;
            func = (func_0)arg1;
            func();
            break;
        case (200): /* enable irq */
            enable_irq(arg1);
            break;
        case (201): /* disable irq */
            disable_irq(arg1);
            break;
        case (300): /* swi */
            swi(arg1);
            break;
        default:
            return -1;
    }

    return ret;
}
