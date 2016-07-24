#include <libc.h>

#include "log.h"
#include "int.h"
#include "uart.h"

PRIVATE u32 default_log_level = LOG_INFO;
PRIVATE char format_buf[FORMAT_BUF_SIZE] = {0};

u8 log_buffer[1*1024*1024] = {0};
u32 lbindex = 0;

PUBLIC s32 set_log_level(u32 log_level)
{
    if ((log_level >= LOG_EMG) && (log_level <= LOG_DEBUG)) {
        default_log_level = log_level;
        return OK;
    } else {
        return EINVAL;
    }

    return 0;
}

PUBLIC s32 log(u32 log_level, const char *format, ...)
{
    u32 len;
    va_list args;

    va_start(args, format);
    len = vsnprintf(format_buf,sizeof(format_buf), format, args);
    va_end(args);

    if (log_level <= default_log_level) {
        /*lock_irq(); */
        uart_puts(format_buf);
        /*unlock_irq();*/
    }

    if (len > (sizeof(log_buffer) - (lbindex + 1))) {
        lbindex = 0;
    }

    memcpy(&log_buffer[lbindex], format_buf, len);
    lbindex += len;

    return OK;
}

PUBLIC s32 dump_log()
{
    u32 i;
#if 0
    for(i = 0; i < sizeof(log_buffer); i++) {
        uart_putc(log_buffer[i]);
    }
#endif

    return 0;
}
