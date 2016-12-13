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

void __aeabi_unwind_cpp_pr0() {}
void __aeabi_unwind_cpp_pr1() {}

s32 blink_task(u32 arg)
{
    u32 count = 0;
    gpio_set_function(16, OUTPUT);

    while(1) {
        PRINT_INFO("in %s %d;\n", __func__, count++);
        gpio_set_output(16, 0);     /* led on */
        mdelay(1000);
        gpio_set_output(16, 1);     /* led off */
        mdelay(1000);

    }   
    return 0;
}

s32 test_task(u32 arg)
{
    u32 count = 0;
    while (1) {
        PRINT_INFO("in %s %d\n", __func__, count++);
        mdelay(1000);
    }

    return 0;
}

u32 test_flag = 0;
PUBLIC s32 main_task(u32 arg)
{
    u32 tid;
    u32 count = 0;
    s32 sem_id;

    if ((tid = os_task_create(test_task, 0, 100)) == -1) {
        PRINT_EMG("test_task create failed %d !\n", tid);
    }
    PRINT_EMG("test_task tid %d\n", tid);

    if ((tid = os_task_create(blink_task, 0, 100)) == -1) {
        PRINT_EMG("blink_task create failed %d !\n", tid);
    }
    PRINT_EMG("blink_task tid %d\n", tid);

    if ((sem_id = os_semaphore_create(1) == -1)) {
        PRINT_ERR("%s create sem fail! \n", __func__);
    }

    while (1) {
        PRINT_INFO("in %s %d %d\n", __func__, __LINE__, count++);
        os_task_sleep(5*OS_HZ); /* 10 s */
        os_semaphore_get(sem_id);
        PRINT_INFO("in %s %d %d\n", __func__, __LINE__, count++);
#if 0
        if (test_flag == 1) {
            PRINT_INFO("in %s start\n", __func__);
            os_semaphore_put(0);
            PRINT_INFO("in %s   end\n", __func__);
            test_flag = 0;
        }
#endif
    }
    return 0;
}
