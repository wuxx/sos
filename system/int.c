#include <libc.h>
#include <memory_map.h>
#include <os_task.h>
#include "log.h"
#include "cpu.h"

extern void dump_mem(u32 addr, u32 word_nr);

struct __task__ *old_task, *new_task;
struct cpu_context *current_context;

func_1 irq_table[IRQ_MAX] = {0};

void General_Irq_Handler()
{
    u32 i, j, cpsr;
    u32 pend[3], enable[3];
    u32 irq_nr;
    func_1 irq_func = NULL;

    cpsr = __get_cpsr();
    PRINT_DEBUG("enter %s 0x%x %s\n", __func__, cpsr, get_cpu_mode());
#if 0
    PRINT_DEBUG("\ncurrent_context: %x\n", current_context);
    dump_mem((u32)current_context, 20);
    dump_mem(VIC_BASE, 10);
#endif

    pend[0]   = readl(IRQ_PEND_BASIC);
    pend[1]   = readl(IRQ_PEND1);
    pend[2]   = readl(IRQ_PEND2);

    enable[0] = readl(IRQ_ENABLE_BASIC);
    enable[1] = readl(IRQ_ENABLE1);
    enable[2] = readl(IRQ_ENABLE2);

    for(i=0;i<3;i++) {
        for(j=0;j<32;j++) {
            if (get_bit(pend[i], j) && get_bit(enable[i], j)) {
                irq_nr = i * 32 + j;
                PRINT_DEBUG("irq_nr: %d\n", irq_nr);
                irq_func = irq_table[irq_nr];
                if (irq_func != NULL) {
                    irq_func(irq_nr);
                }
            }
        }
    }

    PRINT_DEBUG("exit %s 0x%x %s\n", __func__, cpsr, get_cpu_mode());
}

/* cpu_context save into /restore from user/system mode stack */
void cpu_context_save()
{
    old_task->sp = current_context->sp - sizeof(struct cpu_context);    /* store context in task's stack (but the task don't know) */

    memcpy((void *)(old_task->sp), (void *)current_context, sizeof(struct cpu_context));
}

void cpu_context_restore()
{
    memcpy((void *)current_context, (void *)(new_task->sp), sizeof(struct cpu_context));
}

__attribute__((naked)) void IrqHandler()
{
    asm volatile (
            "stmfd sp!, {lr}\n\t"       /* user/system pc = lr - 4  */
            "stmfd sp!, {r0-r14}^\n\t"  /* the ^ means user/system mode reg */
            "sub sp, sp, #4\n\t"        /* eh... get space to place the user/system mode cpsr, sp */

            "push {r0-r1}\n\t"

            "add r1, sp, #8\n\t"    /* (r1 = sp + 8) the context frame base. */

            "mrs r0, spsr\n\t"      /* user/system mode cpsr is backup in spsr */
            "str r0, [r1, #0x0]\n\t"

            "ldr r0, =current_context\n\t"
            "str r1, [r0]\n\t"      /* store the context frame */

            "pop  {r0-r1}\n\t"

            "bl cpu_context_save\n\t"
            : 
            : 
            : "memory"
            );

    General_Irq_Handler();

    __asm__ volatile (

            "bl cpu_context_restore\n\t"

            "pop {r0}\n\t"              /* spsr -> r0 */
            "msr SPSR_cxsf, r0\n\t"     /* restore cpsr */

            "ldmfd sp!, {r0-r14}^\n\t"
            "ldmfd sp!, {lr}\n\t"
            "subs pc, lr, #4\n\t"       /* (lr - 4) -> pc, rerun the user/system mode code */
            "nop\n\t"
            : 
            : 
            : "memory"
    );
}

void General_Exc_Handler()
{
    u32 lr, cpsr;
    cpsr = __get_cpsr();
    PRINT_EMG("in %s \n", __func__);
    PRINT_EMG("cpsr %x; %s\n", cpsr, get_cpu_mode());
    dump_ctx(current_context);
    dump_tcb_all();
    lockup();
    while(1);
}

__attribute__((naked)) void ExcHandler()
{
    asm volatile (
            "stmfd sp!, {lr}\n\t"       /* user/system pc = lr - 4  */
            "stmfd sp!, {r0-r14}^\n\t"  /* the ^ means user/system mode reg */
            "sub sp, sp, #4\n\t"        /* eh... get space to place the user/system mode cpsr, sp */

            "push {r0-r1}\n\t"

            "add r1, sp, #8\n\t"    /* (r1 = sp + 8) the context frame base. */

            "mrs r0, spsr\n\t"      /* user/system mode cpsr is backup in spsr */
            "str r0, [r1, #0x0]\n\t"

            "ldr r0, =current_context\n\t"
            "str r1, [r0]\n\t"      /* store the context frame */

            "pop  {r0-r1}\n\t"

            "bl cpu_context_save\n\t"
            : 
            : 
            : "memory"
            );
    General_Exc_Handler();
    lockup();
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
            PRINT_EMG("%s: illegal index %d\n", __func__, i);
            return -1;
    }
    
    rv = readl(enable_base);
    set_bit(&rv, offset, 1);
    writel(enable_base, rv);
}

s32 disable_irq(u32 irq_nr)
{
    u32 i, offset, disable_base;
    u32 rv;

    if (irq_nr >= IRQ_MAX) {
        return -1;
    }

    i      = irq_nr / 32; 
    offset = irq_nr % 32;

    switch (i) {
        case (0):
            disable_base = IRQ_DISABLE_BASIC;
            break;
        case (1):
            disable_base = IRQ_DISABLE1;
            break;
        case (2):
            disable_base = IRQ_DISABLE2;
            break;
        default:
            PRINT_EMG("%s: illegal index %d\n", __func__, i);
            return -1;
    }
    
    rv = readl(disable_base);
    set_bit(&rv, offset, 1);
    writel(disable_base, rv);
}

s32 disable_irq_all()
{
    writel(IRQ_DISABLE_BASIC, 0xFFFFFFFF);
    writel(IRQ_DISABLE1, 0xFFFFFFFF);
    writel(IRQ_DISABLE2, 0xFFFFFFFF);
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

s32 lockup()
{
    PRINT_EMG("lockup!\n");
    lock_irq();
    while(1);
}

s32 _assert(char *file_name, char *func_name, u32 line_num, char *desc)
{
    PRINT_EMG("[%s][%s][%d]: %s\n", file_name, func_name, line_num, desc);
    lockup();
    while(1);
}

s32 unexpect_irq_handler(u32 irq_nr)
{
    PRINT_EMG("in %s %d\n", __func__, irq_nr);
    lockup();
    while(1);
}

s32 int_init()
{
    u32 i;
    for(i=0;i<IRQ_MAX;i++) {
        request_irq(i, unexpect_irq_handler);
    }

    disable_irq_all();

    return 0;
}
