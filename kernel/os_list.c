#include <system_config.h>
#include <libc.h>
#include <os_task.h>
#include <os_list.h>


struct __os_list__ os_ready_list = {.next = NULL};
struct __os_list__ os_sleep_list = {.next = NULL};


s32 list_insert(struct __os_list__ *list, struct __os_task__ *ptask)
{
    struct __os_task__ *pprev, *pcurr;
    pprev = list;
    pcurr = list->next;

    if (list == &os_ready_list) {
        while (pcurr != NULL && pcurr->prio <= ptask->prio) {
            pprev = pcurr;
            pcurr = pcurr->next;
        }
        pprev->next = ptask;
        ptask->next = pcurr;

        ptask->prev = pprev;
        pcurr->prev = ptask;

    } else if (list == &os_sleep_list) {

    } else {
        panic();
    }
    return 0;
}

s32 list_delete(struct __os_list__ *list, struct __os_task__ *ptask)
{
    struct __os_task__ *pprev, *pcurr;
    pprev = list;
    pcurr = list->next;

    if (list == &os_ready_list) {
        while (pcurr != ptask && pcurr != NULL) {
            pprev = pcurr;
            pcurr = pcurr->next;
        }
        assert(pcurr != NULL); /* FIXME: kassert */

        pprev->next       = ptask;
        ptask->next->prev = pprev;


    } else if (list == &os_sleep_list) {

    } else {
        panic();
    }
    return 0;
}
