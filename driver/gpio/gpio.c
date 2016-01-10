#include <libc.h>
#include <memory_map.h>

void set_gpio_function(u32 gpio, u32 function)
{
    u32 offset = 0;
    u32 offset0 = 0;
    u32 offset1 = 0;
    u32 mask = 0;
    u32 tmp = 0;
    if (gpio > 53 || function > 7) {
        return;
    }
    
    offset0 = gpio / 10;
    offset1 = gpio % 10;
    offset1 = offset1 * 3;

    tmp = *((u32*)GPIO_BASE + offset0);
    mask = 0x7 << offset1;
    mask = ~mask;
    tmp &= mask;
    tmp |= (function << offset1);
    *((u32*)GPIO_BASE + offset0) = tmp;
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

