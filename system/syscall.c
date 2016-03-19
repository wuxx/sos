#include <libc.h>
#include "syscall.h"

#define SYSCALL_ARG_MAX (4)

u32 args[SYSCALL_ARG_MAX];

s32 do_task_create(u32 args);
s32 do_task_sleep (u32 args);
s32 do_sem_create (u32 args);
s32 do_sem_get    (u32 args);
s32 do_sem_put    (u32 args);
s32 do_sem_delete (u32 args);

struct __syscall__ syscall_table[] = {
    {SYS_TASK_CREATE,  do_task_create},
    {SYS_TASK_SLEEP,   do_task_sleep }, /* not available now */
    {SYS_SEM_CREATE,   do_sem_create }, /* not available now */
    {SYS_SEM_GET,      do_sem_get    }, /* not available now */
    {SYS_SEM_PUT,      do_sem_put    }, /* not available now */
    {SYS_SEM_DELETE,   do_sem_delete }, /* not available now */
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

s32 do_task_sleep(u32 _args)
{
    return 0;
}

s32 do_sem_create(u32 _args)
{
    return 0;
}

s32 do_sem_get(u32 _args)
{
    return 0;
}

s32 do_sem_put(u32 _args)
{
    return 0;
}

s32 do_sem_delete(u32 _args)
{
    return 0;
}
