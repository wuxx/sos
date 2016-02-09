#ifndef __OS_TASK_H__
#define __OS_TASK_H__
#include <libc.h>
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

#endif /* __OS_TASK_H__ */
