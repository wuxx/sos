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

char* get_cpu_mode()
{
    u32 cpsr, mode;
    cpsr = __get_cpsr();
    mode = cpsr & 0x1f;
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

u32 test_gpio = 1;

u32 *test_context;
int main(u32 sp)
{
    u32 cpsr;
    u32 pc;
    u32 tid;
    u8  ch;

    int_init();
    uart_init();
    set_log_level(LOG_INFO);
    timer_init();
    os_init();

    PRINT_INFO("%s\n", sys_banner);
    PRINT_INFO("cpu_mode: %s; lr: 0x%x; sp: 0x%x; cpsr: 0x%x\n", 
            get_cpu_mode(), __get_lr(), sp, __get_cpsr());
    set_gpio_function(GPIO_16, OUTPUT);
    set_gpio_output(GPIO_16, 0);
    unlock_irq();
    PRINT_INFO("cpu_mode: %s; lr: 0x%x; sp: 0x%x; cpsr: 0x%x\n", 
            get_cpu_mode(), __get_lr(), sp, __get_cpsr());

    /* 'slip into idle task', cause the main() is not a task (it's the god code of system) */
    PRINT_EMG("%x \n", &(task_stack[0][TASK_STK_SIZE]));
    __set_sp(&(task_stack[0][TASK_STK_SIZE]));
    idle_task();
#if 0
    asm volatile (
            "stmfd sp!, {r0-r12, lr}\n\t"
            "sub sp, sp, #8\n\t"        /* eh... get space to place the user/system mode cpsr, sp */
            "push {r0-r1}\n\t"

            "add r1, sp, #8\n\t"

            "mrs r0, cpsr\n\t"
            "str r0, [r1, #0x4]\n\t"

            "mov r0, r1\n\t"
            "str r0, [r1, #0x0]\n\t"

            "ldr r0, =test_context\n\t"
            "str r1, [r0]\n\t"      /* store the context frame */

            "pop  {r0-r1}\n\t"
            "b .\n\t"
            :   
            :   
            : "memory"
            );  
    while(test_gpio) {
        set_gpio_output(GPIO_16, 1);     /* led off */
        mdelay(1000);
        set_gpio_output(GPIO_16, 0);     /* led on */
        mdelay(1000);
        PRINT_EMG("%d\n", __LINE__);
    };
    while(1) {
    };
#endif
    while(1); /* never reach here */
    return 0;
}
