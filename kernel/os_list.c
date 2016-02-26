#include <system_config.h>
#include <libc.h>
#include <os_task.h>
#include <os_list.h>


struct __os_list__ os_ready = {.next = NULL};
struct __os_list__ os_sleep = {.next = NULL};


s32 list_insert(struct __os_list__ *list, struct __os_task__ *ptask)
{
    return 0;
}

s32 list_delete(struct __os_list__ *list, struct __os_task__ *ptask)
{
    return 0;
}
