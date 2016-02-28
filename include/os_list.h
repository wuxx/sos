#ifndef __OS_LIST_H__
#define __OS_LIST_H__

#include <os_task.h>

struct __os_list__ {
    struct __os_task__ *next;
};

extern struct __os_list__ os_ready_list;
extern struct __os_list__ os_sleep_list;

s32 list_insert(struct __os_list__ *list, struct __os_task__ *ptask);
s32 list_delete(struct __os_list__ *list, struct __os_task__ *ptask);

#endif /* __OS_LIST_H__ */



