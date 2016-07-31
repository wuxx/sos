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


s32 blink_task(u32 arg)
{
    u32 count = 0;
    set_gpio_function(16, OUTPUT);
    while(1) {
        PRINT_INFO("in %s %d\n", __func__, count++);
        set_gpio_output(16, 0);     /* led on */
        mdelay(1000);
        set_gpio_output(16, 1);     /* led off */
        mdelay(1000);

    }   
    return 0;
}

#if 1
static s32 test_task(u32 arg)
{
    u32 count = 0;
    while (1) {
        PRINT_INFO("in %s %d\n", __func__, count++);
        mdelay(1000);
    }

    return 0;
}
#endif

PUBLIC s32 main_task(u32 arg)
{
    u32 tid;
    u32 count = 0;
    if ((tid = os_task_create(test_task, 0, 100)) == -1) {
        PRINT_EMG("test_task create failed %d !\n", tid);
    }
    PRINT_EMG("test_task tid %d\n", tid);

    if ((tid = os_task_create(blink_task, 0, 100)) == -1) {
        PRINT_EMG("blink_task create failed %d !\n", tid);
    }
    PRINT_EMG("blink_task tid %d\n", tid);

    while (1) {
        PRINT_INFO("in %s %d\n", __func__, count++);
        os_task_sleep(10*OS_HZ); /* 10 s */
    }
    return 0;
}
