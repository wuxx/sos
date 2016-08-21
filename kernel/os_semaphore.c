#include <system_config.h>
#include <libc.h>
#include <os.h>

struct __os_semaphore__ os_semaphore[SEM_NR_MAX];

PRIVATE s32 get_free_sem()
{
    u32 i;
    for(i = 0; i < SEM_NR_MAX; i++) {
        if (os_semaphore[i].status == SEM_FREE) {
            return i;
        }
    }
    return -1;
}

PUBLIC s32 semaphore_create(u32 res_num)
{
    s32 sem_id;
    sem_id = get_free_sem();
    os_semaphore[sem_id].status = SEM_USED;
    os_semaphore[sem_id].token = res_num;
    return sem_id;
}

PUBLIC s32 semaphore_get(u32 sem_id)
{
    struct __os_semaphore__ *sem = NULL;

    kassert(sem_id < SEM_NR_MAX);
    kassert(os_semaphore[sem_id].status == SEM_USED);

    sem = &os_semaphore[sem_id];

    if (sem->token == 0) {
        current_task->private_data = sem;
        current_task->state = TASK_WAIT_SEM;
        task_dispatch();
    } else {
        sem->token -- ;
    }

    return 0;
}

PUBLIC s32 semaphore_put(u32 sem_id)
{
    struct __os_semaphore__ *sem = NULL;
    struct __os_task__ *ptask = NULL;

    kassert(sem_id < SEM_NR_MAX);
    kassert(os_semaphore[sem_id].status == SEM_USED);

    sem = &os_semaphore[sem_id];

    sem->token ++;
    if (sem->token == 1) {  /* 0 -> 1 */
        if (sem->next != NULL) { /* somebody is waiting for the sem */
#if 0
            ptask = sem->next;
            sem->next = sem->next->next;
#endif
            ptask->state = TASK_READY;
            os_sem_delete(sem, ptask);
            os_ready_insert(ptask);

            if (ptask->prio < current_task->prio) {
                current_task->state = TASK_READY;
                task_dispatch();
            }
        }
    }

    return 0;
}

PUBLIC s32 semaphore_delete(u32 sem_id)
{
    struct __os_semaphore__ *sem = NULL;

    kassert(sem_id < SEM_NR_MAX);
    kassert(os_semaphore[sem_id].status == SEM_USED);

    sem = &os_semaphore[sem_id];
    assert(sem->next == NULL);
    sem->status = SEM_FREE;
    sem->token = 0;
    return 0;
}

PUBLIC struct __os_semaphore__ * semaphore_init()
{
    u32 i;
    for(i = 0; i < SEM_NR_MAX; i++) {
        os_semaphore[i].type   = OS_SEM;
        os_semaphore[i].next   = NULL;
        os_semaphore[i].status = SEM_FREE;
        os_semaphore[i].token  = 0;
    }
    return 0;
}
