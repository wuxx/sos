#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#define SYS_TASK_CREATE "0x0"
#define SYS_TASK_SLEEP  "0x1"

#define SYS_SEM_CREATE  "0x2"
#define SYS_SEM_GET     "0x3"
#define SYS_SEM_PUT     "0x4"
#define SYS_SEM_DELETE  "0x5"

#define SYS_MBX_CREATE  "0x6"
#define SYS_MBX_GET     "0x7"
#define SYS_MBX_PUT     "0x8"
#define SYS_MBX_DELETE  "0x9"

#define SYS_CALL_MAX    "0x100"

struct __syscall__ {
    char *desc;
    s32 (*handler)(u32 *arg);
#if 0
    u32 free;   /* if the run context (task context | irq context) is free  */
#endif
};
s32 system_call(u32 nr, u32 *args);

s32 os_task_create(func_1 entry, u32 arg, u32 prio);
s32 os_task_delete(u32 task_id);
s32 os_task_sleep(u32 ticks);

s32 os_semaphore_create(u32 tokens);
s32 os_semaphore_delete(u32 sem_id);
s32 os_semaphore_get(u32 sem_id);
s32 os_semaphore_put(u32 sem_id);

s32 os_mailbox_create(void *addr, u32 mail_size, u32 mail_nr);
s32 os_mailbox_delete(u32 mbx_id);
s32 os_mailbox_get(u32 mbx_id);
s32 os_mailbox_put(u32 mbx_id, void *mail);
#endif /* __SYSCALL_H__ */
