#include <system_config.h>
#include <libc.h>
#include <os.h>

#include "log.h"

struct __os_list__ os_ready_list = {.type = OS_READY, .next = NULL, .prev = NULL};
struct __os_list__ os_sleep_list = {.type = OS_SLEEP, .next = NULL, .prev = NULL};

s32 list_insert(struct __os_list__ *list, struct __os_task__ *ptask)
{
    struct __os_task__ *pprev, *pcurr;
    pprev = (struct __os_task__ *)list;
    pcurr = list->next;

    if (list->type == OS_READY ||
        list->type == OS_SEM   ||
        list->type == OS_MBX    ) {
        while (pcurr != NULL && pcurr->prio <= ptask->prio) {
            pprev = pcurr;
            pcurr = pcurr->next;
        }
        pprev->next = ptask;
        ptask->next = pcurr;

        ptask->prev = pprev;

        if (pcurr != NULL) {
            pcurr->prev = ptask;
        }

    } else if (list->type == OS_SLEEP) {
        while (pcurr != NULL && pcurr->sleep_ticks <= ptask->sleep_ticks) {
            pprev = pcurr;
            pcurr = pcurr->next;
        }
        pprev->next = ptask;
        ptask->next = pcurr;

        ptask->prev = pprev;

        if (pcurr != NULL) {
            pcurr->prev = ptask;
        }

    } else {
        panic();
    }
    return 0;
}

s32 list_delete(struct __os_list__ *list, struct __os_task__ *ptask)
{
    struct __os_task__ *pprev, *pcurr;
    pprev = (struct __os_task__ *)list;
    pcurr = list->next;

    PRINT_DEBUG("list delete %x from list %d\n", ptask, list->type);
    while (pcurr != ptask && pcurr != NULL) {
        pprev = pcurr;
        pcurr = pcurr->next;
    }

    kassert(pcurr != NULL);

    pprev->next = ptask->next;

    if (ptask->next != NULL) {
        ptask->next->prev = pprev;
    }
    return 0;
}

