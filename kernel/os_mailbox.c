#include <system_config.h>
#include <libc.h>
#include <os.h>

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
        os_mailbox[mbx_id].status    = MBX_USED;
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

s32 mailbox_delete(u32 mbx_id)
{
    kassert(mbx_id < MBX_NR_MAX);
    os_mailbox[mbx_id].status = MBX_FREE;
    return 0;
}

s32 mailbox_put(u32 mbx_id, void *mail)
{
    return 0;
}

s32 mailbox_get(u32 mbx_id)
{
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
