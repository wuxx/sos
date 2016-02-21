#ifndef __SYS_CALL_H__
#define __SYS_CALL_H__

#define SYS_TASK_CREATE "0x0"
#define SYS_CALL_MAX    "0x100"

struct __sys_call__ {
    char *desc;
    func_1 handler;
};

#endif /* __SYS_CALL_H__ */
