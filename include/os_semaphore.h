#ifndef __OS_SEMAPHORE__
#define __OS_SEMAPHORE__
#include <types.h>
s32 semaphore_create(u32 res_num);
s32 semaphore_get(u32 sem_id);
s32 semaphore_put(u32 sem_id);
s32 semaphore_delete(u32 sem_id);

struct __os_semaphore__ * semaphore_init();

#endif /* __OS_SEMAPHORE__ */
