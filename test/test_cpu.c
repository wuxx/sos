#include <libc.h>

s32 test_cpu_all(u32 argc, char **argv)
{
    s32 ret = 0;
    u32 i;
    i = atoi(argv[2]);
    switch(i) {
        case (0):
            uart_printf("cpsr: 0x%x\n", __get_cpsr());
            break;
        default:
            return -1;
    }

    return ret;
}
