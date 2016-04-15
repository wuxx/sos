#include <memory_map.h>
#include <libc.h>
#include "gpio.h"
#include "log.h"
#include "systest.h"

/* GPIO17 -> GPIO_GEN0 -> 11 */
/* GPIO18 -> GPIO_GEN1 -> 12 */

s32 test_gpio_irq_handler(u32 irq_nr)
{
    u32 stat;
    PRINT_EMG("in %s %d\n", __func__, irq_nr);
    dump_mem(GPIO_BASE, 40);

    stat = readl(GPEDS0);
    writel(GPEDS0, stat);

    stat = readl(GPEDS1);
    writel(GPEDS1, stat);
    return 0;
}

s32 test_gpio_all(u32 argc, char **argv)
{
    s32 ret = 0;
    u32 i, arg1, arg2;
    i    = atoi(argv[2]);
    arg1 = atoi(argv[3]);
    arg2 = atoi(argv[4]);

    PRINT_EMG("i: %x; arg1: %x; arg2: %x\n", i, arg1, arg2);
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
        case (3):
            set_gpio_function(17, OUTPUT);
            set_gpio_output(17, 1);

            set_gpio_function(18, INPUT);
            writel(GPREN0,  1 << 18);   /* rise trigger int */
#if 0
            writel(GPHEN0,  1 << 18);   /* high trigger int */
            writel(GPFEN0,  1 << 18);
            writel(GPLEN0,  1 << 18);
            writel(GPAREN0, 1 << 18);
            writel(GPAFEN0, 1 << 18);
#endif

            request_irq(IRQ_GPIO0, test_gpio_irq_handler);
            request_irq(IRQ_GPIO1, test_gpio_irq_handler);
            request_irq(IRQ_GPIO2, test_gpio_irq_handler);
            request_irq(IRQ_GPIO3, test_gpio_irq_handler);
            enable_irq(IRQ_GPIO0);
            enable_irq(IRQ_GPIO1);
            enable_irq(IRQ_GPIO2);
            enable_irq(IRQ_GPIO3);
            break;
        default:
            return -1;
    }

    return ret;
}
