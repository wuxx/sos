#include <system_config.h>
#include <libc.h>
#include <os.h>

/* event: L16: event waiting ; H16:event already issued */
s32 event_wait(u32 task_id, u16 event)
{
    u8 i, bw, bp;
    u16 eventw, eventp;
    kassert(task_id < TASK_NR_MAX);

    eventw = (u16)(tcb[task_id].events);         /* event waiting */
    eventp = (u16)(tcb[task_id].events >> 16);   /* event pending (already happened) */

    eventw |= event;

    for(i = 0; i < 16; i++) {
        bw = get_bit(eventw, i);
        bp = get_bit(eventp, i);
        if (bp == 1 && bw == 1) {
            set_bit((u32 *)&eventp, i, 0);
            set_bit((u32 *)&eventw, i, 0);
        }
    }

    tcb[task_id].events = eventp << 16 | eventw;
    if (eventw != 0) {
        current_task->state = TASK_WAIT_EVENT;
        task_dispatch();
    }

    return 0;
}

s32 event_release(u32 task_id, u32 event)
{
    u8 i, bw, bp;
    u16 eventw, eventp;
    struct __os_task__ *ptask = NULL;

    kassert(task_id < TASK_NR_MAX);

    ptask  = &tcb[task_id];
    eventw = (u16)(tcb[task_id].events);         /* event waiting */
    eventp = (u16)(tcb[task_id].events >> 16);   /* event pending (already happened) */

    eventw |= event;

    for(i = 0; i < 16; i++) {
        bw = get_bit(eventw, i);
        bp = get_bit(eventp, i);
        if (bp == 1 && bw == 1) {
            set_bit((u32 *)&eventp, i, 0);
            set_bit((u32 *)&eventw, i, 0);
        }
    }

    tcb[task_id].events = eventp << 16 | eventw;
    if (eventw == 0) {
        ptask->state = TASK_READY;
        os_ready_insert(ptask);

        if (ptask->prio < current_task->prio) {
            current_task->state = TASK_READY;
            task_dispatch();
        }
    }

    return 0;

}
