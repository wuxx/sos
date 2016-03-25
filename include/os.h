#ifndef __OS_H__
#define __OS_H__

#define kassert(fmt, ...)     do {log(LOG_EMG,   fmt, ##__VA_ARGS__); assert(0);} while(0)

#include <os_task.h>
#include <os_list.h>
#endif /* __OS_H__ */
