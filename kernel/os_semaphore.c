#include <system_config.h>
#include <libc.h>
#include <os.h>

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

struct __os_semaphore__ os_semaphore[SEM_NR_MAX];

struct __os_semaphore__ * os_semaphore_init()
{
    u32 i;
    for(i=0;i<SEM_NR_MAX;i++) {
        os_semaphore[i].type   == OS_SEM;
        os_semaphore[i].next   == NULL;
        os_semaphore[i].status == SEM_FREE;
        os_semaphore[i].token == 0;
    }
    return 0;
}

s32 get_free_sem()
{
    u32 i;
    for(i=0;i<SEM_NR_MAX;i++) {
        if (os_semaphore[i].status == SEM_FREE) {
            return i;
        }
    }
    return -1;
}

s32 semaphore_create(u32 res_nr)
{
    s32 sem_id;
    sem_id = get_free_sem();
    os_semaphore[sem_id].status = SEM_USED;
    os_semaphore[sem_id].token = res_nr;
    return sem_id;
}

s32 semaphore_get(u32 sem_id)
{
    struct __os_semaphore__ *sem = NULL;

    assert(sem_id < SEM_NR_MAX);
    assert(os_semaphore[sem_id].status == SEM_USED);

    sem = &os_semaphore[sem_id];

    if (sem->token == 0) {
        current_task->state = TASK_WAIT_SEM;
        current_task->private_data = sem;
        task_dispatch();
    } else {
        sem->token -- ;
    }

    return 0;
}

s32 semaphore_put(u32 sem_id)
{
    struct __os_semaphore__ *sem = NULL;
    struct __os_task__ *ptask = NULL;

    assert(sem_id < SEM_NR_MAX);
    assert(os_semaphore[sem_id].status == SEM_USED);

    sem = &os_semaphore[sem_id];

    sem->token ++;
    if (sem->token == 1) {
        /* */
        if (sem->next != NULL) { /* somebody is waiting for the sem */
            ptask = sem->next;
            sem->next = sem->next->next;
            if (ptask->prio < current_task->prio) {
                os_ready_insert(ptask);
                task_dispatch();
            }
        }
    }

    return 0;
}

s32 semaphore_delete(u32 sem_id)
{
    struct __os_semaphore__ *sem = NULL;

    assert(sem_id < SEM_NR_MAX);
    assert(os_semaphore[sem_id].status == SEM_USED);

    sem = &os_semaphore[sem_id];
    assert(sem->next == NULL);
    sem->status = SEM_FREE;
    sem->token = 0;
    return 0;
}
