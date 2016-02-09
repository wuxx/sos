#include <memory_map.h>
#include <system_config.h>
#include <os.h>
#include <libc.h>
#include "timer.h"
#include "log.h"

extern u32 os_tick;
extern struct __task__ tcb[TASK_NR_MAX];

char *task_state_desc[] = {
    "TASK_UNUSED",
    "TASK_RUNNING",
    "TASK_SUSPEND",
    "TASK_READY",
};

void dump_tcb()
{
    u32 i;
    for(i=0;i<TASK_NR_MAX;i++) {

        PRINT_EMG("[%d]: [%s]\n", i, task_state_desc[tcb[i].state]);
        if (tcb[i].state != TASK_UNUSED) {
            PRINT_EMG("\ttask_id:    [%d]\n", tcb[i].id);
            PRINT_EMG("\tstate:      [%s]\n", task_state_desc[tcb[i].state]);
            PRINT_EMG("\tprio:       [%d]\n", tcb[i].prio);
            PRINT_EMG("\tstack:      [0x%x]\n", tcb[i].stack);
            PRINT_EMG("\tstack_size: [%d]\n", tcb[i].stack_size);
            PRINT_EMG("\ttask_entry: [0x%x]\n", tcb[i].entry);
        }
    }
}

s32 test_os_all(u32 argc, char **argv)
{
    u64 sc;
    u32 clo, chi;
    s32 ret = 0;
    u32 i, arg1;
    i    = atoi(argv[2]);
    arg1 = atoi(argv[3]);

    PRINT_EMG("arg1: %d\n", arg1);

    switch(i) {
        case (0):
            PRINT_EMG("os_tick: 0x%x\n", os_tick);
            break;
        case (1):
            dump_tcb();
            break;
        default:
            return -1;
    }

    return ret;
}
