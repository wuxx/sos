#ifndef __LOG_H__
#define __LOG_H__
#include  <libc.h>

#define LOG_BUF_SIZE 200
enum LOG_LEVEL_E {
    LOG_EMG = 0,
    LOG_ERR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_MAX,
};

s32 log(u32 log_level, char *format, ...);

#define PRINT_EMG(fmt, ...)     log(LOG_EMG,   fmt, ##__VA_ARGS__)
#define PRINT_ERR(fmt, ...)     log(LOG_ERR,   fmt, ##__VA_ARGS__)
#define PRINT_WARN(fmt, ...)    log(LOG_WARN,  fmt, ##__VA_ARGS__)
#define PRINT_INFO(fmt, ...)    log(LOG_INFO,  fmt, ##__VA_ARGS__)
#define PRINT_DEBUG(fmt, ...)   log(LOG_DEBUG, fmt, ##__VA_ARGS__)

#endif /* __LOG_H__ */
