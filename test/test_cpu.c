#include <libc.h>
#include <memory_map.h>
#include "log.h"
#include "cpu.h"

s32 test_cpu_all(u32 argc, char **argv)
{
    s32 ret = 0;
    u32 i;
    i = atoi(argv[2]);
    switch(i) {
        case (0):
            dump_mem(IRQ_BASE, 10);
            break;
        case (1):
            PRINT_EMG("pc:   0x%x\n", __get_pc());
            PRINT_EMG("lr:   0x%x\n", __get_lr());
            PRINT_EMG("cpsr: 0x%x\n", __get_cpsr());
            break;
        default:
            return -1;
    }

    return ret;
}
