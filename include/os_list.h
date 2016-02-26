#ifndef __OS_LIST_H__
#define __OS_LIST_H__

#include <os_task.h>

struct __os_list__ {
    struct __os_task__ *next;
};

#endif /* __OS_LIST_H__ */
