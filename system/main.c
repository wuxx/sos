#include <libc.h>

#include <memory_map.h>
#include <system_config.h>

#include <os.h>

#include "mmio.h"
#include "uart.h"
#include "timer.h"
#include "uart.h"
#include "log.h"
#include "gpio.h"
#include "cpu.h"

char sys_banner[] = {"SOS system buildtime [" __TIME__ " " __DATE__ "]"};

s32 blink_task(u32 arg)
{
    set_gpio_function(16, OUTPUT);
    while(1) {
        PRINT_INFO("in %s\n", __func__);
        set_gpio_output(16, 1);     /* led off */
        mdelay(1000);
        set_gpio_output(16, 0);     /* led on */

    }   
    return 0;
}

char* get_cpu_mode(u32 *m)
{
    u32 cpsr, mode;
    cpsr = __get_cpsr();
    mode = cpsr & 0x1f;

    if (m != NULL) {
        *m = mode;
    }

    switch (mode) {
        case (16):
            return "user mode";
        case (17):
            return "fiq mode";
        case (18):
            return "irq mode";
        case (19):
            return "supervisor mode";
        case (22):
            return "secmonitor mode";
        case (23):
            return "abort mode";
        case (27):
            return "undefined mode";
        case (31):
            return "system mode";
        default:
            return "unknown mode";
    }
}

#if 1
static s32 test_task(u32 arg)
{
    while (1) {
        PRINT_STAMP();
        mdelay(1000);
    }

    return 0;
}
#endif

s32 main_task(u32 arg)
{
    u32 tid;
    if ((tid = os_task_create(test_task, 0, 100)) == -1) {
        PRINT_EMG("test_task create failed %d !\n", tid);
    }
    PRINT_EMG("test_task tid %d\n", tid);

    while (1) {
        PRINT_STAMP();
        mdelay(1000);
    }
    return 0;
}

s32 os_main(u32 sp)
{
    u32 cpsr;
    u32 pc;
    u32 tid;

    u8  ch;
    struct __os_task__ *ptask;

    int_init();
    uart_init();
    timer_init();

    /* os_init(); */

    PRINT_INFO("%s\n", sys_banner);
    PRINT_INFO("cpu_mode: %s; lr: 0x%x; sp: 0x%x; cpsr: 0x%x\n",
            get_cpu_mode(NULL), __get_lr(), sp, __get_cpsr());
    set_gpio_function(GPIO_16, OUTPUT);
    set_gpio_output(GPIO_16, 0);
    PRINT_INFO("cpu_mode: %s; lr: 0x%x; sp: 0x%x; cpsr: 0x%x\n",
            get_cpu_mode(NULL), __get_lr(), sp, __get_cpsr());

    /* set_log_level(LOG_DEBUG); */

    coretimer_init();
    PRINT_STAMP();
#if 0
    if (task_create(idle_task, 0, 100) != 0) {
        PRINT_EMG("idle_task create failed !\n");
        return ERROR;
    }
#endif

    /* create idle task */
    if ((ptask = tcb_alloc()) == NULL) {
        panic();
    }

extern s32 idle_task(u32 arg);

    tcb_init(ptask, idle_task, 0, 100);

    /*os_ready_insert(ptask);*/

    current_task = &tcb[0];  /* idle_task */
    PRINT_STAMP();

#if 0
    /* FIXME: create main() task */
    if (task_create(blink_task, 0, 100) == -1) {
        PRINT_EMG("blink_task create failed !\n");
    }


    if ((task_create(main_task, 0, 100)) == -1) {
        PRINT_EMG("blink_task create failed !\n");
    }
#endif

    /* create main task */
    if ((ptask = tcb_alloc()) == NULL) {
        panic();
    }

    tcb_init(ptask, main_task, 0, 50);

    os_ready_insert(ptask);

    PRINT_STAMP();

    /* 'slip into idle task', cause the os_main() is not a task (it's the god code of system) */
    __set_sp(&(task_stack[0][TASK_STK_SIZE]));
    current_task->state = TASK_RUNNING;
    idle_task(0);

    kassert(0);
#if 0
    PRINT_EMG("tid %d !\n", tid);
    __set_sp(&(task_stack[0][TASK_STK_SIZE]));
    current_task->state = TASK_RUNNING;
    idle_task();
    while(1); /* never reach here */
#endif
    return 0;
}
