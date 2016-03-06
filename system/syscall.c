#include <libc.h>
#include "syscall.h"

#define SYSCALL_ARG_MAX (4)

u32 args[SYSCALL_ARG_MAX];
s32 do_task_create(u32 args);

struct __syscall__ syscall_table[] = {
    {SYS_TASK_CREATE,  do_task_create},
};

s32 os_task_create(func_1 entry, u32 arg, u32 prio)
{
    args[0] = (u32)entry;
    args[1] = arg;
    args[2] = prio;
    /* invoke the swi */
    asm (   "ldr r0, =args\n\t"
            "swi " SYS_TASK_CREATE "\n\t"
        :
        :
        :"r0"
            );
    return 0;
}

s32 do_task_create(u32 _args)
{
    u32 *args = (u32*)_args;
    return task_create(args[0], args[1], args[2]);
}
