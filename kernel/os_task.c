#include <libc.h>
#include <system_config.h>
#include "task.h"
#include "log.h"

struct __task__ tcb[TASK_NR_MAX] = {0};
u8 task_stack[TASK_NR_MAX][TASK_STK_SIZE] = {0};

s32 tcb_alloc()
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

s32 tcb_init(struct __task__ *ptask, func_1 task_entry, u32 arg)
{
    struct cpu_context *cc;

    ptask->state = TASK_READY;
    ptask->stack = &task_stack[ptask->id][0];
    ptask->stack_size = TASK_STK_SIZE;
    ptask->entry = task_entry;
    /* context init */

    cc = (struct cpu_context *)(ptask->stack[TASK_STK_SIZE - sizeof(struct cpu_context)]);

    cc->cpsr = 0x15F;   /* irq enable, fiq disable, arm instruction, system mode */
    cc->r0   = arg;
    cc->r1   = 0;
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
    /*cc->r13 = 0;*/
    cc->lr = (u32)task_entry + 4;     /* pc + 4 */

    ptask->stack[0] = 0xbadbeef;
}

s32 task_create(func_1 entry, u32 arg)
{
    struct __task__ *ptask;
    if ((ptask = tcb_alloc()) == NULL) {
        return ENOMEM;
    }

    tcb_init(ptask, entry, arg);

    return 0;
}

s32 task_delete(u32 task_id)
{
    return 0;
}
