#include <system_config.h>
#include <libc.h>

enum SEM_STATUS_E {
    SEM_UNUSED = 0,
    SEM_USED   = 1,
};

struct __os_semaphore__ {
    u32 status; 
    u32 tokens;
    struct __os_task__ *next;
};

struct __os_semaphore__ os_semaphore[SEM_NR_MAX];

struct __os_semaphore__ * os_semaphore_init()
{
    struct __os_semaphore__ *sem = NULL;
    sem->tokens = 0;
    sem->next   = NULL;
    return 0;
}
