#include <libc.h>
#include "log.h"

 __attribute__((naked)) void IrqHandler()
{
    uart_printf("in General_Irq_Handler\n");
    uart_printf("in %s \n", __func__);
    while(1);
}

void General_Exc_Handler()
{
    uart_puts("abab\r\n");
    uart_printf("in %s \n", __func__);
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

void lock_irq()   
{
    u32 _cpsr = __get_cpsr();

    set_bit(&_cpsr, IRQ_DISABLE_BIT, 1); 
    asm volatile("msr CPSR_c, %[_cpsr]"
            :   
            : [_cpsr]"r"(_cpsr));
}

void unlock_irq()   
{
    u32 _cpsr = __get_cpsr();

    set_bit(&_cpsr, IRQ_DISABLE_BIT, 0); 
    asm volatile("msr CPSR_c, %[_cpsr]"
            :   
            : [_cpsr]"r"(_cpsr));
}

s32 lockup(char *file_name, char *func_name, u32 line_num, char *desc)
{
    uart_printf("lockup!\n");
    uart_printf("%s-%s-%d: %s\n", file_name, func_name, line_num, desc);
    while(1);
}
s32 int_init()
{
}
