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
    func_1 handler;
};

#endif /* __SYSCALL_H__ */
