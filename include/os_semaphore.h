#ifndef __OS_SEMAPHORE__
#define __OS_SEMAPHORE__
#include <types.h>

enum SEM_STATUS_E {
    SEM_FREE = 0,
    SEM_USED = 1,
};

struct __os_semaphore__ {
    u32 type;
    struct __os_task__ *next;
    u32 status;
    u32 token;
};

s32 semaphore_create(u32 res_num);
s32 semaphore_get(u32 sem_id);
s32 semaphore_put(u32 sem_id);
s32 semaphore_delete(u32 sem_id);

struct __os_semaphore__ * semaphore_init();

#endif /* __OS_SEMAPHORE__ */
