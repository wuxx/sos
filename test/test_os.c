#include <memory_map.h>
#include <system_config.h>
#include <os.h>
#include <libc.h>
#include "timer.h"
#include "log.h"
#include "systest.h"

extern u32 os_tick;
extern struct __os_task__ tcb[TASK_NR_MAX];
extern struct __os_semaphore__ os_semaphore[];

extern void dump_ctx(struct cpu_context *ctx);

char *task_state_desc[] = {
    "TASK_UNUSED",
    "TASK_RUNNING",
    "TASK_SUSPEND",
    "TASK_READY",
};

s32 test_task(u32 arg)
{
    while (1) {
        PRINT_EMG("in %s \n", __func__);
        mdelay(1000);
    }
    return 0;
}


void dump_tcb(u32 i)
{
    struct cpu_context *ctx = (struct cpu_context *)(tcb[i].sp);

    PRINT_EMG("\n[%d]: [%s]\n", i, task_state_desc[tcb[i].state]);

    if (tcb[i].state != TASK_UNUSED) {
        dump_ctx(ctx);
        PRINT_EMG("\ttask_id:     [%d]\n", tcb[i].id);
        PRINT_EMG("\tstate:       [%s]\n", task_state_desc[tcb[i].state]);
        PRINT_EMG("\tprio:        [%d]\n", tcb[i].prio);
        PRINT_EMG("\tsleep_ticks: [%d]\n", tcb[i].sleep_ticks);
        
        PRINT_EMG("\tstack:       [0x%x]\n", tcb[i].stack);
        PRINT_EMG("\tstack_size:  [%d]\n", tcb[i].stack_size);
        PRINT_EMG("\ttask_entry:  [0x%x]\n", tcb[i].entry);
        dump_mem((u32)(tcb[i].stack), tcb[i].stack_size);
    }

}

void dump_tcb_all()
{
    u32 i;
    for(i=0;i<TASK_NR_MAX;i++) {
        dump_tcb(i);
    }
}

s32 dump_list()
{
    u32 i;
    struct __os_task__ *ptask;

    ptask = os_ready_list.next;
    PRINT_EMG("os_ready: \n");
    while(ptask != NULL) {
        /*dump_tcb(ptask->id);*/
        PRINT_EMG("[%d] ->", ptask->id);
        ptask = ptask->next;
    }

    ptask = os_sleep_list.next;
    PRINT_EMG("\nos_sleep: \n");
    while(ptask != NULL) {
        /*dump_tcb(ptask->id);*/
        PRINT_EMG("[%d] ->", ptask->id);
        ptask = ptask->next;
    }

    for(i = 0; i < SEM_NR_MAX; i++) {
        if (os_semaphore[i].status == SEM_USED) {
            ptask = os_semaphore[i].next;
            PRINT_EMG("os_sem[%d]:", i);
            while(ptask != NULL) {
                /*dump_tcb(ptask->id);*/
                PRINT_EMG("[%d] ->", ptask->id);
                ptask = ptask->next;
            }

        }
    }

    return 0;
}

s32 test_os_all(u32 argc, char **argv)
{
    s32 ret = 0;
    u32 i, arg1;
    u32 sem_id;

    i    = atoi(argv[2]);
    arg1 = atoi(argv[3]);

    PRINT_EMG("arg1: %d\n", arg1);

    switch (i) {
        case (0):
            PRINT_EMG("os_tick: 0x%x\n", os_tick);
            break;
        case (1):
            dump_tcb_all();
            break;
        case (99):  /* slip to case 100 */
            set_log_level(LOG_DEBUG);
        case (100):
            os_task_create(test_task, 0, 0);
            break;
        case (200):
            dump_list();
            break;
        case (300):
            sem_id = arg1;
            os_semaphore_put(sem_id);
            break;
        default:
            return -1;
    }

    return ret;
}
