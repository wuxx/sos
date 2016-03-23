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

PRIVATE s32 idle_task(u32 arg)
{
    if (idle_init == 0) {
        os_ready_delete(&tcb[0]);
        extern s32 dump_list();
        dump_list();
        idle_init = 1;
        unlock_irq();   /* kick off the system */
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

PRIVATE struct __os_task__ * need_schedule()
{
    struct __os_task__ *best_task;

    best_task = get_task_ready(); /* get the highest priority task in READY STATE */
    if (best_task->prio <= current_task->prio || /* current_task create a higher prio task  */
        current_task->state == TASK_UNUSED    || /* current_task self-destruction  */
        current_task->state == TASK_SLEEP        /* current_task invoke os_sleep */
        ) {
        PRINT_DEBUG("schedule task %d \n", best_task->id);
        return best_task;
    }
    return NULL;
}

/*
   1. update current_task
   2. delete the current_task from os_ready_list
   3. insert old_task into os_sleep_list or os_ready_list or sem_list.
*/
PRIVATE void task_sched(struct __os_task__ *best_task)
{
    struct __os_task__ *old_task;

    old_task     = current_task;
    current_task = best_task;

    current_task->state = TASK_RUNNING;
    os_ready_delete(best_task);

    if (old_task->state == TASK_UNUSED) {       /* self-destruction */

    } else if (old_task->state == TASK_SLEEP) { /* current_task invoke os_sleep */
        os_sleep_insert(old_task);
    } else {
        old_task->state = TASK_READY;
        os_ready_insert(old_task);
    }

    /* dump_list(); */
    PRINT_DEBUG("schedule %d \n", current_task->id);
}

PRIVATE void os_clock_irq_hook(struct cpu_context *ctx)
{
    struct __os_task__ *best_task;

    os_tick ++ ;
    
    os_sleep_expire();
    if ((best_task = need_schedule()) != NULL) {
        task_sched(best_task);
    }
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
    if (task_create(idle_task, 0, 100) != OK) {
        PRINT_EMG("idle_task create failed !\n");
        return ERROR;
    }
    current_task = &tcb[0];  /* idle_task */
    PRINT_STAMP();
}
