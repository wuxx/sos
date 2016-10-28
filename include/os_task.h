#ifndef __OS_TASK_H__
#define __OS_TASK_H__
#include <libc.h>
#include <system_config.h>
#include "cpu.h"

/* task state */
enum TASK_STATE_E {
    TASK_UNUSED     = 0,
    TASK_RUNNING    = 1,
    TASK_SLEEP      = 2,
    TASK_READY      = 3,
    TASK_WAIT_SEM   = 4,
    TASK_WAIT_MBX   = 5,
    TASK_WAIT_EVENT = 6,
    TASK_STATE_MAX,
};
/* task priority */
#define TASK_PRIO_MAX   (255)  /* 0: the highest priority, 255: the lowest priority */

#define IDLE_TASK_ID    (0) /* idle task id */

struct __os_task__
{
    u32 id;

    struct __os_task__ *next;   /* must be at offset 4byte */
    struct __os_task__ *prev;

    u32 state;
    u32 prio;
    void *private_data;

    u32 sleep_ticks;
    u32 events;

    u32 sp;
    u32 *stack;
    u32 stack_size;
    func_1 entry;
};

extern u32 task_stack[TASK_NR_MAX][TASK_STK_SIZE];
extern struct __os_task__ tcb[TASK_NR_MAX];
extern struct __os_task__ *current_task;
extern struct __os_task__ *new_task;

struct __os_task__ * get_task_ready();
s32 task_create(func_1 entry, u32 arg, u32 prio);
s32 task_delete(u32 task_id);
s32 task_sleep(u32 ticks);
s32 task_dispatch();
struct __os_task__ * get_best_task();
s32 task_delete(u32 task_id);
s32 task_init();
#endif /* __OS_TASK_H__ */


