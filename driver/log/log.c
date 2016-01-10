#include <libc.h>

#include "log.h"

PRIVATE u32 default_log_level = LOG_INFO;
PRIVATE u8  log_buf[LOG_BUF_SIZE] = {0};

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

PUBLIC s32 log(u32 log_level, char *format, ...)
{
    if (log_level <= default_log_level) {
      va_list args;
      va_start(args,format);
      vsnprintf(log_buf,sizeof(log_buf), format, args);
      va_end(args);
      uart_puts(log_buf);
    }

    return OK;
}
