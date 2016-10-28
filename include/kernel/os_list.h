#ifndef __OS_LIST_H__
#define __OS_LIST_H__

#include <os_task.h>

#define os_ready_insert(task)   list_insert(&os_ready_list, task)
#define os_ready_delete(task)   list_delete(&os_ready_list, task)

#define os_sleep_insert(task)   list_insert(&os_sleep_list, task)
#define os_sleep_delete(task)   list_delete(&os_sleep_list, task)

#define os_sem_insert(psem, task)   list_insert((struct __os_list__ *)psem, task)
#define os_sem_delete(psem, task)   list_delete((struct __os_list__ *)psem, task)

#define os_mbx_insert(pmbx, task)   list_insert((struct __os_list__ *)pmbx, task)
#define os_mbx_delete(pmbx, task)   list_delete((struct __os_list__ *)pmbx, task)

enum CB_TYPE_E {
    OS_READY,
    OS_SLEEP,
    OS_SEM,
    OS_MBX,
};

struct __os_list__ {
    u32 type;
    struct __os_task__ *next;
    struct __os_task__ *prev;
};

extern struct __os_list__ os_ready_list;
extern struct __os_list__ os_sleep_list;

s32 list_insert(struct __os_list__ *list, struct __os_task__ *ptask);
s32 list_delete(struct __os_list__ *list, struct __os_task__ *ptask);

#endif /* __OS_LIST_H__ */



