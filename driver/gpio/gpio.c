#include <libc.h>
#include <memory_map.h>
#include "mmio.h"
#include "gpio.h"

s32 set_gpio_function(u32 gpio_index, u32 func_index)
{
    u32 i, fsel_addr;
    union gpio_fsel_reg fr;


    if ((gpio_index >= GPIO_NR_MAX) || func_index >= ALT_FUNC_MAX) { 
        uart_printf("invalid para %d %d\n", gpio_index, func_index);
        return -1;
    }

    /* which reg */
    fsel_addr = GPFSEL0 + 4*(gpio_index / 10);
    i = gpio_index % 10;

    fr.value = readl(fsel_addr);
    uart_printf("%d %x\n", i, fr.value);
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
    uart_printf(" 0x%x -> [0x%x] \n", fr.value, fsel_addr);
    writel(fr.value, fsel_addr);
    return 0;
}

s32 set_gpio_output(u32 gpio_index, u32 bit)
{
    u32 output_set_addr, output_clear_addr;
    u32 bit_offset;

    if ((gpio_index >= GPIO_NR_MAX) || bit > 1) { 
        uart_printf("invalid para %d %d\n", gpio_index, bit);
        return -1;
    }

    bit_offset        = gpio_index % 32;
    output_set_addr   = GPSET0 + gpio_index / 32;
    output_clear_addr = GPCLR0 + gpio_index / 32;

    uart_printf(" 0x%x [0x%x] [0x%x]\n", bit, output_set_addr, output_clear_addr);

    if (bit == 0) {
        writel(0x1 << bit_offset, output_clear_addr);
    } else {
        writel(0x1 << bit_offset, output_set_addr);
    }

    return 0;
}

void bcm2835_gpio_fnsel(u32 gpio_pin, u32 gpio_fn)
{
    u32 gpfnbank = gpio_pin/10;
    u32 offset = (gpio_pin - (gpfnbank * 10)) * 3;
    volatile u32 *gpfnsel = GPIO_BASE + gpfnbank;
    *gpfnsel &= ~(0x07 << offset);
    *gpfnsel |= (gpio_fn << offset);
}

void set_gpio_value(u32 gpio, u32 val)
{
    u32 offset0 = 0;
    u32 offset1 = 0;
    u32 base = 0;
    u32 tmp = 0;
    u32 mask = 0;
    if (gpio > 53) {
        return;
    }
    
    switch(val) {
        case (0):
            base = 10;  /* offset 40  output 0 */
            break;

        case (1):
            base = 7;   /* offset 28 output 1 */
            break;

        default:
            base = 7;
            val = 1;
            break;
    }

    offset0 = gpio / 32;
    offset1 = gpio % 32;

    tmp = *((u32 *)GPIO_BASE + base + offset0);

    tmp |= (1 << offset1);
    *((u32 *)GPIO_BASE + base + offset0) = tmp;

}

void delay_gpio()
{
    u32 count = 400000;
    while(count--);
}

