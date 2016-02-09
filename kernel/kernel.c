#include <system_config.h>
#include <os_task.h>
#include <libc.h>
#include "log.h"
#include "gpio.h"

extern u8 task_stack[TASK_NR_MAX][TASK_STK_SIZE];
extern struct __task__ tcb[TASK_NR_MAX];
extern struct __task__ *old_task;
extern struct __task__ *new_task;

u32 os_init_ok = 0;
u32 os_tick = 0;
u32 need_schedule = 1;

void idle_task()
{
    while(1) {
        PRINT_STAMP();
        mdelay(1000);
    }
}
void blink_task()
{
    /* set_gpio_function(16, OUTPUT); */
    PRINT_STAMP();
    while(1) {
#if 0
        set_gpio_output(16, 1);     /* led off */
        mdelay(1000);
        set_gpio_output(16, 0);     /* led on */
#endif
        PRINT_STAMP();
        mdelay(1000);

    }
}

void dump_ctx(struct cpu_context *ctx)
{
#define DUMP_VAR(c, var) PRINT_EMG("[0x%x]:" #var "\t 0x%x\n", &c->var, c->var)
    DUMP_VAR(ctx, r13);
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
    DUMP_VAR(ctx, lr);
}

/* just re-set old_task & new_task */
void task_sched()
{
    new_task = get_task_ready(); /* get the highest priority task in READY STATE */

    old_task->state = TASK_READY;
    new_task->state = TASK_RUNNING;

    old_task = new_task;
}

void os_clock_irq_hook(struct cpu_context *ctx)
{
    u32 old_task_id, new_task_id;
    PRINT_STAMP();
    PRINT_EMG("ctx: 0x%x  %x \n", ctx, sizeof(struct cpu_context));
    dump_ctx(ctx);
    if (os_init_ok == 0) {
        memcpy(ctx, (void *)(tcb[0].sp), sizeof(struct cpu_context));  /* idle task context */
        PRINT_STAMP();
        tcb[0].state = TASK_RUNNING;
        PRINT_STAMP();
        dump_ctx(ctx);
        os_init_ok = 1;
    } else {
        os_tick ++ ;

        PRINT_STAMP();
        old_task_id = (ctx->r13 - (u32)task_stack) / TASK_STK_SIZE; /* little hack */
        PRINT_EMG("%d %x %x %x %x\n", __LINE__, old_task_id, ctx->r13, (u32)task_stack, TASK_STK_SIZE);

        if (need_schedule) {
            PRINT_STAMP();
            task_sched();
            PRINT_STAMP();

#if 0
            PRINT_STAMP();
            new_task_id = get_task_ready(); /* get the highest priority task in READY STATE */
            PRINT_EMG("%d %x \n", __LINE__, new_task_id);
            /* save the old task context */
            PRINT_STAMP();
            tcb[old_task_id].state = TASK_READY;
            memcpy((void *)(tcb[old_task_id].sp), ctx, sizeof(struct cpu_context));
            dump_ctx(ctx);

            /* restore the new task context */
            PRINT_STAMP();
            tcb[new_task_id].state = TASK_RUNNING;
            memcpy(ctx, (void *)(tcb[new_task_id].sp), sizeof(struct cpu_context));
            dump_ctx(ctx);
#endif
        }
    }
}

s32 os_init()
{
    PRINT_STAMP();
    if (task_create(idle_task, 0, 100) != OK) {
        PRINT_EMG("idle_task create failed !\n");
        return ERROR;
    }
    old_task = new_task = &tcb[0];  /* idle_task */
    PRINT_STAMP();

    if (task_create(blink_task, 0, 100) != OK) {
        PRINT_EMG("idle_task create failed !\n");
        return ERROR;
    }
    PRINT_STAMP();
}
