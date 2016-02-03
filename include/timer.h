#ifndef __TIMER_H__
#define __TIMER_H__

#include <libc.h>

#define CORETM_HZ (1000000) /* 1M, may be not very precise */
#define SYSTM_HZ  (1000000)

#define US2TICK(us)     (us)
#define TICK2US(tick)   (tick)

#define MS2TICK(ms)     (ms*1000)
#define TICK2MS(tick)   (tick/1000)

u64 get_syscounter();
void udelay(u32 us);
void mdelay(u32 ms);
s32 timer_init();
#endif /* __TIMER_H__ */
