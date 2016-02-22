#include <libc.h>

#include <memory_map.h>
#include <system_config.h>

#include <os_task.h>

#include "mmio.h"
#include "uart.h"
#include "timer.h"
#include "uart.h"
#include "log.h"
#include "gpio.h"
#include "cpu.h"

char sys_banner[] = {"SOS system buildtime [" __TIME__ " " __DATE__ "]"};

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

int main(u32 sp)
{
    u32 cpsr;
    u32 pc;
    u32 tid;
    u8  ch;

    int_init();
    uart_init();
    timer_init();
    set_log_level(LOG_INFO);
    os_init();

    PRINT_INFO("%s\n", sys_banner);
    PRINT_INFO("cpu_mode: %s; lr: 0x%x; sp: 0x%x; cpsr: 0x%x\n", 
            get_cpu_mode(NULL), __get_lr(), sp, __get_cpsr());
    set_gpio_function(GPIO_16, OUTPUT);
    set_gpio_output(GPIO_16, 0);
    PRINT_INFO("cpu_mode: %s; lr: 0x%x; sp: 0x%x; cpsr: 0x%x\n", 
            get_cpu_mode(NULL), __get_lr(), sp, __get_cpsr());

    /* 'slip into idle task', cause the main() is not a task (it's the god code of system) */
#if 0
    char *argv[5] = {"systest", "os", "100"};
    systest(0, argv);
#endif
    __set_sp(&(task_stack[0][TASK_STK_SIZE]));
    unlock_irq();
    idle_task();
    while(1); /* never reach here */
    return 0;
}
