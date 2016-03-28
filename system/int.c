#include <libc.h>
#include <memory_map.h>
#include <os_task.h>
#include "log.h"
#include "cpu.h"
#include "syscall.h"

extern void dump_mem(u32 addr, u32 word_nr);
extern struct __syscall__ syscall_table[];

struct __os_task__ *current_task;
struct cpu_context *current_context;

func_1 irq_table[IRQ_MAX] = {0};

PUBLIC void General_Irq_Handler()
{
    u32 i, j, cpsr;
    u32 pend[3], enable[3];
    u32 irq_nr;
    func_1 irq_func = NULL;

    cpsr = __get_cpsr();
#if 0
    PRINT_DEBUG("enter %s 0x%x %s\n", __func__, cpsr, get_cpu_mode());
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
                /* PRINT_DEBUG("irq_nr: %d\n", irq_nr); */
                irq_func = irq_table[irq_nr];
                if (irq_func != NULL) {
                    irq_func(irq_nr);
                }
            }
        }
    }

    /* PRINT_DEBUG("exit %s 0x%x %s\n", __func__, cpsr, get_cpu_mode()); */
}

/* cpu_context save into /restore from user/system mode stack */
PRIVATE void cpu_context_save()
{
    current_task->sp = current_context->sp - sizeof(struct cpu_context);    /* store context in task's stack (but the task don't know) */

    memcpy((void *)(current_task->sp), (void *)current_context, sizeof(struct cpu_context));
    PRINT_DEBUG("save %d \n", current_task->id);
    /* dump_ctx((struct cpu_context *)(current_task->sp)); */
}

PRIVATE void cpu_context_restore()
{
    PRINT_DEBUG("restore %d \n", current_task->id); 
    /* dump_ctx((struct cpu_context *)(current_task->sp)); */
    memcpy((void *)current_context, (void *)(current_task->sp), sizeof(struct cpu_context));
}

/* when irq happen, = user/system mode [pc] +4 -> irq mode [lr]  */
__attribute__((naked)) void IrqHandler()
{
    asm volatile (  /* cpu context save, please check the struct cpu_context */
            "stmfd sp!, {lr}\n\t"       /* user/system pc = irq lr - 4  */
            "stmfd sp!, {r0-r14}^\n\t"  /* the ^ means user/system mode reg */
            "sub sp, sp, #4\n\t"        /* eh... get space to place the user/system mode cpsr, sp */

            "push {r0-r1}\n\t"

            "add r1, sp, #8\n\t"        /* (r1 = sp + 8) the context frame base. */

            "mrs r0, spsr\n\t"          /* user/system mode cpsr is backup in spsr */
            "str r0, [r1, #0x0]\n\t"    /* store the cpsr */

            "ldr r0, =current_context\n\t"
            "str r1, [r0]\n\t"          /* store the context frame point in current_context */

            "pop  {r0-r1}\n\t"

            "bl cpu_context_save\n\t"
            :
            :
            : "memory"
            );

    General_Irq_Handler();

    __asm__ volatile (  /* cpu context restore */

            "bl cpu_context_restore\n\t"

            "pop {r0}\n\t"              /* spsr -> r0 */
            "msr SPSR_cxsf, r0\n\t"     /* ready to restore cpsr */

            "ldmfd sp!, {r0-r14}^\n\t"  /* restore user/system mode r0-r14 */
            "ldmfd sp!, {lr}\n\t"       /*  user/system mode pc -> irq mode lr */
            "subs pc, lr, #4\n\t"       /* (lr - 4) -> pc, launching into the user/system mode code */
            "nop\n\t"
            :
            :
            : "memory"
    );
}

PUBLIC void General_Exc_Handler()
{
    u32 cpsr, syscall_nr, mode;
    s32 ret;
    struct __os_task__ *ptask;
    struct cpu_context *pctx;

    ptask = current_task;
    cpsr  = __get_cpsr();
    pctx  = (struct cpu_context *)(ptask->sp);
    pctx->pc += 4;  /* assume that we are in a irq */

    PRINT_EMG("in %s \n", __func__);
    PRINT_EMG("cpsr %x; %s\n", cpsr, get_cpu_mode(&mode));
    dump_ctx(current_context);
    if (mode == MODE_SVC) {
        syscall_nr = readl(current_context->pc - 4) & 0xFFFFFF;
        PRINT_STAMP();
        ret = syscall_table[syscall_nr].handler(current_context->r0);   /* may be invoke task_dispatch */
        PRINT_STAMP();

        pctx->r0  = ret;
        current_context->pc += 4;
    } else {
        panic();
        while(1);

    }
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
    __asm__ volatile (

            "bl cpu_context_restore\n\t"

            "pop {r0}\n\t"              /* spsr -> r0 */
            "msr SPSR_cxsf, r0\n\t"     /* restore cpsr */

            "ldmfd sp!, {r0-r14}^\n\t"
            "ldmfd sp!, {lr}\n\t"       /* user/system mode pc -> irq mode lr */
            "subs pc, lr, #4\n\t"       /* (lr - 4) -> pc, rerun the user/system mode code */
            "nop\n\t"
            :
            :
            : "memory"
    );

}

PUBLIC s32 request_irq(u32 irq_nr, func_1 irq_handler)
{
    if (irq_nr >= IRQ_MAX) {
        return -1;
    }

    irq_table[irq_nr] = irq_handler;
    return 0;
}

PUBLIC s32 release_irq(u32 irq_nr)
{
    if (irq_nr >= IRQ_MAX) {
        return -1;
    }

    irq_table[irq_nr] = NULL;
    return 0;

}

PUBLIC s32 enable_irq(u32 irq_nr)
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

PUBLIC s32 disable_irq(u32 irq_nr)
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

PUBLIC s32 disable_irq_all()
{
    writel(IRQ_DISABLE_BASIC, 0xFFFFFFFF);
    writel(IRQ_DISABLE1, 0xFFFFFFFF);
    writel(IRQ_DISABLE2, 0xFFFFFFFF);
}

PUBLIC void lock_irq()
{
    u32 _cpsr = __get_cpsr();

    set_bit(&_cpsr, IRQ_DISABLE_BIT, 1);
    asm volatile("msr CPSR_c, %[_cpsr]"
            :
            : [_cpsr]"r"(_cpsr));
}

PUBLIC void unlock_irq()
{
    u32 _cpsr = __get_cpsr();

    set_bit(&_cpsr, IRQ_DISABLE_BIT, 0);
    asm volatile("msr CPSR_c, %[_cpsr]"
            :
            : [_cpsr]"r"(_cpsr));
}

PUBLIC s32 panic()
{
    dump_tcb_all();
    lockup();
    while(1);
}

PUBLIC s32 lockup()
{
    PRINT_EMG("lockup!\n");
    lock_irq();
    while(1);
}

PUBLIC s32 _assert(char *file_name, char *func_name, u32 line_num, char *desc)
{
    PRINT_EMG("[%s][%s][%d]: %s\n", file_name, func_name, line_num, desc);
    lockup();
    while(1);
}

PRIVATE s32 unexpect_irq_handler(u32 irq_nr)
{
    PRINT_EMG("in %s %d\n", __func__, irq_nr);
    panic();
    while(1);
}

PUBLIC s32 int_init()
{
    u32 i;
    for(i=0;i<IRQ_MAX;i++) {
        request_irq(i, unexpect_irq_handler);
    }

    disable_irq_all();

    return 0;
}
