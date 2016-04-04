#include <system_config.h>
#include <libc.h>
#include <memory_map.h>
#include <os.h>

#include "cpu.h"
#include "timer.h"
#include "log.h"
#include "gpio.h"

extern struct cpu_context *current_context;

volatile u32 os_tick = 0;
volatile u32 idle_init = 0;

/*PRIVATE*/ s32 idle_task(u32 arg)
{
    if (idle_init == 0) {
        extern s32 dump_list();
        dump_list();
        idle_init = 1;
        unlock_irq();   /* kick off the system, will switch to the main_task */
    }

    while(1) {
        PRINT_INFO("in %s\n", __func__);
        mdelay(1000);
    }
    return 0;
}

PUBLIC s32 os_sleep(u32 ms)
{

    u32 ticks = (ms * OS_HZ) / 1000;
    current_task->sleep_ticks = ticks;
    current_task->state == TASK_SLEEP;
    /* FIXME: need do task switch immediately */
    return 0;
}

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

/*
   1. update current_task
   2. delete the current_task from os_ready_list
   3. insert old_task into os_sleep_list or os_ready_list or sem_list.
*/
PRIVATE void task_sched(struct __os_task__ *best_task)
{
    struct __os_task__ *old_task;
    struct __os_semaphore__ *psem;

    old_task     = current_task;
    current_task = best_task;

    current_task->state = TASK_RUNNING;
    os_ready_delete(best_task);

    switch (old_task->state) {
        case (TASK_UNUSED):     /* current task self-destruction */
            break;
        case (TASK_RUNNING):    /* current task create higher prio task  */
            old_task->state = TASK_READY;
            os_ready_insert(old_task);
            break;
        case (TASK_SLEEP):      /* current task invoke os_task_sleep sleep */
            os_sleep_insert(old_task);
            break;
        case (TASK_WAIT_SEM):   /* current task wait for sem */
            psem = (struct __os_semaphore__ *)(current_task->private_data);
            os_sem_insert(psem, current_task);
            break;
        default:
            kassert("%x \n", old_task->state);
    }

    /* dump_list(); */
    PRINT_DEBUG("schedule %d \n", current_task->id);
}

PRIVATE void os_clock_irq_hook(struct cpu_context *ctx)
{
    struct __os_task__ *best_task;

    os_tick ++ ;
    
    os_sleep_expire();

    task_dispatch();
}

PRIVATE s32 coretimer_irq_handler(u32 irq_nr)
{
    PRINT_DEBUG("in %s %d\n", __func__, irq_nr);
    os_clock_irq_hook(current_context);
    writel(CORETMCLR, 0x0);
    return 0;
}

PRIVATE s32 coretimer_init()
{
    /* core timer */
    writel(CORETMLOAD, MS2TICK(1000/OS_HZ));
                        /* 23-bit counter & irq enable & timer enable */
    writel(CORETMCTRL, 0x1 << 1 | 0x1 << 5 | 0x1 << 7);
    request_irq(IRQ_CORE_TIMER, coretimer_irq_handler);
    enable_irq(IRQ_CORE_TIMER);
}

PUBLIC s32 os_init()
{
    coretimer_init();
    PRINT_STAMP();
    if (task_create(idle_task, 0, 100) != 0) {
        PRINT_EMG("idle_task create failed !\n");
        return ERROR;
    }
    current_task = &tcb[0];  /* idle_task */
    PRINT_STAMP();
    /* FIXME: create main() task */
}

