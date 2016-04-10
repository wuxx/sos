#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#define SYS_TASK_CREATE "0x0"
#define SYS_TASK_SLEEP  "0x1"

#define SYS_SEM_CREATE  "0x2"
#define SYS_SEM_GET     "0x3"
#define SYS_SEM_PUT     "0x4"
#define SYS_SEM_DELETE  "0x5"

#define SYS_CALL_MAX    "0x100"

struct __syscall__ {
    char *desc;
    s32 (*handler)(u32 *arg);
};
s32 system_call(u32 nr, u32 *args);

s32 os_task_create(func_1 entry, u32 arg, u32 prio);
s32 os_task_delete(u32 task_id);
s32 os_task_sleep(u32 ticks);

s32 os_semaphore_create(u32 tokens);
s32 os_semaphore_delete(u32 sem_id);
s32 os_semaphore_get(u32 sem_id);
s32 os_semaphore_put(u32 sem_id);
#endif /* __SYSCALL_H__ */
