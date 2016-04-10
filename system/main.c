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
    set_gpio_function(16, OUTPUT);
    while(1) {
        PRINT_INFO("in %s\n", __func__);
        set_gpio_output(16, 0);     /* led on */
        mdelay(1000);
        set_gpio_output(16, 1);     /* led off */

    }   
    return 0;
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

PUBLIC s32 main_task(u32 arg)
{
    u32 tid;
    if ((tid = os_task_create(test_task, 0, 100)) == -1) {
        PRINT_EMG("test_task create failed %d !\n", tid);
    }
    PRINT_EMG("test_task tid %d\n", tid);

    if ((tid = os_task_create(blink_task, 0, 100)) == -1) {
        PRINT_EMG("blink_task create failed %d !\n", tid);
    }
    PRINT_EMG("blink_task tid %d\n", tid);

    while (1) {
        PRINT_STAMP();
        mdelay(1000);
    }
    return 0;
}
