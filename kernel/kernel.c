#include <system_config.h>
#include <os_task.h>
#include <libc.h>
#include "log.h"
#include "gpio.h"

extern u8 task_stack[TASK_NR_MAX][TASK_STK_SIZE];
extern struct __task__ tcb[TASK_NR_MAX];

u32 os_tick = 0;
u32 need_schedule = 0;

void idle_task()
{
    while(1) {
        PRINT_STAMP();
        mdelay(1000);
    }
}
void blink_task()
{
    set_gpio_function(16, OUTPUT);
    while(1) {
        set_gpio_output(16, 1);     /* led off */
        mdelay(1000);
        set_gpio_output(16, 0);     /* led on */
        mdelay(1000);

    }
}


void os_clock_irq_hook(struct cpu_context *ctx)
{
    u32 old_task_id, new_task_id;

    os_tick ++ ;

    old_task_id = (ctx->r13 - (u32)task_stack) / TASK_STK_SIZE;

    /**/
    if (need_schedule) {
        new_task_id = get_task(); /* get the highest priority task in READY STATE */
        /* save the old task context */
        tcb[old_task_id].state = TASK_READY;
        memcpy(&(tcb[old_task_id].cc), ctx, sizeof(struct cpu_context));

        /* restore the new task context */
        tcb[new_task_id].state = TASK_RUNNING;
        memcpy(ctx, &(tcb[new_task_id].cc), sizeof(struct cpu_context));
    }
}

s32 os_init()
{
    PRINT_STAMP();
    if (task_create(idle_task, 0, 100) != OK) {
        PRINT_EMG("idle_task create failed !\n");
        return ERROR;
    }
    PRINT_STAMP();

    if (task_create(blink_task, 0, 100) != OK) {
        PRINT_EMG("idle_task create failed !\n");
        return ERROR;
    }
    PRINT_STAMP();
}
