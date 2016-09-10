#ifndef __OS_MAILBOX_H__
#define __OS_MAILBOX_H__

enum MBX_STATUS_E {
    MBX_FREE  = 0,
    MBX_EMPTY = 1,
    MBX_FULL  = 2,
    MBX_IDLE  = 3,
};

/* user should manage the mailbox memory themself */
struct __os_mailbox__ {
    u32 type;
    struct __os_task__ *next;
    u32 status;
    void *mailbox;
    u32 mail_nr;
    u32 mail_size;
    u32 head;
    u32 tail;
};

s32 mailbox_create(void *addr, u32 mail_size, u32 mail_nr);
s32 mail_alloc(u32 mbx_id);
s32 mail_free(u32 mbx_id, void *mail);
s32 mailbox_delete(u32 mbx_id);
s32 mailbox_put(u32 mbx_id, void *mail);
s32 mailbox_get(u32 mbx_id);
s32 mailbox_init();

#endif /* __OS_MAILBOX_H__ */
