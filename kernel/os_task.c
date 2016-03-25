#include <system_config.h>
#include <libc.h>
#include <os.h>
#include "log.h"

struct __os_task__ tcb[TASK_NR_MAX]        __attribute__((__aligned__(0x100))) = {0};
u32 task_stack[TASK_NR_MAX][TASK_STK_SIZE] __attribute__((__aligned__(0x100))) = {0};

/* get current task id, little hack */
PRIVATE u32 get_current_task()
{
    u32 task_id;
    u32 sp = __get_sp();

    task_id = (sp - (u32)task_stack) / (TASK_STK_SIZE * 4);
    PRINT_EMG("task_id: %d\n", task_id);
    assert(task_id <= TASK_NR_MAX);
    return task_id;
}

/* get the highest priority task in os_ready_list */
PUBLIC struct __os_task__ * get_task_ready()
{
#if 0
    u32 i;
    u32 best = 256;
    u32 prio = TASK_PRIO_MAX;
    for(i=0;i<TASK_NR_MAX;i++) {
        if (tcb[i].state == TASK_READY && tcb[i].prio < prio) {
            prio = tcb[i].prio;
            best = i;
        }   
    }   
    /*PRINT_EMG("get %d \n", best);*/
    if (best == 256) {
        return NULL;
    } else {
        return &tcb[best];
    }   
#else
    struct __os_task__ *ptask;
    ptask = os_ready_list.next;
    PRINT_DEBUG("get_task_ready %x \n", ptask);
    assert(ptask != NULL);
    return ptask;
#endif
}

PRIVATE struct __os_task__ * tcb_alloc()
{
    u32 i;

    for(i=0;i<sizeof(tcb)/sizeof(tcb[0]);i++) {
        if (tcb[i].state == TASK_UNUSED) {
            tcb[i].id = i;
            return &tcb[i];
        }
    }
    return NULL;
}

PRIVATE void task_matrix(u32 addr, u32 arg)
{
    u32 current_task_id;
    func_1 task_entry = (func_1)addr;
    task_entry(arg);
    current_task_id = get_current_task();

    task_delete(current_task_id);
    while(1);
}

PRIVATE s32 tcb_init(struct __os_task__ *ptask, func_1 task_entry, u32 arg, u32 priority)
{
    struct cpu_context *cc;

    ptask->state = TASK_READY;
    ptask->prio = priority;
    ptask->stack = &task_stack[ptask->id][0];
    ptask->stack_size = TASK_STK_SIZE;
    ptask->entry = task_entry;

    /* task context init */
    cc = (struct cpu_context *)
        (&(ptask->stack[TASK_STK_SIZE - (sizeof(struct cpu_context) / 4)]));

    cc->cpsr = 0x15F;   /* irq enable, fiq disable, arm instruction, system mode */
    cc->r0   = (u32)task_entry;
    cc->r1   = arg;
    cc->r2   = 0;
    cc->r3   = 0;
    cc->r4   = 0;
    cc->r5   = 0;
    cc->r6   = 0;
    cc->r7   = 0;
    cc->r8   = 0;
    cc->r9   = 0;
    cc->r10  = 0;
    cc->r11  = 0;
    cc->r12  = 0;
    cc->sp   = (u32)(&ptask->stack[TASK_STK_SIZE]);
    cc->lr   = 0;
    cc->pc   = (u32)task_matrix + 4;     /* real pc + 4, create the fake irq mode saved cpu_context */

    ptask->sp = (u32)cc;

    ptask->stack[0] = 0xbadbeef;

    return 0;
}

PUBLIC s32 task_dispatch()
{
    return 0;
}

PUBLIC s32 task_create(func_1 entry, u32 arg, u32 prio)
{
    struct __os_task__ *ptask;
    if ((ptask = tcb_alloc()) == NULL) {
        return ENOMEM;
    }

    tcb_init(ptask, entry, arg, prio);

    os_ready_insert(ptask);

    return 0;
}

PUBLIC s32 task_delete(u32 task_id)
{
    tcb[task_id].prio  = TASK_PRIO_MAX; /* lowest prio */
    tcb[task_id].state = TASK_UNUSED;
    return 0;
}
