#include <os_task.h>
#include "log.h"

u32 os_tick = 0;

void idle_task()
{
    while(1) {
        PRINT_STAMP();
        mdelay(1);
    }
}


void os_clock_irq_hook()
{
    os_tick ++ ;
    
}

s32 os_init()
{
    if (task_create(idle_task, 0) != OK) {
        PRINT_EMG("idle_task create failed !\n");
        return ERROR;
    }
}
