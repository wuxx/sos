#include <libc.h>
#include "sys_call.h"

#define SYSCALL_ARG_MAX (4)

s32 do_task_create(u32 *args);

struct __sys_call__ sc[] = {
    {SYS_TASK_CREATE,  do_task_create},
};

s32 os_task_create(func_1 entry, u32 arg, u32 prio)
{
    u32 args[SYSCALL_ARG_MAX];
    args[0] = (u32)entry;
    args[1] = arg;
    args[2] = prio;
    /* invoke the swi */
    asm ("swi " SYS_TASK_CREATE);
    return 0;
}

s32 do_task_create(u32 *args)
{
    return task_create(args[0], args[1], args[2]);
}
