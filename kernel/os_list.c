#include <libc.h>

enum LIST_TYPE_E {
    LIST_TASK = 0,
    LIST_SEMAPHORE,
    LIST_MAILBOX,
};

#if 0
struct list_data {
    void *data;
};

struct list_head {
    u32 type;   /* list type */
    struct list_data ld;
};

s32 list_add()
{
}

s32 list_del()
{
}
#endif
