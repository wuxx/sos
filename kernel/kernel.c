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

void os_init()
{
    /*task_create(idle_task, 0);*/
}
