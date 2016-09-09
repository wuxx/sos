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

    if (sem_id != -1) {
        os_semaphore[sem_id].status = SEM_USED;
        os_semaphore[sem_id].token = res_num;
    }

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

/*
    TODO:   1. task A prio 10 is waiting for sem, but token == 0, so put it in sem list
            2. task D prio 20 running, it create a task B prio 5
            3. task B prio  5 running, it put the sem, token -> 1, then put task A in ready list
            4. task B exit, then assume task C prio 7 running, get the sem, token -> 0; 
            5. at this point, task B should not run, but task B is in ready list!
*/

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
            sem->token --;
            ptask = sem->next;
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

