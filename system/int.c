#include <libc.h>
#include <memory_map.h>
#include <os_task.h>
#include "log.h"
#include "cpu.h"
#include "syscall.h"

extern void  dump_mem(u32 addr, u32 word_nr);
extern char* get_cpu_mode(u32 *m);
extern void dump_tcb_all();

extern struct __syscall__ syscall_table[];

struct __os_task__ *current_task;
struct cpu_context *current_context;

func_1 irq_table[IRQ_MAX] = {0};

PUBLIC void dump_ctx(struct cpu_context *ctx)
{
#define DUMP_VAR(c, var) PRINT_EMG("[0x%x]:" #var "\t 0x%x\n", &c->var, c->var)
    DUMP_VAR(ctx, cpsr);
    DUMP_VAR(ctx, r0);
    DUMP_VAR(ctx, r1);
    DUMP_VAR(ctx, r2);
    DUMP_VAR(ctx, r3);
    DUMP_VAR(ctx, r4);
    DUMP_VAR(ctx, r5);
    DUMP_VAR(ctx, r6);
    DUMP_VAR(ctx, r7);
    DUMP_VAR(ctx, r8);
    DUMP_VAR(ctx, r9);
    DUMP_VAR(ctx, r10);
    DUMP_VAR(ctx, r11);
    DUMP_VAR(ctx, r12);
    DUMP_VAR(ctx, sp);
    DUMP_VAR(ctx, lr);
    DUMP_VAR(ctx, pc);
}

PUBLIC char* get_cpu_mode(u32 *m)
{
    u32 cpsr, mode;
    cpsr = __get_cpsr();
    mode = cpsr & 0x1f;

    if (m != NULL) {
        *m = mode;
    }

    switch (mode) {
        case (16):
            return "user mode";
        case (17):
            return "fiq mode";
        case (18):
            return "irq mode";
        case (19):
            return "supervisor mode";
        case (22):
            return "secmonitor mode";
        case (23):
            return "abort mode";
        case (27):
            return "undefined mode";
        case (31):
            return "system mode";
        default:
            return "unknown mode";
    }
}

PUBLIC u32 in_interrupt()
{
    u32 mode;
    get_cpu_mode(&mode);
    if (mode == MODE_IRQ || mode == MODE_FIQ) {
        return 1;
    } else {
        return 0;
    }
}

PRIVATE void General_Irq_Handler()
{
    u32 i, j;
    u32 pend[3], enable[3];
    u32 irq_nr;
    func_1 irq_func = NULL;

#if 0
    u32 cpsr;
    cpsr = __get_cpsr();
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

PRIVATE void General_Exc_Handler()
{
    u32 cpsr, nr, mode;
    s32 ret;
    u32 args[4]; /* r0-r3 */
    struct __os_task__ *ptask;
    struct cpu_context *pctx;

    ptask = current_task;
    cpsr  = __get_cpsr();
    pctx  = (struct cpu_context *)(ptask->sp);

    PRINT_DEBUG("in %s \n", __func__);
    PRINT_DEBUG("cpsr %x; %s\n", cpsr, get_cpu_mode(&mode));
    /* dump_ctx(current_context); */

    args[0] = current_context->r0;
    args[1] = current_context->r1;
    args[2] = current_context->r2;
    args[3] = current_context->r3;

    if (mode == MODE_SVC) {
        nr  = readl(current_context->pc - 8) & 0xFFFFFF;
        ret = system_call(nr, args);

        pctx->r0  = ret;
    } else {
        PRINT_EMG("in %s \n", __func__);
        panic();
        while(1);

    }
}

__attribute__((naked)) void ExcHandler()
{
    asm volatile ( "add lr, lr, #4" ); /* irq mode lr = user/system mode pc + 4, but in svc mode lr = user/system mode pc   */
    asm volatile (
            "stmfd sp!, {lr}\n\t"
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
    return 0;
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
    return 0;
}

PUBLIC s32 disable_irq_all()
{
    writel(IRQ_DISABLE_BASIC, 0xFFFFFFFF);
    writel(IRQ_DISABLE1, 0xFFFFFFFF);
    writel(IRQ_DISABLE2, 0xFFFFFFFF);
    return 0;
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
    u32 lr = __get_lr();
    PRINT_EMG("in %s, cpu_mode: [%s]; lr: [%x]; current_task_id: %d\n\n", 
            __func__, get_cpu_mode(NULL), lr, current_task != NULL ? current_task->id: -1);
    dump_ctx(current_context);
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

PUBLIC s32 _assert(const char *file_name, const char *func_name, u32 line_num, char *desc)
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
