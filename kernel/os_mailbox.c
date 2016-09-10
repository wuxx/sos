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

/* for producer */
s32 mail_alloc(u32 mbx_id)
{
    u32 mail_addr = 0;

    kassert(mbx_id < MBX_NR_MAX);

    if (os_mailbox[mbx_id].status != MBX_FULL) {
        mail_addr = (u32)(os_mailbox[mbx_id].mailbox) + os_mailbox[mbx_id].tail * os_mailbox[mbx_id].mail_size;
    }

    return mail_addr;
}

/* for producer */
s32 mailbox_put(u32 mbx_id, void *mail)
{

    u32 mail_index;
    kassert(mbx_id < MBX_NR_MAX);

    mail_index = ((u32)mail - (u32)os_mailbox[mbx_id].mailbox) / os_mailbox[mbx_id].mail_size;

    kassert(mail_index == os_mailbox[mbx_id].tail);

    os_mailbox[mbx_id].tail++ ;
    os_mailbox[mbx_id].tail = os_mailbox[mbx_id].tail % os_mailbox[mbx_id].mail_nr;

    if (os_mailbox[mbx_id].tail == os_mailbox[mbx_id].head) {
        os_mailbox[mbx_id].status = MBX_FULL;
    }

    /* somebody is waiting for mail, wake it up */
    if (os_mailbox[mbx_id].next != NULL) {

    }

    return 0;
}

/* for consumer */
s32 mailbox_get(u32 mbx_id)
{
    u32 mail;

    if (os_mailbox[mbx_id].status != MBX_EMPTY) {
        mail = (u32)(os_mailbox[mbx_id].mailbox) + os_mailbox[mbx_id].head * os_mailbox[mbx_id].mail_size;
    } else {
        /* wait in mbx list */
    }

    return mail;
}

/* for consumer */
s32 mail_free(u32 mbx_id, void *mail)
{
    u32 mail_index;
    kassert(mbx_id < MBX_NR_MAX);
    kassert(mail != NULL);
    kassert(os_mailbox[mbx_id].status != MBX_EMPTY);

    mail_index = ((u32)mail - (u32)os_mailbox[mbx_id].mailbox) / os_mailbox[mbx_id].mail_size;

    kassert(mail_index == os_mailbox[mbx_id].head);

    os_mailbox[mbx_id].head++;

    os_mailbox[mbx_id].head = os_mailbox[mbx_id].head % os_mailbox[mbx_id].mail_nr;

    if (os_mailbox[mbx_id].tail == os_mailbox[mbx_id].head) {
        os_mailbox[mbx_id].status = MBX_EMPTY;
    }

    if (os_mailbox[mbx_id].status == MBX_FULL) {
        os_mailbox[mbx_id].status = MBX_IDLE;
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
