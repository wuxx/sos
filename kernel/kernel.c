#include <system_config.h>
#include <os_task.h>
#include <libc.h>

#include "log.h"
#include "gpio.h"

/* u32 os_init_ok = 0; */
u32 os_tick = 0;

void idle_task()
{
    while(1) {
        PRINT_INFO("in %s\n", __func__);
        mdelay(1000);
    }
}

void blink_task()
{
    set_gpio_function(16, OUTPUT);
    while(1) {
        PRINT_INFO("in %s\n", __func__);
        set_gpio_output(16, 1);     /* led off */
        mdelay(1000);
        set_gpio_output(16, 0);     /* led on */
        mdelay(1000);

    }
}

void dump_ctx(struct cpu_context *ctx)
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

u32 need_schedule()
{
    new_task = get_task_ready(); /* get the highest priority task in READY STATE */
    if ((new_task != NULL) && (new_task->prio <= old_task->prio)) {
        return 1;
    } else {
        return 0;
    }
}

/* just re-set old_task & new_task */
void task_sched()
{

    old_task->state = TASK_READY;
    new_task->state = TASK_RUNNING;

    old_task = new_task;
}

void os_clock_irq_hook(struct cpu_context *ctx)
{
    u32 old_task_id, new_task_id;
    os_tick ++ ;

    if (need_schedule()) {
        task_sched();
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
        PRINT_EMG("blink_task create failed !\n");
        return ERROR;
    }
    PRINT_STAMP();
}
