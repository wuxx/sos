#include <libc.h>
#include "syscall.h"

#define SYSCALL_ARG_MAX (4)


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
    register int __r0 __asm("r0");
    register int __r1 __asm("r1");
    register int __r2 __asm("r2");
    register int __r3 __asm("r3");

    __r0 = (u32)entry;
    __r1 = arg;
    __r2 = prio;
    /* invoke the swi */
    asm ( 
            "swi " SYS_TASK_CREATE "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );
    return __r0;   /* TODO: add appropriate return value */
}

s32 os_task_sleep(u32 ticks)
{
#if 0
    register int __r0 __asm("r0");

    args[0] = (u32)ticks;
    /* invoke the swi */
    asm (   "ldr r0, =args\n\t"
            "swi " SYS_TASK_CREATE "\n\t"
        :"=r" (__r0)
        :
        :"cc"
            );
    return __r0;   /* TODO: add appropriate return value */
#endif
    return 0;
}

s32 os_sem_create(u32 tokens)
{
#if 0
    u32 args[SYSCALL_ARG_MAX];
    register int __r0 __asm("r0");

    args[0] = (u32)tokens;
    /* invoke the swi */
    asm (   "ldr r0, =args\n\t"
            "swi " SYS_TASK_CREATE "\n\t"
        :"=r" (__r0)
        :
        :"cc"
            );
    return __r0;   /* TODO: add appropriate return value */

#endif
    return 0;
}

s32 do_task_create(u32 _args)
{
    u32 *args = (u32*)_args;
    return task_create(args[0], args[1], args[2]);
}

s32 do_task_sleep(u32 _args)
{
    u32 *args = (u32*)_args;
    task_sleep(args[0]);
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
