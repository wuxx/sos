#include <libc.h>
#include <system_config.h>
#include "task.h"
#include "log.h"

struct __task__ tcb[TASK_NR_MAX];
u8 task_stack[TASK_NR_MAX][TASK_STK_SIZE];

s32 tcb_alloc()
{
    u32 i;

    for(i=0;i<sizeof(tcb)/sizeof(tcb[0]);i++) {
        if (tcb[i].state == TASK_UNUSED) {
            tcb[i].id = i;
            return &tcb[i];
        }
    }
    return NULL;
}

s32 tcb_init(struct __task__ *ptask, func_1 task_entry, u32 arg)
{
    ptask->state = TASK_READY;
    ptask->stack = &task_stack[ptask->id][TASK_STK_SIZE-1];
    ptask->stack_size = TASK_STK_SIZE;
    ptask->entry = task_entry;
    /* context init */
}

s32 task_create(func_1 entry, u32 arg)
{
    struct __task__ *ptask;
    if ((ptask = tcb_alloc()) == NULL) {
        return ENOMEM;
    }

    tcb_init(ptask, entry, arg);

    return 0;
}

s32 task_delete(u32 task_id)
{
    return 0;
}
