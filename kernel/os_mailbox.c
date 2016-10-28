#include <system_config.h>
#include <libc.h>
#include <os.h>

/* for producer (int context or task context (FIXME)) */
/* mail_alloc  -> mailbox_put */

/* for consumer (task context) */
/* mailbox_get -> mail_free */

struct __os_mailbox__ os_mailbox[MBX_NR_MAX];

PRIVATE s32 get_free_mbx()
{
    u32 i;
    for(i = 0; i < MBX_NR_MAX; i++) {
        if (os_mailbox[i].status == MBX_FREE) {
            return i;
        }
    }
    return -1;
}

s32 mailbox_create(void *addr, u32 mail_size, u32 mail_nr)
{
    s32 mbx_id;
    mbx_id = get_free_mbx();

    if (mbx_id != -1) {
        os_mailbox[mbx_id].status    = MBX_EMPTY;
        os_mailbox[mbx_id].mailbox   = addr;
        os_mailbox[mbx_id].mail_size = mail_size;
        os_mailbox[mbx_id].mail_nr   = mail_nr;
        os_mailbox[mbx_id].head      = 0;
        os_mailbox[mbx_id].tail      = 0;
        os_mailbox[mbx_id].next      = NULL;
        return mbx_id;
    } else {
        return -1;
    }

}

/* producer */
s32 mail_alloc(u32 mbx_id)
{
    u32 mail_addr = 0;

    kassert(mbx_id < MBX_NR_MAX);

    if (os_mailbox[mbx_id].status != MBX_FULL) {
        mail_addr = (u32)(os_mailbox[mbx_id].mailbox) + os_mailbox[mbx_id].tail * os_mailbox[mbx_id].mail_size;
    }

    return mail_addr;
}

/* producer */
s32 mailbox_put(u32 mbx_id, void *mail)
{
    u32 mail_index;
    struct __os_mailbox__ *pmbx;
    struct __os_task__ *ptask;
    struct __cpu_context__ *cc;

    kassert(mbx_id < MBX_NR_MAX);

    pmbx = &os_mailbox[mbx_id];
    mail_index = ((u32)mail - (u32)(pmbx->mailbox)) / pmbx->mail_size;

    kassert(mail_index == pmbx->tail);

    pmbx->tail++ ;
    pmbx->tail = pmbx->tail % pmbx->mail_nr;

    if (pmbx->tail == pmbx->head) {
        pmbx->status = MBX_FULL;
    }

    /* somebody is waiting for mail, wake it up */
    if (pmbx->next != NULL) {
        ptask = pmbx->next;

        mail = (void *)((u32)(pmbx->mailbox) + pmbx->head * pmbx->mail_size);
        pmbx->head++;
        pmbx->head = pmbx->head % pmbx->mail_nr;

        cc = (struct __cpu_context__ *)(ptask->sp);
        cc->r0 = (u32)mail;

        ptask->state = TASK_READY;
        os_mbx_delete(pmbx, ptask);
        os_ready_insert(ptask);

        if (ptask->prio < current_task->prio) {
            current_task->state = TASK_READY;
            task_dispatch();
        }
    }

    return 0;
}

/* consumer */
s32 mailbox_get(u32 mbx_id)
{
    u32 mail;
    struct __os_mailbox__ *pmbx = NULL;

    pmbx = &os_mailbox[mbx_id];

get_mail:

    if (os_mailbox[mbx_id].status != MBX_EMPTY) {
        mail = (u32)(pmbx->mailbox) + pmbx->head * pmbx->mail_size;
        pmbx->head++;
        pmbx->head = pmbx->head % pmbx->mail_nr;
    } else {
        /* wait in mbx list */
        current_task->private_data = pmbx;
        current_task->state = TASK_WAIT_MBX;
        task_dispatch();
        goto get_mail;  /* FIXME: we are in swi context, how can the task restore in swi context? need read the arm-v6 TRM */
    }

    return mail;
}

/* consumer */
s32 mail_free(u32 mbx_id, void *mail)
{
    u32 mail_index;
    struct __os_mailbox__ *pmbx = NULL;

    kassert(mbx_id < MBX_NR_MAX);
    kassert(mail != NULL);
    kassert(os_mailbox[mbx_id].status != MBX_EMPTY);
    pmbx = &os_mailbox[mbx_id];

    mail_index = ((u32)mail - (u32)pmbx->mailbox) / pmbx->mail_size;

    kassert(mail_index == pmbx->head);

    pmbx->head++;

    pmbx->head = pmbx->head % pmbx->mail_nr;

    if (pmbx->tail == pmbx->head) {
        pmbx->status = MBX_EMPTY;
    }

    if (pmbx->status == MBX_FULL) {
        pmbx->status = MBX_IDLE;
    }

    return 0;
}

s32 mailbox_delete(u32 mbx_id)
{
    kassert(mbx_id < MBX_NR_MAX);
    os_mailbox[mbx_id].status = MBX_FREE;
    return 0;
}

s32 mailbox_init()
{
    u32 i;
    for(i = 0; i < MBX_NR_MAX; i++) {
        os_mailbox[i].type    = OS_MBX;
        os_mailbox[i].next    = NULL;
        os_mailbox[i].status  = MBX_FREE;
        os_mailbox[i].mailbox = NULL;
        os_mailbox[i].mail_nr = 0;
        os_mailbox[i].mail_size = 0;
        os_mailbox[i].head = 0;
        os_mailbox[i].tail = 0;
    }

    return 0;
}
