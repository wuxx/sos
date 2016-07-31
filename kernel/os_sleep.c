#include <system_config.h>
#include <libc.h>
#include <os.h>
#include "log.h"

extern struct __os_list__ os_sleep_list;

PUBLIC s32 os_sleep_expire()
{
    struct __os_task__ *pprev, *pcurr;

    pprev = (struct __os_task__ *)&os_sleep_list;
    pcurr = pprev->next;

    /* FIXME: it's a stupid-loop, I am a impatient man */
    while (pcurr != NULL) {
        pcurr->sleep_ticks--;
        if (pcurr->sleep_ticks == 0) {

            if (pcurr->next == NULL) {
                pcurr->prev = NULL;
                pprev->next = NULL;
            } else {
                pprev->next = pcurr->next;
                pcurr->next->prev = pprev;
            }

            pcurr->state = TASK_READY;
            os_ready_insert(pcurr);

            /* pick next task */
            pcurr = pprev->next;

        } else {
            pprev = pcurr;
            pcurr = pcurr->next;
        }
    }

    return 0;
}
