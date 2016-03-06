#include <system_config.h>
#include <libc.h>

/* user should manage the mailbox memory themself */
struct __os_mailbox__ {
    void *mailbox;
    u32 mail_nr;
    u32 mail_size;
    u32 head;
    u32 tail;
    struct __os_task__ *next;
};

struct __os_mailbox__ mailbox[MBX_NR_MAX];

s32 os_mail_put(struct __os_mailbox__ *mailbox, void *mail)
{
    return 0;
}

s32 os_mail_get(struct __os_mailbox__ *mailbox)
{
    return 0;
}

s32 os_mailbox_init(void *mem, u32 size, u32 mail_nr)
{
    return 0;
}
