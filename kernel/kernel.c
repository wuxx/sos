#include <system_config.h>
#include <os_task.h>
#include <libc.h>
#include "log.h"

extern u8 task_stack[TASK_NR_MAX][TASK_STK_SIZE];
extern struct __task__ tcb[TASK_NR_MAX];

u32 os_tick = 0;
u32 need_schedule = 0;

void idle_task()
{
    while(1) {
        PRINT_STAMP();
        mdelay(1);
    }
}


void os_clock_irq_hook(struct cpu_context *ctx)
{
    u32 task_id;

    os_tick ++ ;

    task_id = (ctx->r13 - (u32)task_stack) / TASK_STK_SIZE;

    /* save the old task context */
    tcb[task_id].state = TASK_READY;
    memcpy(&(tcb[task_id].cc), ctx, sizeof(struct cpu_context));

    /* restore the new task context */
    tcb[task_id].state = TASK_READY;
    memcpy(&(tcb[task_id].cc), ctx, sizeof(struct cpu_context));
}

s32 os_init()
{
    if (task_create(idle_task, 0) != OK) {
        PRINT_EMG("idle_task create failed !\n");
        return ERROR;
    }
}
