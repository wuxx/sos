#include <libc.h>
#include <memory_map.h>
#include "log.h"

extern void dump_mem(u32 addr, u32 word_nr);

func_1 irq_table[IRQ_MAX] = {0};
char *mode[32] =  {"unknown mode", "unknown mode", "unknown mode",    "unknown mode",
                   "unknown mode", "unknown mode", "unknown mode",    "unknown mode",
                   "unknown mode", "unknown mode", "unknown mode",    "unknown mode",
                   "unknown mode", "unknown mode", "unknown mode",    "unknown mode",
                   "user mode",    "fiq mode",     "irq mode",        "supervisor mode",
                   "unknown mode", "unknown mode", "secmonitor mode", "abort mode",
                   "unknown mode", "unknown mode", "unknown mode", "undefined mode",
                   "unknown mode", "unknown mode", "unknown mode", "system mode",
};

void General_Irq_Handler()
{
    u32 i, pend, cpsr;
    u32 irq_nr;
    func_1 irq_func = NULL;
    cpsr = __get_cpsr();

    uart_printf("in %s 0x%x %s\n", __func__, cpsr, mode[cpsr & 0x1f]);
    dump_mem(0x2000B200, 10);

    pend = readl(IRQ_PEND_BASIC);
    for(i=0;i<32;i++) {
        if (get_bit(pend, i) == 1) {
            irq_nr = i;
            uart_printf("irq_nr: %d \n", i);
        }
    }

    pend = readl(IRQ_PEND1);
    for(i=0;i<32;i++) {
        if (get_bit(pend, i) == 1) {
            irq_nr = 32+i;
            uart_printf("irq_nr: %d \n", 32+i);
        }
    }

    pend = readl(IRQ_PEND2);
    for(i=0;i<32;i++) {
        if (get_bit(pend, i) == 1) {
            irq_nr = 64+i;
            uart_printf("irq_nr: %d \n", 64+i);
        }
    }

    dump_mem(0x2000B200, 10);
    irq_func = irq_table[irq_nr];
    if (irq_func != NULL) {
        irq_func(irq_nr);
    }
}

__attribute__((naked)) void IrqHandler()
{
    asm volatile ("stmfd    sp!, {r0-r3, r12, lr}" : : : "memory");
    General_Irq_Handler();
    __asm__ volatile (  \
            "ldmfd sp!, {r0-r3, r12, lr}\n\t"   \
            "subs pc, lr, #4\n\t"               \
            "NOP\n\t"                           \
    );
}

void General_Exc_Handler()
{
    u32 lr, cpsr;
    cpsr = __get_cpsr();
    uart_printf("in %s \n", __func__);
    uart_printf("cpsr %x; %s\n", cpsr, mode[cpsr & 0x1f]);
    while(1);
}

s32 request_irq(u32 irq_nr, func_1 irq_handler)
{
    if (irq_nr >= IRQ_MAX) {
        return -1;
    }

    irq_table[irq_nr] = irq_handler;
    return 0;
}

s32 release_irq(u32 irq_nr)
{
    if (irq_nr >= IRQ_MAX) {
        return -1;
    }

    irq_table[irq_nr] = NULL;
    return 0;

}

s32 enable_irq(u32 irq_nr)
{
    u32 i, offset, enable_base;
    u32 rv;
    if (irq_nr >= IRQ_MAX) {
        return -1;
    }

    i      = irq_nr / 32; 
    offset = irq_nr % 32;

    switch (i) {
        case (0):
            enable_base = IRQ_ENABLE_BASIC;
            break;
        case (1):
            enable_base = IRQ_ENABLE1;
            break;
        case (2):
            enable_base = IRQ_ENABLE2;
            break;
        default:
            uart_printf("%s: illegal index %d\n", __func__, i);
            return -1;
    }
    
    rv = readl(enable_base);
    set_bit(&rv, offset, 1);
    writel(enable_base, rv);
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

s32 unexpect_irq_handler(u32 irq_nr)
{
    uart_printf("in %s %d\n", __func__, irq_nr);
    while(1);
}

s32 int_init()
{
    u32 i;
    for(i=0;i<IRQ_MAX;i++) {
        request_irq(i, unexpect_irq_handler);
    }

    return 0;
}
