#include <system_config.h>
#include <libc.h>
#include <memory_map.h>
#include <os.h>

#include "cpu.h"
#include "timer.h"
#include "log.h"
#include "int.h"
#include "gpio.h"
#include "uart.h"
#include "mmc.h"

extern struct __os_task__ * tcb_alloc();
extern s32 tcb_init(struct __os_task__ *ptask, func_1 task_entry, u32 arg, u32 priority);
extern s32 main_task(u32 arg);

char sys_banner[] = {"SOS system buildtime [" __TIME__ " " __DATE__ " " "revision "__REVISION__ "]"};

extern struct cpu_context *current_context;

volatile u32 os_tick = 0;
PRIVATE s32 idle_task(u32 arg)
{

    unlock_irq();   /* kick off the system, will switch to the main_task */
    while(1) {
#if 0
        PRINT_INFO("in %s %d cpu_mode: %s; lr: 0x%x; sp: 0x%x; cpsr: 0x%x\n", 
                __func__, __LINE__, get_cpu_mode(NULL), __get_lr(), __get_sp(),  __get_cpsr());

#endif
        /* mdelay(100000); */
#if 0
        PRINT_INFO("in %s %d cpu_mode: %s; lr: 0x%x; sp: 0x%x; cpsr: 0x%x\n", 
                __func__, __LINE__, get_cpu_mode(NULL), __get_lr(), __get_sp(),  __get_cpsr());
#endif
    }
    return 0;
}

PUBLIC s32 os_sleep(u32 ms)
{

    u32 ticks = (ms * OS_HZ) / 1000;
    current_task->sleep_ticks = ticks;

    current_task->state = TASK_SLEEP;
    task_dispatch();
    return 0;
}

PRIVATE void os_clock_irq_hook(struct cpu_context *ctx)
{
    struct __os_task__ *best_task;

    os_tick ++ ;
    
    os_sleep_expire();

    best_task = get_best_task();
    if (best_task == NULL) {
        kassert(current_task == &tcb[IDLE_TASK_ID]);
    } else if (best_task->prio <= current_task->prio) {
        current_task->state = TASK_READY;
        task_dispatch();
    }
}

PRIVATE s32 coretimer_irq_handler(u32 irq_nr)
{
    PRINT_DEBUG("%s start: %d %d\n", __func__, irq_nr, os_tick);
    os_clock_irq_hook(current_context);
    writel(CORETMCLR, 0x0);
    PRINT_DEBUG("%s   end: %d %d\n", __func__, irq_nr, os_tick);
    return 0;
}

PRIVATE s32 coretimer_init()
{
    /* core timer */
    writel(CORETMLOAD, MS2TICK(1000/OS_HZ));
                        /* 23-bit counter & irq enable & timer enable */
    writel(CORETMCTRL, 0x1 << 1 | 0x1 << 5 | 0x1 << 7);
    request_irq(IRQ_CORE_TIMER, coretimer_irq_handler);
    enable_irq(IRQ_CORE_TIMER);
    return 0;
}

s32 os_main(u32 sp)
{
    struct __os_task__ *ptask;

    int_init();
    uart_init();
    timer_init();
    mmc_init();

    PRINT_INFO("%s\n", sys_banner);

    coretimer_init();
    task_init();
    semaphore_init();

    PRINT_INFO("cpu_mode: %s; lr: 0x%x; sp: 0x%x; cpsr: 0x%x\n",
            get_cpu_mode(NULL), __get_lr(), sp, __get_cpsr());

    set_gpio_function(GPIO_16, OUTPUT);
    set_gpio_output(GPIO_16, 0);

    /* set_log_level(LOG_DEBUG); */

    /* create idle task */
    if ((ptask = tcb_alloc()) == NULL) {
        panic();
    }

    tcb_init(ptask, idle_task, 0, 256);

    /*os_ready_insert(ptask);*/

    current_task = &tcb[IDLE_TASK_ID];  /* assume that this is idle_task */

    /* create main task */
    if ((ptask = tcb_alloc()) == NULL) {
        panic();
    }

    tcb_init(ptask, main_task, 0, 100);

    os_ready_insert(ptask);

    /* 'slip into idle task', cause the os_main() is not a task (it's the god code of system) */
    __set_sp(&(task_stack[0][TASK_STK_SIZE]));
    current_task->state = TASK_RUNNING;
    idle_task(0);

    kassert(0);
    return 0;
}

