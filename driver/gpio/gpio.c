#include <libc.h>
#include <memory_map.h>
#include "mmio.h"
#include "log.h"
#include "gpio.h"

PUBLIC s32 gpio_set_function(u32 gpio_index, u32 func_index)
{
    u32 i, fsel_addr;
    union gpio_fsel_reg fr;


    if ((gpio_index >= GPIO_NR_MAX) || func_index >= ALT_FUNC_MAX) {
        PRINT_EMG("invalid para %d %d\n", gpio_index, func_index);
        return -1;
    }

    /* which reg */
    fsel_addr = GPFSEL0 + 4*(gpio_index / 10);
    i = gpio_index % 10;

    fr.value = readl(fsel_addr);
#if 0
    PRINT_EMG("%d %x %x\n", i, fr.value, func_index);
#endif
    switch (i) {
        case (0):
            fr.reg.fsel0 = func_index;
            break;
        case (1):
            fr.reg.fsel1 = func_index;
            break;
        case (2):
            fr.reg.fsel2 = func_index;
            break;
        case (3):
            fr.reg.fsel3 = func_index;
            break;
        case (4):
            fr.reg.fsel4 = func_index;
            break;
        case (5):
            fr.reg.fsel5 = func_index;
            break;
        case (6):
            fr.reg.fsel6 = func_index;
            break;
        case (7):
            fr.reg.fsel7 = func_index;
            break;
        case (8):
            fr.reg.fsel8 = func_index;
            break;
        case (9):
            fr.reg.fsel9 = func_index;
            break;
        default:
            break;
    }
#if 0
    PRINT_EMG(" 0x%x -> [0x%x] \n", fr.value, fsel_addr);
#endif
    writel(fsel_addr, fr.value);
    return 0;
}

PUBLIC u32 gpio_get_function(u32 gpio_index)
{
    u32 i, fsel_addr;
    u32 rvalue;
    u32 func_index;


    if (gpio_index >= GPIO_NR_MAX) {
        PRINT_EMG("invalid para %d \n", gpio_index);
        return -1;
    }

    /* which reg */
    fsel_addr = GPFSEL0 + 4*(gpio_index / 10);
    i = gpio_index % 10;

    rvalue = readl(fsel_addr);
#if 0
    PRINT_EMG("%d %x %x\n", i, fr.value, func_index);
#endif
    func_index = (rvalue >> (3 * i)) & 0x7;
    return func_index;
}

PUBLIC s32 gpio_set_output(u32 gpio_index, u32 bit)
{
    u32 output_set_addr, output_clear_addr;
    u32 bit_offset;

    if ((gpio_index >= GPIO_NR_MAX) || bit > 1) {
        PRINT_EMG("invalid para %d %d\n", gpio_index, bit);
        return -1;
    }

    bit_offset        = gpio_index % 32;
    output_set_addr   = GPSET0 + gpio_index / 32;
    output_clear_addr = GPCLR0 + gpio_index / 32;

    /* PRINT_DEBUG("%s 0x%x 0x%x [0x%x] [0x%x]\n", __func__, bit, bit_offset, output_set_addr, output_clear_addr); */

    if (bit == 0) {
        writel(output_clear_addr, 0x1 << bit_offset);
    } else {
        writel(output_set_addr, 0x1 << bit_offset);
    }

    return 0;
}
