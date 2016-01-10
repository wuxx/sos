#include <libc.h>
#include "log.h"


void General_Irq_Handler()
{
    PRINT_EMG("in %s \n", __func__);
    while(1);
}

void General_Exc_Handler()
{
    PRINT_EMG("in %s \n", __func__);
    while(1);
}

s32 request_irq(u32 irq_nr, func_1 irq_handler)
{

}

s32 release_irq(u32 irq_nr)
{

}

s32 enable_irq(u32 irq_nr)
{
}

s32 disable_irq(u32 irq_nr)
{
}


s32 int_init()
{
}
