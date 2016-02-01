#ifndef __TIMER_H__
#define __TIMER_H__

#include <libc.h>

#define CORETM_HZ (1000000) /* 1M, may be not very precise */
#define SYSTM_HZ  (1000000)

#define MS2TICK(ms)     (ms*1000)
#define TICK2MS(tick)   (tick/1000)

void delay(s32 count);
s32 timer_init();
u64 get_syscounter();
#endif /* __TIMER_H__ */
