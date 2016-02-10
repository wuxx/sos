#ifndef __OS_TASK_H__
#define __OS_TASK_H__
#include <libc.h>
#include <system_config.h>
#include "cpu.h"

/* task state */
enum TASK_STATE_E {
    TASK_UNUSED  = 0,
    TASK_RUNNING = 1, 
    TASK_SUSPEND = 2, 
    TASK_READY   = 3,
    TASK_STATE_MAX,
};
/* task priority */
#define TASK_PRIO_MAX  255  /* 0: the highest priority, 255: the lowest priority */

struct __task__
{
    u32 sp;

    u32 id;
    u32 state;
    u32 prio;

    struct __task__ *prev;
    struct __task__ *next;

    u32 *stack;
    u32 stack_size;
    func_1 entry;
};

extern u32 task_stack[TASK_NR_MAX][TASK_STK_SIZE];
extern struct __task__ tcb[TASK_NR_MAX];
extern struct __task__ *old_task;
extern struct __task__ *new_task;

s32 task_create(func_1 entry, u32 arg, u32 prio);
s32 task_delete(u32 task_id);

#endif /* __OS_TASK_H__ */


