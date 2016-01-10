#include <libc.h>
#include "log.h"


void General_Irq_Handler()
{
    PRINT_EMG("in General_Irq_Handler\n");
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

s32 lockup(char *file_name, char *func_name, u32 line_num, char *desc)
{
    PRINT_EMG("lockup!\n");
    PRINT_EMG("%s-%s-%d: %s\n", file_name, func_name, line_num, desc);
    while(1);
}
s32 int_init()
{
}
