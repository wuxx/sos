#ifndef __OS_EVENT_H__
#define __OS_EVENT_H__
#include <types.h>

s32 event_wait(u32 task_id, u16 event);
s32 event_release(u32 task_id, u16 event);

#endif /* __OS_EVENT_H__ */
