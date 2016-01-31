#include <memory_map.h>
#include <libc.h>
#include "gpio.h"

s32 test_gpio_all(u32 argc, char **argv)
{
    s32 ret = 0;
    u32 i, arg1, arg2;
    i    = atoi(argv[2]);
    arg1 = atoi(argv[3]);
    arg2 = atoi(argv[4]);
    switch(i) {
        case (0):
            dump_mem(GPIO_BASE, 40);
            break;
        case (1):
            ret = set_gpio_function(arg1, arg2);
            break;
        case (2):
            ret = set_gpio_output(arg1, arg2);
            break;
        default:
            return -1;
    }

    return ret;
}
