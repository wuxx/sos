#include <system_config.h>
#include <libc.h>
#include <os.h>
#include <int.h>

#include "log.h"


#define SYSCALL_ARG_MAX (4)

s32 do_tsk_create(u32 *args);
s32 do_tsk_sleep (u32 *args);
s32 do_sem_create (u32 *args);
s32 do_sem_get    (u32 *args);
s32 do_sem_put    (u32 *args);
s32 do_sem_delete (u32 *args);
s32 do_mbx_create (u32 *args);
s32 do_mail_alloc (u32 *args);
s32 do_mbx_get    (u32 *args);
s32 do_mbx_put    (u32 *args);
s32 do_mbx_delete (u32 *args);

struct __syscall__ syscall_table[] = {
    {SYS_TASK_CREATE,  do_tsk_create,  },
    {SYS_TASK_SLEEP,   do_tsk_sleep,   },
    {SYS_SEM_CREATE,   do_sem_create,  },
    {SYS_SEM_GET,      do_sem_get,     },
    {SYS_SEM_PUT,      do_sem_put,     },
    {SYS_SEM_DELETE,   do_sem_delete,  },
    {SYS_MBX_CREATE,   do_mbx_create,  },
    {SYS_MAIL_ALLOC,   do_mail_alloc,  },
    {SYS_MBX_GET,      do_mbx_get,     },
    {SYS_MBX_PUT,      do_mbx_put,     },
    {SYS_MBX_DELETE,   do_mbx_delete,  },
};

PUBLIC s32 system_call(u32 nr, u32 *args)
{
    s32 ret;
    PRINT_DEBUG("syscall %d \n", nr);

    ret = syscall_table[nr].handler(args);   /* syscall handler may invoke task_dispatch */
    return ret;
}

PUBLIC s32 os_task_create(func_1 entry, u32 arg, u32 prio)
{
    register int __r0 __asm("r0");
    register int __r1 __asm("r1");
    register int __r2 __asm("r2");
    register int __r3 __asm("r3");

    kassert(!in_interrupt());

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
    return __r0;
}

PUBLIC s32 os_task_sleep(u32 ticks)
{
    register s32 __r0 __asm("r0");
    register s32 __r1 __asm("r1");
    register s32 __r2 __asm("r2");
    register s32 __r3 __asm("r3");

    kassert(!in_interrupt());

    __r0 = (u32)ticks;
    /* invoke the swi */
    asm (
            "swi " SYS_TASK_SLEEP "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );
    return __r0;
}

PUBLIC s32 os_semaphore_create(u32 tokens)
{
    register s32 __r0 __asm("r0");
    register s32 __r1 __asm("r1");
    register s32 __r2 __asm("r2");
    register s32 __r3 __asm("r3");

    kassert(!in_interrupt());

    __r0 = (u32)tokens;
    /* invoke the swi */
    asm (
            "swi " SYS_SEM_CREATE "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );
    return __r0;
}

PUBLIC s32 os_semaphore_delete(u32 sem_id)
{
    register s32 __r0 __asm("r0");
    register s32 __r1 __asm("r1");
    register s32 __r2 __asm("r2");
    register s32 __r3 __asm("r3");

    kassert(!in_interrupt());

    __r0 = (u32)sem_id;
    /* invoke the swi */
    asm (
            "swi " SYS_SEM_DELETE "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );
    return __r0;
}


PUBLIC s32 os_semaphore_get(u32 sem_id)
{
    register s32 __r0 __asm("r0");
    register s32 __r1 __asm("r1");
    register s32 __r2 __asm("r2");
    register s32 __r3 __asm("r3");

    kassert(!in_interrupt());

    __r0 = (u32)sem_id;
    /* invoke the swi */
    asm (
            "swi " SYS_SEM_GET "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );

    return __r0;
}

PUBLIC s32 os_semaphore_put(u32 sem_id)
{
    register s32 __r0 __asm("r0");
    register s32 __r1 __asm("r1");
    register s32 __r2 __asm("r2");
    register s32 __r3 __asm("r3");

    /* kassert(!in_interrupt()); */

    __r0 = (u32)sem_id;
    /* invoke the swi */
    asm (
            "swi " SYS_SEM_PUT "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );
    return __r0;
}

PUBLIC s32 os_mailbox_create(void *addr, u32 mail_size, u32 mail_nr)
{
    register s32 __r0 __asm("r0");
    register s32 __r1 __asm("r1");
    register s32 __r2 __asm("r2");
    register s32 __r3 __asm("r3");

    kassert(!in_interrupt());

    __r0 = (u32)addr;
    __r1 = (u32)mail_size;
    __r2 = (u32)mail_nr;
    /* invoke the swi */
    asm (
            "swi " SYS_MBX_CREATE "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );
    return __r0;
}

PUBLIC s32 os_mail_alloc(u32 mbx_id)
{
    register s32 __r0 __asm("r0");
    register s32 __r1 __asm("r1");
    register s32 __r2 __asm("r2");
    register s32 __r3 __asm("r3");

    kassert(!in_interrupt());

    __r0 = (u32)mbx_id;
    /* invoke the swi */
    asm (
            "swi " SYS_MAIL_ALLOC "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );
    return __r0;
}

PUBLIC s32 os_mail_free(u32 mbx_id, void *mail)
{
    register s32 __r0 __asm("r0");
    register s32 __r1 __asm("r1");
    register s32 __r2 __asm("r2");
    register s32 __r3 __asm("r3");

    kassert(!in_interrupt());

    __r0 = (u32)mbx_id;
    __r1 = (u32)mail;
    /* invoke the swi */
    asm (
            "swi " SYS_MAIL_FREE "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );
    return __r0;
}

PUBLIC s32 os_mailbox_delete(u32 mbx_id)
{
    register s32 __r0 __asm("r0");
    register s32 __r1 __asm("r1");
    register s32 __r2 __asm("r2");
    register s32 __r3 __asm("r3");

    kassert(!in_interrupt());

    __r0 = (u32)mbx_id;
    /* invoke the swi */
    asm (
            "swi " SYS_MBX_DELETE "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );
    return __r0;
}

PUBLIC s32 os_mailbox_get(u32 mbx_id)
{
    register s32 __r0 __asm("r0");
    register s32 __r1 __asm("r1");
    register s32 __r2 __asm("r2");
    register s32 __r3 __asm("r3");

    kassert(!in_interrupt());

    __r0 = (u32)mbx_id;
    /* invoke the swi */
    asm (
            "swi " SYS_MBX_GET "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );

    return __r0;
}

PUBLIC s32 os_mailbox_put(u32 mbx_id, void *mail)
{
    register s32 __r0 __asm("r0");
    register s32 __r1 __asm("r1");
    register s32 __r2 __asm("r2");
    register s32 __r3 __asm("r3");

    /* kassert(!in_interrupt()); */

    __r0 = (u32)mbx_id;
    __r1 = (u32)mail;
    /* invoke the swi */
    asm (
            "swi " SYS_MBX_PUT "\n\t"
        :"=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)
        : "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)
        :"cc"
            );
    return __r0;
}
PUBLIC s32 do_tsk_create(u32 *args)
{
    func_1 entry; 
    u32 arg; 
    u32 prio;
    entry = (func_1)(args[0]);
    arg   = args[1];
    prio  = args[2];
    return task_create(entry, arg, prio);
}

PRIVATE s32 do_tsk_sleep(u32 *args)
{
    task_sleep(args[0]);
    return 0;
}

PRIVATE s32 do_sem_create(u32 *args)
{
    u32 tokens = args[0];
    return semaphore_create(tokens);
}

PRIVATE s32 do_sem_get(u32 *args)
{
    u32 sem_id = args[0];
    return semaphore_get(sem_id);
}

PRIVATE s32 do_sem_put(u32 *args)
{
    u32 sem_id = args[0];
    return semaphore_put(sem_id);
}

PRIVATE s32 do_sem_delete(u32 *args)
{
    u32 sem_id = args[0];
    return semaphore_delete(sem_id);
}

PRIVATE s32 do_mbx_create(u32 *args)
{
    void *addr    = (void *)args[0];
    u32 mail_size = args[1];
    u32 mail_nr   = args[2];

    return mailbox_create(addr, mail_size, mail_nr);
}

PRIVATE s32 do_mail_alloc(u32 *args)
{
    u32 mbx_id = args[0];

    return mail_alloc(mbx_id);
}

PRIVATE s32 do_mail_free(u32 *args)
{
    u32 mbx_id = args[0];
    void *mail = (void *)args[1];

    return mail_free(mbx_id, mail);
}

PRIVATE s32 do_mbx_get(u32 *args)
{
    u32 mbx_id = args[0];
    return mailbox_get(mbx_id);
}

PRIVATE s32 do_mbx_put(u32 *args)
{
    u32 mbx_id = args[0];
    void *mail = (void *)args[1];
    return mailbox_put(mbx_id, mail);
}

PRIVATE s32 do_mbx_delete(u32 *args)
{
    u32 mbx_id = args[0];
    return mailbox_delete(mbx_id);
}
